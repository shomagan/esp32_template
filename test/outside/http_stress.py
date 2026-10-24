"""Long-running HTTP stress test for the device web server.

Runs N parallel clients that keep requesting static pages and JSON register
reads (random registers, 1..10 per request, "full" or "only_value") for the
given duration. Every request goes to a CSV file; time statistics (ms) and
response sizes are printed and accumulated per window and for the whole run.
Each window also reads free heap and live time over Modbus to catch leaks
and reboots.

Example:
    py http_stress.py --device-ip 192.168.100.26 --clients 5 --duration-min 65
"""

import argparse
import csv
import json
import random
import socket
import statistics
import threading
import time
from collections import defaultdict
from datetime import datetime
from pathlib import Path

STATIC_PAGES = ["/", "/favicon.ico", "/wifi_settings.html"]
INFO_REQUESTS = ["regs_num", "user_task_regs_num", "os_param"]
REG_COUNTS = [1, 2, 3, 5, 8, 10]
GET_TYPES = ["full", "only_value"]

CSV_FIELDS = ["time", "client", "name", "ok", "status", "body_len", "connect_ms", "ttfb_ms", "total_ms", "overflow",
              "indexes", "error"]


def http_get(host, path, json_header, timeout):
    headers = f"GET {path} HTTP/1.1\r\nHost: {host}\r\nConnection: close\r\n"
    if json_header is not None:
        headers += f"Json: {json.dumps(json_header, separators=(',', ':'))}\r\nAccept: text/html\r\n"
    headers += "User-Agent: http_stress/1.0 " + "x" * 300 + "\r\n\r\n"

    result = {"ok": False, "status": "", "body_len": 0, "connect_ms": None, "ttfb_ms": None, "total_ms": None,
              "overflow": False, "error": "", "json": None}
    start = time.perf_counter()
    try:
        with socket.create_connection((host, 80), timeout=timeout) as s:
            result["connect_ms"] = (time.perf_counter() - start) * 1000
            s.sendall(headers.encode())
            data = b""
            while True:
                chunk = s.recv(4096)
                if not chunk:
                    break
                if not data:
                    result["ttfb_ms"] = (time.perf_counter() - start) * 1000
                data += chunk
    except OSError as e:
        result["total_ms"] = (time.perf_counter() - start) * 1000
        result["error"] = type(e).__name__ + ": " + str(e)
        return result
    result["total_ms"] = (time.perf_counter() - start) * 1000

    head, _, body = data.partition(b"\r\n\r\n")
    lines = head.decode(errors="replace").split("\r\n")
    result["status"] = lines[0] if lines else ""
    result["body_len"] = len(body)
    content_length = None
    for line in lines[1:]:
        if line.lower().startswith("content-length:"):
            content_length = int(line.split(":", 1)[1])

    if " 200 " not in result["status"]:
        result["error"] = "bad status"
    elif content_length is not None and content_length != len(body):
        result["error"] = f"short body {len(body)}/{content_length}"
    elif json_header is not None:
        try:
            result["json"] = json.loads(body)
            result["overflow"] = "error" in result["json"]
        except ValueError:
            result["error"] = "invalid json"
    result["ok"] = result["error"] == ""
    return result


def check_regs_response(result, indexes):
    """A "regs" answer must hold the last requested index (duplicate keys keep the last one)."""
    js = result["json"]
    if result["ok"] and not result["overflow"] and js.get("index") != indexes[-1]:
        result["ok"] = False
        result["error"] = f"wrong index {js.get('index')} expected {indexes[-1]}"


class RequestPlan:
    """Builds the next request for a client: static page, info request or register read."""

    def __init__(self, total_regs, seed):
        self.total_regs = total_regs
        self.rng = random.Random(seed)

    def next(self):
        kind = self.rng.random()
        if kind < 0.25:
            path = self.rng.choice(STATIC_PAGES)
            return path, path, None, []
        if kind < 0.35:
            req = self.rng.choice(INFO_REQUESTS)
            return f"json {req}", "/get_json.cgi", {"request": req}, []
        count = self.rng.choice(REG_COUNTS)
        get_type = self.rng.choice(GET_TYPES)
        indexes = [self.rng.randrange(self.total_regs) for _ in range(count)]
        pack = {"request": "regs", "reg_num": str(count), "get_type": get_type,
                "index": str(indexes[0]) if count == 1 else indexes}
        return f"regs n={count:<2} {get_type}", "/get_json.cgi", pack, indexes


