# Modbus TCP tests against a running device

Pytest suite that talks to a flashed ESP32 over Modbus TCP (port 502). Register addresses, types and
read-only flags are parsed from `main/regs_description.c` (the generated `regs_description_global[]` table)
and `main/regs.h`, so the tests follow whatever the generator last produced.

## Setup

```powershell
cd test\outside
py -m pip install -r requirements.txt
```

## Run

```powershell
py -m pytest --device-ip 192.168.1.232
```

| Option          | Environment   | Default                       |
|-----------------|---------------|-------------------------------|
| `--device-ip`   | `MODBUS_IP`   | `ip_address` from `config.py` |
| `--device-port` | `MODBUS_PORT` | `502`                         |
| `--unit-id`     | `MODBUS_UNIT` | `14` (`def_mdb_addr`)         |

The unit id must equal the device's `mdb_addr`; the firmware ignores requests for other ids, which shows up
as timeouts.

## Safety

Tests only write registers that are not saved to flash, not `&crtcl`, not credentials and not `command`, so
no test triggers a restart, a Wi-Fi change or a flash write. Every value written is restored to what it was
before the test, even when the test fails.

## What is tested

- TCP connection to the device.
- The parsed table has `NUM_OF_MAIN_VARS` rows and the flags the tests rely on.
- `mdb_addr` reads back as the unit id.
- `fw_version` equals `FW_VERSION` in `regs.h` (the device runs firmware built from this tree).
- `sys_tick_counter` advances between two reads.
- Writing the read-only `unused_3` returns exception 2 (illegal data address) and leaves it unchanged.
- `table_id` (writable, not saved to flash) is written with function 6, read back, then restored with
  function 16.
