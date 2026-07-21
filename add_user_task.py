import argparse
import glob
import json
import os
import re
import sys

__description__ = 'add a new user task (.c/.h pair) and wire it into the build'

USER_TASKS_DIR = "user_tasks"
ROOT_CMAKE = "CMakeLists.txt"
USER_TASKS_CMAKE = USER_TASKS_DIR + "/CMakeLists.txt"
USER_TASKS_INIT = USER_TASKS_DIR + "/user_tasks_init.c"
MAIN_CMAKE = "main/CMakeLists.txt"

GENERATOR_MARKER = "#generator_use_description"


def main():
    parser = argparse.ArgumentParser(description=__description__)
    parser.add_argument('-n', '--name', type=str, required=True,
                         help='task base name, e.g. "fan_control" (no extension)')
    parser.add_argument('--stack', type=int, default=2464,
                         help='task stack size passed to os_thread_create (default: %(default)s)')
    parser.add_argument('--period', type=int, default=1000,
                         help='task loop period in ms (default: %(default)s)')
    args = parser.parse_args()

    name = args.name.strip().lower()
    if not re.match(r'^[a-z][a-z0-9_]*$', name):
        print("name must be a valid lowercase C identifier (letters, digits, underscore)")
        exit()

    define_name = name.upper()
    cmake_option = "USER_TASK_" + define_name
    struct_type = f"{name}_reg_t"
    space_name = struct_type

    if not os.path.exists(USER_TASKS_DIR):
        print("path not exist:", USER_TASKS_DIR)
        exit()

    c_path = f"{USER_TASKS_DIR}/{name}.c"
    h_path = f"{USER_TASKS_DIR}/{name}.h"
    if os.path.exists(c_path) or os.path.exists(h_path):
        print("file already exist")
        exit()

    address_space, register_start_address = pick_next_address_space_and_register()

    write_header(h_path, name, define_name, struct_type, space_name, address_space, register_start_address)
    write_source(c_path, name, define_name, args.period)
    add_root_cmake_option(cmake_option, define_name)
    add_user_tasks_cmake_entry(name, define_name)
    add_user_tasks_init_entry(name, define_name, args.stack)
    add_main_cmake_regs_paths(name)

    print(f"created {h_path}")
    print(f"created {c_path}")
    print(f"assigned address_space={address_space}, register_start_address={register_start_address}"
          f" - double check these don't collide with any out-of-band modbus/HMI address planning")
    print(f"enable with: -D{cmake_option}=ON (or edit {ROOT_CMAKE})")
    print("run generator/regs_generator.py --user-tasks-only (or just reconfigure/build) "
          f"to inject regs_table_{name}_reg into {c_path}")


def read_lines(path):
    with open(path, 'rb') as f:
        raw = f.read()
    eol = '\r\n' if b'\r\n' in raw else '\n'
    text = raw.decode('utf-8').replace('\r\n', '\n')
    return text.split('\n'), eol


def write_lines(path, lines, eol):
    text = '\n'.join(lines)
    if eol == '\r\n':
        text = text.replace('\n', '\r\n')
    with open(path, 'wb') as f:
        f.write(text.encode('utf-8'))


