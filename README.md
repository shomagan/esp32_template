# esp32_template

ESP-IDF firmware template: an OS core in `main/` plus optional user tasks in `user_tasks/`. All of the
device's state is held in register structs. A generator turns them into description tables, and Modbus,
HTTP/JSON and NVM use those tables to reach every register in the same way.

- ESP-IDF v6.0 (`C:\esp\v6.0\esp-idf`), default target `esp32c3`
- Build shell profile: `C:\Espressif\tools\Microsoft.v6.0.PowerShell_profile.ps1`

## Layout

| Path          | Content |
|---------------|---------|
| `main/`       | OS core: Wi-Fi control, Modbus TCP server and client, HTTP server, UDP broadcast, SLIP, Telegram, NVM mirror storage. `regs.h` holds `main_vars_t`. |
| `user_tasks/` | One `.h/.c` pair per task, each with its own register table. A task is built only when its `USER_TASK_*` option is `ON` in the root `CMakeLists.txt`. |
| `generator/`  | `regs_generator.py` + `regs_handl.py`: build `regs_description_t` tables from the register structs. |
| `updater/`    | Separate recovery app for web firmware updates (`updater` partition in `partitions.csv`). |
| `test/unit/`  | Host unit tests that run in Docker without a board, not ready yet. |
| `test/outside/` | Tests that run against a flashed device on the network. |

Features are switched on and off in `main/main_config.h` (`SLIP_ENABLE`, `MODBUS_MASTER_ENABLE`, `TELEGRAM`, ...).

## Build and flash

```
idf.py set-target esp32c3     # once
idf.py build
idf.py -p COMx flash monitor
```

`idf.py build` runs the build: `main/CMakeLists.txt` calls `regs_generator.py --no-user-tasks` and
`--user-tasks-only`. This rewrites the generated blocks in `main/regs_description.c`, `main/regs.c` and
`user_tasks/*.c`.

## Registers

To add a register, put a field with a `//!<` comment into the struct (`main/regs.h` or a `user_tasks/*.h`
struct marked `#generator_use_description`) and build. The flags in the comment (`&ro`, `&save`, `&def`,
`&min`, `&max`, `&credential`, `&crtcl`) set access, flash saving and default values. Modbus addresses
follow the field's byte offset within the struct.

## User tasks

Currently user tasks reach OS services not only through `link_functions.*` but in future will be.

To add a new task:
```
python add_user_task.py -n fan_control [--stack 2464] [--period 1000]
```
This creates `user_tasks/fan_control.{h,c}` with an empty register table and picks the next free address
space and start address. It also adds the `USER_TASK_FAN_CONTROL` option and wires the task into
`user_tasks/CMakeLists.txt`, `user_tasks_init.c` and `main/CMakeLists.txt`.

## Unit tests (host, Docker)

These tests need Docker Desktop but no board. They run in the `espressif/idf:v6.0` image, and the repo is
mounted read-only.

```sh
docker compose -f test/unit/docker-compose.yml run --rm ut
```

`test/unit/run.sh` runs two suites. The command returns non-zero if either one fails.

- **generator**: pytest in `test/unit/generator/`. It covers `regs_generator.py` / `regs_handl.py` on a temp
  copy of the tree: flag parsing, addresses, error paths, idempotence and a golden check against the
  committed generated code.
- **regs_description**: Unity tests in `test/unit/regs_app/`, built for the ESP-IDF `linux` target. They
  cover the lookup, default, flag and string functions in `main/regs_description.c` and check that the
  generated table is consistent.

The first run builds the image, which takes a few minutes. After you change the `Dockerfile`, rebuild with
`docker compose -f test/unit/docker-compose.yml build`.

## Device tests (outside)

These tests need a flashed board that is reachable over the network. See `test/outside/README.md` for
the full option list and the safety rules.

```sh
cd test/outside
python3 -m pip install -r requirements.txt
python3 -m pytest --device-ip 192.168.1.232 [--device-port 502] [--unit-id 14]
```

`test_modbus_tcp.py` reads register addresses from the generated `main/regs_description.c`. It reads
registers, checks for the Modbus exception on a read-only write and writes `table_id`, then restores
every value it wrote.

`udp_broadcast_server.py` is a manual tool. It listens on UDP port 65500 and prints the device's
broadcast packets. Press `q` to quit.
