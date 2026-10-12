---
name: regs-description
description: Add, change or remove a register in main/regs.h or a user_tasks/*.h struct and regenerate the regs_description tables used by Modbus, HTTP and NVM. Use whenever a #generator_use_description struct changes.
---

# Register descriptions

## Where things live
- Main table: `main_vars_t` in `main/regs.h` (address_space 0). Defaults (`def_*`, `def_table_version`) in `main/regs_description.c`.
- Task tables: the marked union in `user_tasks/<task>.h`. Defaults (`def_*`) and `def_table_version` in `user_tasks/<task>.c`, outside the generated block.
- A table is the union between `/** #generator_use_description {"space_name":..., "address_space":N, "modbus_type":"server", "modbus_function":"holding_registers", "register_start_address":A}*/` and `}<type>; // #generator_use_description {"message":"end_struct"}`, followed by `extern <type> * const <global>;` (required, the parser finds the global by it).

## Register line format
One member per line inside `vars`:

    <type> <name>[<size>];   //!< "<description>" &flags

- Types: u8 u16 u32 u64 s8 s16 s32 s64 int float double. Size is a decimal or a `#define` with a decimal value in the same header.
- Flags: `&ro` read only, `&save` persisted to NVM, `&def` default from `def_<name>`, `&min`/`&max` from `def_min_<name>`/`def_max_<name>`, `&credential`, `&crtcl` (restart after change, informational only).
- Description characters allowed: letters, digits, space, `-()[],!;.><*:#=`. Anything else drops the description silently.

## Steps
1. Edit the struct only. Keep names unique across tables and under 32 chars (`REGS_MAX_NAME_SIZE`). Append new registers at the end unless the Modbus map must change; addresses follow byte offset.
2. For every `&def`/`&min`/`&max`, add the matching `const` next to the existing defaults.
3. If saved layout or defaults changed, bump `def_table_version` for that table so devices reset to defaults instead of reading stale NVM.
4. Regenerate from `generator/`:
   - main: `python regs_generator.py --no-user-tasks`
   - tasks: `python regs_generator.py --user-tasks-only [--user-task-header <task>.h]`
   `idf.py build` also runs both through `main/CMakeLists.txt`.
5. Check output: no `error` lines; `find N own network variable` matches the expected count; `NUM_OF_MAIN_VARS` or `NUM_OF_<GLOBAL>_VARS` updated; for tasks `injected user task regs table into ...` is printed.
6. Review `generator/address_space_<type>.rst` for the new Modbus addresses.

## Do not
- Hand-edit generated regions: `regs_description_global[]` body, `NUM_OF_*`, `regs_main_t`, the `regs_pointers` block in `regs.c`, `FW_VERSION*` in `regs.h`, the `user_task_regs` block in task `.c` files.
- Nest structs (only `structured_time_t` and `task_info_t` are special-cased); a `}` ends the table.
- Grow `main_vars_t` beyond `GLOBAL_UNION_SIZE` (256 bytes), or let a task struct outgrow its `bytes[]` member.
- Reuse an `address_space` or overlap `register_start_address` ranges; addresses must keep rising.