def pick_next_address_space_and_register():
    """Scan every existing user task header's #generator_use_description marker
    and pick the next free address_space / register_start_address, so a new task
    doesn't collide with an already-assigned modbus range."""
    marker_re = re.compile(re.escape(GENERATOR_MARKER) + r'\s+(\{.*\})')
    max_address_space = 0
    max_register_start = 0
    for header_path in glob.glob(os.path.join(USER_TASKS_DIR, '*.h')):
        with open(header_path, 'r', encoding='utf-8', errors='ignore') as f:
            for line in f:
                match = marker_re.search(line)
                if not match:
                    continue
                try:
                    description = json.loads(match.group(1))
                except ValueError:
                    continue
                if 'address_space' in description:
                    max_address_space = max(max_address_space, int(description['address_space']))
                if 'register_start_address' in description:
                    max_register_start = max(max_register_start, int(description['register_start_address']))
    return max_address_space + 1, ((max_register_start // 100) + 1) * 100


def write_header(path, name, define_name, struct_type, space_name, address_space, register_start_address):
    lines = [
        "/**",
        f" * @file {name}.h",
        " * @author Shoma Gane <shomagan@gmail.com>",
        " *",
        " * @defgroup main",
        " * @ingroup main",
        " * @version 0.1",
        " * @brief  TODO!!! write brief in",
        " */",
        f"#ifndef {define_name}_H",
        f"#define {define_name}_H 1",
        "",
        "/*add includes below */",
        "#include \"type_def.h\"",
        "#include \"main_config.h\"",
        "#include \"regs.h\"",
        "#include \"common.h\"",
        "",
        "/*add includes before */",
        "#ifdef __cplusplus",
        "   extern \"C\" {",
        "#endif",
        "/*add functions and variable declarations below */",
        "/**",
        f" * @brief {name} registers",
        " * name variables uses for generate name in description file and then in get value by name",
        " * and therefore use max size len name is 16 charackter \\n",
        " * coment style :   \"\" - description, \\n",
        " *                  &ro  - read only, \\n",
        " *                  &def -> have const varibale with struct like def_name, \\n",
        " *                  &save- will have saved in bkram, \\n",
        " * @ingroup regs",
        " */",
        f'/** {GENERATOR_MARKER} {{"space_name" :"{space_name}",  "address_space" :{address_space}, '
        f'"modbus_type" :"server", "modbus_function" :"holding_registers", '
        f'"register_start_address" :{register_start_address}}}*/',
        "typedef union{",
        "    struct MCU_PACK{",
        "        // start regs struct",
        "        /*add register fields above table_version, marked with &ro/&save/&def/&min/&max as needed */",
        "        u32 table_version;         //!< \"table version, resets regs to defaults on mismatch\" &ro &save",
        "    }vars;",
        "    u32 bytes[4]; //for full bksram copy",
        f'}}{struct_type};// {GENERATOR_MARKER} {{"message":"end_struct"}}',
        f"extern {struct_type} * const {name}_reg;",
        "",
        f"extern task_handle_t {name}_handle_id;",
        f"#if {define_name}",
        f"extern void {name}_task(void *arg);",
        f"#endif //{define_name}",
        "/*add functions and variable declarations before */",
        "#ifdef __cplusplus",
        "}",
        "#endif",
        f"#endif /*{define_name}_H*/",
        "",
    ]
    write_lines(path, lines, '\r\n')


def write_source(path, name, define_name, period_ms):
    lines = [
        "/**",
        f" * @file {name}.c",
        " * @author Shoma Gane <shomagan@gmail.com>",
        " *",
        " * @defgroup main",
        " * @ingroup main",
        " * @version 0.1",
        " * @brief  TODO!!! write brief in",
        " */",
        f"#ifndef {define_name}_C",
        f"#define {define_name}_C 1",
        "",
        f"#include \"{name}.h\"",
        "#include \"regs_description.h\"",
        "#include \"link_functions.h\"",
        "",
        f"task_handle_t {name}_handle_id = NULL;",
        f"static const char *TAG = \"{name}\";",
        f"#define {define_name}_TASK_PERIOD ({period_ms}u)",
        "",
        f'/*{GENERATOR_MARKER} {{"user_task_regs":"start_struct"}}*/',
        f'/*{GENERATOR_MARKER} {{"user_task_regs":"end_struct"}}*/',
        "",
        f"#if {define_name}",
        f"static int {name}_init(void);",
        f"static int {name}_deinit(void);",
        "",
        f"static int {name}_init(void){{",
        "    int result = 0;",
        f"    int table_ind = link_functions.regs_description_list_add_new(regs_table_{name}_reg);",
        "    link_functions.preinit_table_vars((u16)table_ind);",
        "    return result;",
        "}",
        f"static int {name}_deinit(void){{",
        "    int result = 0;",
        "    return result;",
        "}",
        f"void {name}_task(void *arg){{",
        "    (void)arg;",
        "    uint32_t signal_value;",
        f"    {name}_init();",
        "    u64 task_counter = 0u;",
        "    while(1){",
        f"        if(link_functions.os_thread_signal_wait(STOP_CHILD_PROCCES, &signal_value, {define_name}_TASK_PERIOD)==pdTRUE){{",
        "            /*by signal*/",
        "            if (signal_value & STOP_CHILD_PROCCES){",
        f"                {name}_deinit();",
        "                link_functions.os_thread_terminate(link_functions.os_thread_get_id());",
        "            }",
        "        }",
        "        task_counter++;",
        "    }",
        "}",
        f"#endif //{define_name}",
        f"#endif /*{define_name}_C*/",
        "",
    ]
    write_lines(path, lines, '\r\n')


def add_root_cmake_option(cmake_option, define_name):
    try:
        lines, eol = read_lines(ROOT_CMAKE)
    except FileNotFoundError:
        sys.stdout.write('did not find ' + ROOT_CMAKE + '\n')
        return

    if any(cmake_option in line for line in lines):
        sys.stdout.write(f'{cmake_option} already present in {ROOT_CMAKE}\n')
        return

    insert_at = -1
    for index, line in enumerate(lines):
        if re.match(r'^\s*option\(USER_TASK_', line):
            insert_at = index + 1
    if insert_at < 0:
        sys.stdout.write(f'no USER_TASK_* option block found in {ROOT_CMAKE}, {cmake_option} was not added\n')
        return

    label = define_name.replace('_', ' ').title()
    name_field = f'USER_TASK_{define_name}'.ljust(20)
    lines.insert(insert_at, f'option({name_field} "Enable {label} task" OFF)')
    write_lines(ROOT_CMAKE, lines, eol)


def add_user_tasks_cmake_entry(name, define_name):
    try:
        lines, eol = read_lines(USER_TASKS_CMAKE)
    except FileNotFoundError:
        sys.stdout.write('did not find ' + USER_TASKS_CMAKE + '\n')
        return

    if any(f'USER_TASK_{define_name})' in line for line in lines):
        sys.stdout.write(f'USER_TASK_{define_name} already wired in {USER_TASKS_CMAKE}\n')
        return

    register_at = -1
    for index, line in enumerate(lines):
        if line.strip().startswith('idf_component_register('):
            register_at = index
            break
    if register_at < 0:
        sys.stdout.write(f'idf_component_register(...) not found in {USER_TASKS_CMAKE}\n')
        return

    insert_at = register_at
    for index in range(register_at - 1, -1, -1):
        if lines[index].strip() == 'endif()':
            insert_at = index + 1
            break

    block = [
        f'if(USER_TASK_{define_name})',
        f'    list(APPEND USER_TASK_SRCS "{name}.c")',
        f'    list(APPEND USER_TASK_DEFS "{define_name}=1")',
        'endif()',
    ]
    lines[insert_at:insert_at] = block
    write_lines(USER_TASKS_CMAKE, lines, eol)


def add_user_tasks_init_entry(name, define_name, stack_size):
    try:
        lines, eol = read_lines(USER_TASKS_INIT)
    except FileNotFoundError:
        sys.stdout.write('did not find ' + USER_TASKS_INIT + '\n')
        return

    if any(f'"{name}.h"' in line for line in lines):
        sys.stdout.write(f'{name} already wired in {USER_TASKS_INIT}\n')
        return

    tag_at = -1
    for index, line in enumerate(lines):
        if line.strip().startswith('static const char *TAG = "user_tasks_init";'):
            tag_at = index
            break
    if tag_at < 0:
        sys.stdout.write(f'include insertion point not found in {USER_TASKS_INIT}\n')
        return

    include_at = tag_at
    for index in range(tag_at - 1, -1, -1):
        if lines[index].strip() == '#endif':
            include_at = index + 1
            break

    include_block = [f'#if {define_name}', f'#include "{name}.h"', '#endif']
    lines[include_at:include_at] = include_block

    function_start = -1
    for index, line in enumerate(lines):
        if line.strip().startswith('void user_tasks_start('):
            function_start = index
            break
    if function_start < 0:
        sys.stdout.write(f'user_tasks_start(...) not found in {USER_TASKS_INIT}\n')
        write_lines(USER_TASKS_INIT, lines, eol)
        return

    function_end = -1
    for index in range(function_start, len(lines)):
        if lines[index] == '}':
            function_end = index
            break
    if function_end < 0:
        sys.stdout.write(f'closing brace of user_tasks_start(...) not found in {USER_TASKS_INIT}\n')
        write_lines(USER_TASKS_INIT, lines, eol)
        return

    task_block = [
        '',
        f'#if {define_name}',
        f'    res = link_functions.os_thread_create({name}_task, "{name}_task", {stack_size}, NULL, (tskIDLE_PRIORITY + 2), &{name}_handle_id);',
        '    if (res != pdTRUE) {',
        f'        link_functions.os_log_info(TAG, "create {name}_task failed");',
        '    }',
        '#endif',
    ]
    lines[function_end:function_end] = task_block
    write_lines(USER_TASKS_INIT, lines, eol)


def add_main_cmake_regs_paths(name):
    try:
        lines, eol = read_lines(MAIN_CMAKE)
    except FileNotFoundError:
        sys.stdout.write('did not find ' + MAIN_CMAKE + '\n')
        return

    if any(f'user_tasks/{name}.h' in line for line in lines):
        sys.stdout.write(f'{name} already wired in {MAIN_CMAKE}\n')
        return

    def insert_before_set_closing_paren(lines, set_name, new_line):
        start = -1
        for index, line in enumerate(lines):
            if line.strip().startswith(f'set({set_name}'):
                start = index
                break
        if start < 0:
            sys.stdout.write(f'set({set_name} ...) not found in {MAIN_CMAKE}\n')
            return lines
        for index in range(start + 1, len(lines)):
            if lines[index].strip() == ')':
                lines.insert(index, new_line)
                return lines
        sys.stdout.write(f'closing ) for set({set_name} ...) not found in {MAIN_CMAKE}\n')
        return lines

    lines = insert_before_set_closing_paren(
        lines, 'USER_TASK_HDR_PATHS', f'    ${{project_dir}}/user_tasks/{name}.h')
    lines = insert_before_set_closing_paren(
        lines, 'USER_TASK_C_PATHS', f'    ${{project_dir}}/user_tasks/{name}.c')
    write_lines(MAIN_CMAKE, lines, eol)


if __name__ == "__main__":
    'add a new user task in project'
    main()
