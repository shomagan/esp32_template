---
name: add-user-task
description: Create a new user task in user_tasks/ with its own register table, wired into CMake and user_tasks_init.c. Use when asked to add a task, sensor or module under user_tasks.
---

# Add a user task

1. From repo root: `python add_user_task.py -n <name> [--stack N] [--period ms]`.
   It creates `user_tasks/<name>.h/.c`, picks the next free `address_space` and `register_start_address`, and wires root `CMakeLists.txt` option `USER_TASK_<NAME>` (OFF), `user_tasks/CMakeLists.txt`, `user_tasks/user_tasks_init.c`, and `USER_TASK_HDR_PATHS`/`USER_TASK_C_PATHS` in `main/CMakeLists.txt`.
2. In `<name>.c`, above the `user_task_regs` markers, add `static const u32 def_table_version = 1;` (the generated `regs_table_<name>_reg` references it), and mark `table_version` in the header with `&def`.
3. Add registers to the header struct per the `regs-description` skill. Keep `table_version` as the last member and make sure `bytes[]` covers the struct.
4. Keep `space_name` distinct within its first 15 characters (NVS namespace limit).
5. Run `cd generator && python regs_generator.py --user-tasks-only --user-task-header <name>.h`.
6. Enable with `option(USER_TASK_<NAME> ... ON)` in root `CMakeLists.txt` and build.
7. Task code calls OS functions only through `link_functions.*`. Registration happens in `<name>_init`: `regs_description_list_add_new(regs_table_<name>_reg)` then `preinit_table_vars(ind)`, which restores defaults when the stored `table_version` differs.
