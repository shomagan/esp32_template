# Task: host unit tests in Docker, starting with register descriptions

Status: draft, not started

## Goal
Run unit tests on a PC (no ESP32 board) inside a Docker container with one command, and use them first to
cover the register description system: the generator scripts, the tables they produce, and the lookup
code in `main/regs_description.c`.

## Scope of phase 1
1. Generator (Python): `generator/regs_generator.py`, `generator/regs_handl.py`.
2. Generated output: `regs_description_global[]` in `main/regs_description.c`, `NUM_OF_MAIN_VARS`,
   the `regs_pointers` block in `main/regs.c`, and the `user_task_regs` blocks injected into `user_tasks/*.c`.
3. C runtime: the `regs_description_*` functions in `main/regs_description.c` (lookup by index, name,
   guid, address and Modbus address; flag checks; default values; string formatting; multi-table list).

Out of scope for phase 1: Modbus, HTTP/JSON, NVM/`mirror_storage`, Wi-Fi, FreeRTOS behaviour, anything
needing hardware.

## Environment
- Docker image: official `espressif/idf` from Docker Hub, tag matching the project (`v6.0`; confirm the tag
  exists, else the closest v6.x). ESP-IDF, Python and the host toolchain come with the image.
- C tests use the ESP-IDF `linux` target (`idf.py --preview set-target linux`, check whether v6.0 still needs
  `--preview`): the test app builds as a Linux executable with POSIX FreeRTOS, real `esp_log`/`esp_check`
  and the IDF `unity` component.
- `test/unit/` holds everything: `docker-compose.yml` (or a `run.ps1`/`run.sh` wrapper), the IDF test app
  for C tests, pytest tests for the generator, and any stubs or mocks. `test/outside/` stays as is.
- The repo is bind-mounted read-only; tests that run the generator work on a copy in a temp dir,
  because the generator rewrites sources in place (`regs_description.c`, `regs.c`, task `.c` files,
  `regs_sha256.txt`, `version_from_git.txt`) and calls `makefsdata`. Build output goes to a container volume,
  never to the repo's `build/`.
- One entry point from Windows: `docker compose -f test/unit/docker-compose.yml run --rm ut`
  returns non-zero on any failure.

## Python tests (pytest)
- Parse a small fixture header with `#generator_use_description` structs and check the produced
  `regs_description_t` rows: name, type, size, property flags, guid, Modbus address/function, defaults.
- Each `//!<` flag: `&ro`, `&save`, `&def`, `&min`, `&max`, `&credential`, `&crtcl`.
- Array sizes from a decimal and from a `#define`.
- Description character rules (allowed set kept; a disallowed char drops the description).
- Addresses follow byte offset; `register_start_address` and `address_space` applied.
- Error paths: duplicate names, names over 32 chars, nested structs, missing `extern <type> * const`.
- `--no-user-tasks` and `--user-tasks-only [--user-task-header X.h]` touch only the files they should.
- Idempotence: running the generator twice on the real tree leaves no diff.
- Golden check: generating from the current `main/regs.h` and `user_tasks/*.h` reproduces the
  committed generated blocks byte for byte.

## C tests (Unity, ESP-IDF linux target)
- A separate IDF project under `test/unit/` whose component compiles only `main/regs_description.c` and the
  few sources it needs; the production `main` component is not built, since its Wi-Fi, BT and driver
  dependencies do not exist on the linux target.
- What linux lacks gets a CMock mock or a small stub: `mirror_storage.h` and `credentials.h` (gitignored,
  so a test copy is needed); check how `os_type.h` semaphores map onto POSIX FreeRTOS.
  No change to production code unless unavoidable; any such change is proposed first.
- `regs_description_get_by_index/name/guid/address`, `regs_description_get_index_by_name/address`:
  found, not found, first and last entry, entries in a user table.
- Binary search assumptions: `regs_description_global[]` sorted by guid and by address.
- `regs_description_list_add_new`: adds tables up to `LIST_DESCRIPTIONS_SIZE`, rejects beyond it;
  per-table getters return what was added.
- `regs_description_get_pointer_by_modbus` for each Modbus function used.
- `set_regs_def_values`: every `&def` register gets its default, others untouched.
- `regs_description_is_writable/is_credential/flag_check`.
- `regs_description_get_regs_string_value` for each type, arrays, and a buffer too small.
- `regs_description_write_value_by_address`, `regs_fill/check/clear_temp_buffer`.
- Table consistency over the real generated table: unique names and guids, `NUM_OF_MAIN_VARS`
  matches the array, `main_vars_t` fits `GLOBAL_UNION_SIZE`, every `p_value` lies inside the struct.

## Acceptance
- One command pulls the image, builds the test app and runs both suites; summary shows pass/fail counts.
- Tests pass on the current tree, or each failure is a real bug written up in `todo.txt` style
  rather than worked around.
- Nothing in `main/`, `user_tasks/` or `generator/` changes without approval, file by file.
- New C code follows `.clang-format`; register tables keep one register per line with `//!<`.

## Open questions
1. Location `test/unit/` OK, or prefer something else?
2. Generator tests in pytest inside the same image, OK?
3. Should a CI job (GitHub Actions) run the same container in a later phase?
4. Next areas after phase 1: Modbus frame handling, JSON, `crc.c`?
