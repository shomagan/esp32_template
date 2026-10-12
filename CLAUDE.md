# esp32_template

ESP-IDF v6.0 project (`C:\esp\v6.0\esp-idf`). Build shell profile: `C:\Espressif\tools\Microsoft.v6.0.PowerShell_profile.ps1`.

## Rules
- Propose every change and wait for approval, file by file.
- No git write operations (commit, stage/unstage, reset, checkout).
- Avoid explanatory comments; code should describe behavior. Exception: `//!<` register comments and `#generator_use_description` markers are inputs to the generator and are required.

## Code style
- C formatting is defined by `.clang-format` and `.editorconfig`: 3-space indent, no tabs, 131-column limit, attached braces, `type *name` pointers, `* const` with spaces, braces on every control statement, UTF-8, final newline, no trailing whitespace.
- Format only the lines you change (`clang-format --lines` or `git clang-format`); never reformat a whole existing file, since much legacy code uses 4-space indent and a full reformat buries the real diff.
- Register tables can be clang-formatted: the generator joins wrapped declarations and accepts `*const`. Keep one register per declaration with its `//!<` comment on the same line as the `;`.
- Naming: snake_case for functions and variables, `_t` suffix for typedefs, UPPER_CASE for macros and enum constants, module prefix on public functions (`regs_`, `scd41_`), `static const char *TAG = "<module>";` for logging.
- Types from `type_def.h` (`u8 u16 u32 u64 s8 s16 s32 s64`) rather than `<stdint.h>` names.
- Headers: `#ifndef NAME_H` / `#define NAME_H 1` guard and an `extern "C"` block.
- User tasks call OS services only through `link_functions.*`.

## Layout
- `main/`: OS core. `regs.h` holds `main_vars_t` (address space 0); `regs_description.c` holds main defaults and the generated table.
- `user_tasks/`: one `.h/.c` per task, each with its own register table; compiled when `USER_TASK_*` is ON in root `CMakeLists.txt`.
- `generator/`: `regs_generator.py` + `regs_handl.py` turn register structs into `regs_description_t` tables used by Modbus, HTTP/JSON and NVM. `main/CMakeLists.txt` runs them during `idf.py build`.

For register work use the `regs-description` skill; for a new task use `add-user-task`.