class Stats:
    """Collects total_ms and body size per request name, for one window or the whole run."""

    def __init__(self):
        self.times = defaultdict(list)
        self.sizes = defaultdict(list)
        self.errors = defaultdict(int)
        self.overflows = defaultdict(int)
        self.error_kinds = defaultdict(int)

    def add(self, name, result):
        if result["ok"]:
            self.times[name].append(result["total_ms"])
            self.sizes[name].append(result["body_len"])
            if result["overflow"]:
                self.overflows[name] += 1
        else:
            self.errors[name] += 1
            self.error_kinds[result["error"].split(":")[0]] += 1

    def lines(self):
        out = [f"{'request':<24}{'ok':>7}{'err':>5}{'ovf':>6}{'bytes':>7}"
               f"{'min':>7}{'avg':>7}{'p50':>7}{'p95':>7}{'p99':>7}{'max':>7}"]
        all_times = []
        for name in sorted(set(self.times) | set(self.errors)):
            t = sorted(self.times[name])
            all_times += t
            size = f"{statistics.mean(self.sizes[name]):7.0f}" if self.sizes[name] else f"{'-':>7}"
            out.append(f"{name:<24}{len(t):7d}{self.errors[name]:5d}{self.overflows[name]:6d}{size}{self.time_row(t)}")
        out.append(f"{'ALL':<24}{len(all_times):7d}{sum(self.errors.values()):5d}{sum(self.overflows.values()):6d}"
                   f"{'':>7}{self.time_row(sorted(all_times))}")
        if self.error_kinds:
            out.append("errors: " + ", ".join(f"{k}={v}" for k, v in sorted(self.error_kinds.items())))
        return out

    @staticmethod
    def time_row(t):
        if not t:
            return f"{'-':>7}" * 6
        q = statistics.quantiles(t, n=100) if len(t) > 1 else [t[0]] * 99
        return f"{min(t):7.0f}{statistics.mean(t):7.0f}{q[49]:7.0f}{q[94]:7.0f}{q[98]:7.0f}{max(t):7.0f}"


class DeviceMonitor:
    """Reads free heap and live time over Modbus; returns None values if Modbus is not available."""

    NAMES = ("cur_free_heap", "min_free_heap", "live_time")

    def __init__(self, host, unit_id):
        self.host = host
        self.unit_id = unit_id
        try:
            from regs_table import load_main_table
            regs = load_main_table()
            self.regs = {n: regs[n] for n in self.NAMES}
        except Exception as e:
            print(f"modbus monitor disabled: {e}")
            self.regs = None

    def read(self):
        values = {n: None for n in self.NAMES}
        if self.regs is None:
            return values
        try:
            from pymodbus.client import ModbusTcpClient
            client = ModbusTcpClient(self.host, timeout=3)
            if client.connect():
                for n, r in self.regs.items():
                    resp = client.read_holding_registers(r.modbus_address, count=r.word_count, device_id=self.unit_id)
                    if not resp.isError():
                        values[n] = sum(w << (16 * i) for i, w in enumerate(resp.registers))
            client.close()
        except Exception as e:
            print(f"modbus read failed: {e}")
        return values


def read_total_regs(host, timeout):
    total = 0
    for req, key in (("regs_num", "regs_num"), ("user_task_regs_num", "user_task_regs_num")):
        result = http_get(host, "/get_json.cgi", {"request": req}, timeout)
        if not result["ok"]:
            raise SystemExit(f"cannot read {req}: {result['error'] or result['status']}")
        total += int(result["json"][key])
    return total


def client_loop(client_id, args, plan, stop, on_result):
    while not stop.is_set():
        name, path, pack, indexes = plan.next()
        result = http_get(args.device_ip, path, pack, args.timeout)
        if indexes:
            check_regs_response(result, indexes)
        on_result(client_id, name, result, indexes)
        if not result["ok"]:
            stop.wait(args.error_pause_s)
        elif args.interval_ms:
            stop.wait(args.interval_ms / 1000)


def main():
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--device-ip", default="192.168.100.26")
    parser.add_argument("--unit-id", type=int, default=14, help="modbus unit id for the heap/live time monitor")
    parser.add_argument("--clients", type=int, default=5)
    parser.add_argument("--duration-min", type=float, default=65.0)
    parser.add_argument("--interval-ms", type=int, default=0, help="pause of each client between requests")
    parser.add_argument("--summary-s", type=int, default=60, help="statistics window")
    parser.add_argument("--timeout", type=float, default=10.0, help="socket timeout per request, s")
    parser.add_argument("--error-pause-s", type=float, default=1.0, help="client pause after a failed request")
    parser.add_argument("--seed", type=int, default=1, help="random seed of the request mix")
    parser.add_argument("--out-dir", default=str(Path(__file__).parent / "stress_logs"))
    args = parser.parse_args()

    total_regs = read_total_regs(args.device_ip, args.timeout)
    monitor = DeviceMonitor(args.device_ip, args.unit_id)

    out_dir = Path(args.out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    stamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    csv_path = out_dir / f"http_stress_{stamp}.csv"
    summary_path = out_dir / f"http_stress_{stamp}_summary.txt"
    device_path = out_dir / f"http_stress_{stamp}_device.csv"

    lock = threading.Lock()
    window = Stats()
    total = Stats()
    stop = threading.Event()
    csv_file = open(csv_path, "w", newline="")
    writer = csv.DictWriter(csv_file, fieldnames=CSV_FIELDS)
    writer.writeheader()
    with open(device_path, "w", newline="") as f:
        csv.writer(f).writerow(["time", *DeviceMonitor.NAMES])

    def on_result(client_id, name, result, indexes):
        row = {"time": datetime.now().isoformat(timespec="milliseconds"), "client": client_id, "name": name,
               "indexes": " ".join(map(str, indexes))}
        row.update({k: (f"{v:.1f}" if isinstance(v, float) else v) for k, v in result.items() if k in CSV_FIELDS})
        with lock:
            writer.writerow(row)
            window.add(name, result)
            total.add(name, result)

    last_live_time = [None]

    def report(title, stats):
        dev = monitor.read()
        with open(device_path, "a", newline="") as f:
            csv.writer(f).writerow([datetime.now().isoformat(timespec="seconds"), *dev.values()])
        dev_line = "device: " + ", ".join(f"{k}={v}" for k, v in dev.items())
        if dev["live_time"] is not None and last_live_time[0] is not None and dev["live_time"] < last_live_time[0]:
            dev_line += "  REBOOT DETECTED"
        if dev["live_time"] is not None:
            last_live_time[0] = dev["live_time"]
        text = "\n".join([f"=== {datetime.now():%Y-%m-%d %H:%M:%S} {title} ==="] + stats.lines() + [dev_line]) + "\n"
        print(text, flush=True)
        with open(summary_path, "a") as f:
            f.write(text)

    header = (f"device={args.device_ip} clients={args.clients} duration={args.duration_min} min "
              f"interval={args.interval_ms} ms timeout={args.timeout} s seed={args.seed} total_regs={total_regs}\n"
              f"csv={csv_path}\n")
    print(header, flush=True)
    summary_path.write_text(header)
    report("start", Stats())

    threads = [threading.Thread(target=client_loop, args=(c, args, RequestPlan(total_regs, args.seed * 100 + c), stop,
                                                          on_result), daemon=True)
               for c in range(args.clients)]
    for t in threads:
        t.start()

    start = time.monotonic()
    end = start + args.duration_min * 60
    try:
        while time.monotonic() < end:
            time.sleep(min(args.summary_s, max(0.0, end - time.monotonic())))
            with lock:
                csv_file.flush()
                current = window
                window = Stats()
            report(f"window {args.summary_s} s, elapsed {(time.monotonic() - start) / 60:.1f} min", current)
    except KeyboardInterrupt:
        print("interrupted", flush=True)
    stop.set()
    for t in threads:
        t.join(args.timeout + args.error_pause_s + 1)
    with lock:
        csv_file.close()
    report(f"TOTAL, elapsed {(time.monotonic() - start) / 60:.1f} min", total)
    return 1 if sum(total.errors.values()) else 0


if __name__ == "__main__":
    raise SystemExit(main())
