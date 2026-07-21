import sys
import os
import re
import glob
import fileinput
import regs_handl
import argparse
import platform
import base_object
from git import Repo
import hashlib
import json
try:
    import msvcrt
    PLATFORM = "win"
except ImportError:
    PLATFORM = "unix"
    import tty
    import termios
    from select import select

# Define your own exception
class Error(Exception):
    pass


__description__ = 'generate regs description file'
REGS_H_PATH = "../main/regs.h"


def generate(type_module, skip_user_tasks=False):
    regs_h_file_path = '../main/regs.h'
    regs_c_file_path = '../main/regs.c'
    user_describe_rst = open('address_space_' + type_module + '.rst', 'w', encoding='UTF-8')
    regs_description = open('regs_description.pat', 'w', encoding='UTF-8')
    regs_description_client = open('regs_description_client.pat', 'w', encoding='UTF-8')
    regs_description_header_path = '../main/regs_description.h'
    rst_desc_table_head = ".. csv-table::\n"\
                          "   :header: \"index\",\"name\",\"type\",\"size\",\"byte\",\"mdb\",\"flags\", " \
                          "\"description\"\n"\
                          "   :widths: 3, 5, 3, 5, 5, 12, 10, 30\n\n"
    user_describe_rst.write(rst_desc_table_head)
    regs_hand = regs_handl.RegsHand(os_type=platform.system())
    regs_hand.regs_file_handling(regs_h_file_path, regs_description, regs_description_client)
    user_describe_rst.write(regs_hand.user_describe_rst)
    user_describe_rst.close()
    regs_description.close()
    regs_description_client.close()
    print("first config variable " + str(regs_hand.guid))
    change_regs_num(regs_description_header_path, regs_hand.regs_self_num, regs_hand.regs_client_num)
    current_path = os.path.dirname(os.path.abspath(__file__))
    change_version(current_path + "/../")
    print("find " + str(regs_hand.regs_self_num) + " own network variable")
    print("find " + str(regs_hand.regs_client_num) + " client network variable")
    print("last bkram address " + str(regs_hand.saved_address))
    write_exist_file()
    fill_up_regs_h_main_structure(regs_h_file_path, regs_hand.modbus_structures_description)
    fill_up_regs_c_main_pointers(regs_c_file_path, regs_hand.modbus_structures_description)
    # user_tasks/regs_description_user.c is generated separately via
    # user_tasks/CMakeLists.txt using --user-tasks-only. Only generate here
    # when running the full generator manually (skip_user_tasks=False).
    if not skip_user_tasks and os.path.exists('../user_tasks/'):
        generate_user_tasks()  # manual full-run convenience

    fs_save_file = open('../main/http/fs.c', 'a')
    fs_save_file.write(' ')
    fs_save_file.close()


def generate_user_tasks(header_basenames=None):
    """Generate regs_description_user.c for user tasks.

    Parameters
    ----------
    header_basenames : list[str] | None
        If given, only process these header filenames (basenames, e.g. ['scd41.h']).
        If None, process all *.h files in ../user_tasks/.
    """
    user_tasks_dir = '../user_tasks/'
    if header_basenames:
        user_task_headers = sorted(
            os.path.join(user_tasks_dir, h) for h in header_basenames
        )
    else:
        user_task_headers = sorted(glob.glob(user_tasks_dir + '*.h'))

    regs_hand = regs_handl.RegsHand(os_type=platform.system())
    for header_path in user_task_headers:
        if os.path.exists(header_path):
            regs_hand.regs_file_handling_user_task(header_path)
        else:
            print(f"WARNING: user task header not found: {header_path}")

    print("find {} user task variables in {} files".format(
        regs_hand.regs_self_num, len(regs_hand.modbus_structures_description)))

    inject_user_task_regs_files(regs_hand)


def inject_user_task_regs_files(regs_hand):
    """Inject each task's regs_description_t table into its own user_tasks/<task>.c,
    between the {"user_task_regs":"start_struct"}/"end_struct"} markers."""
    user_tasks_dir = '../user_tasks/'
    task_descriptions = regs_hand.modbus_structures_description

    groups = {}
    for i, desc in enumerate(task_descriptions, start=1):
        source_file = desc.get("source_file", "")
        groups.setdefault(source_file, []).append(i)

    for source_file, structure_numbers in groups.items():
        c_file_name = os.path.splitext(source_file)[0] + '.c'
        c_path = os.path.join(user_tasks_dir, c_file_name)
        block_lines = build_user_task_regs_block(regs_hand, task_descriptions, structure_numbers)
        inject_user_task_regs_block(c_path, block_lines)


def build_user_task_regs_block(regs_hand, task_descriptions, structure_numbers):
    lines = []
    for i in structure_numbers:
        desc = task_descriptions[i - 1]
        struct_type = desc["struct_type"]
        global_name = desc["regs_global_name"]
        lines.append(f'static {struct_type} {global_name}_storage = {{{{0}}}};')
        lines.append(f'{struct_type} * const {global_name} = &{global_name}_storage;')

    for i in structure_numbers:
        desc = task_descriptions[i - 1]
        global_name = desc["regs_global_name"]
        space_name = desc["space_name"][:15]  # NVS namespace names are capped at 15 chars
        entries = regs_hand.user_task_entries.get(i, [])
        saved_size = regs_hand.user_task_saved_sizes.get(i, 0) + 4  # plus crc
        num_vars = len(entries)
        saved_size = max(saved_size, 4)

        lines.append(f'#define NUM_OF_{global_name.upper()}_VARS {num_vars}')
        lines.append(f'static u8 {global_name}_saved_buf[{saved_size}];')
        lines.append(f'static const char {global_name}_space_name[] = "{space_name}";')
        lines.append(f'static regs_description_t const regs_description_{global_name}[NUM_OF_{global_name.upper()}_VARS] = {{')
        for entry in entries:
            lines.append(f'    {entry},')
        lines.append('};')
        lines.append(f'const regs_description_list_t regs_table_{global_name} = {{')
        lines.append(f'    .description = regs_description_{global_name},')
        lines.append(f'    .num_of_regs = NUM_OF_{global_name.upper()}_VARS,')
        lines.append('    .table_version = &def_table_version,')
        lines.append(f'    .space_name = {global_name}_space_name,')
        lines.append(f'    .saved_regs_buffer = {global_name}_saved_buf,')
        lines.append(f'    .saved_regs_buffer_size = sizeof({global_name}_saved_buf),')
        lines.append('};')
    return lines


def inject_user_task_regs_block(c_path, block_lines):
    if not os.path.exists(c_path):
        print(f"WARNING: user task source not found: {c_path}")
        return
    replace = 0
    found_start = 0
    found_end = 0
    for line in fileinput.input(c_path, inplace=1):
        json_str = regs_handl.RegsHand.check_generator_descriptions(line)
        is_start = False
        is_end = False
        if len(json_str):
            json_description = json.loads(json_str)
            if json_description.get("user_task_regs") == "start_struct":
                is_start = True
            elif json_description.get("user_task_regs") == "end_struct":
                is_end = True
        if is_start:
            found_start = 1
            replace = 1
            print(line, end='')
        elif is_end:
            if replace:
                for block_line in block_lines:
                    print(block_line)
            replace = 0
            found_end = 1
            print(line, end='')
        elif not replace:
            print(line, end='')
    fileinput.close()
    if found_start and found_end:
        print(f"injected user task regs table into {c_path}")
    else:
        print(f"WARNING: user_task_regs markers not found in {c_path}, regs table not injected")


def main():
    parser = argparse.ArgumentParser(description=__description__)
    parser.add_argument('-t', '--type', type=str, default="isimfw400",
                        help=('type of device'
                              '(default: %(default)s)'))
    parser.add_argument('--user-tasks-only', action='store_true',
                        help='Only generate regs_description_user.c for user tasks, skip main generation')
    parser.add_argument('--no-user-tasks', action='store_true',
                        help='Skip generating regs_description_user.c (used when CMake handles it separately)')
    parser.add_argument('--user-task-header', action='append', dest='user_task_headers',
                        metavar='HEADER', default=[],
                        help='User task header basename to include (can be repeated, e.g. scd41.h). '
                             'If omitted all *.h in user_tasks/ are scanned.')
    args = parser.parse_args()

    # ---- User-tasks-only mode: generate regs_description_user.c and exit ----
    if args.user_tasks_only:
        print("user-tasks-only mode: generating regs_description_user.c")
        headers = args.user_task_headers if args.user_task_headers else None
        generate_user_tasks(header_basenames=headers)
        return

    # ---- Full generation mode ----
    print("module type - {}".format(args.type))
    regs_h_had_changed = 1
    with open(REGS_H_PATH, "rb") as f:
        sha_file_path = "../regs_sha256.txt"
        bytes_read = f.read()         # read entire file as bytes
        readable_hash = hashlib.sha256(bytes_read).hexdigest()
        if not os.path.exists(sha_file_path):
            regs_h_had_changed = 1
            sha_file = open(sha_file_path, 'w')
            sha_file.write(str(readable_hash))
            sha_file.close()
        else:
            sha_file = open(sha_file_path, 'r')
            sha = sha_file.read()
            sha_file.close()
            if sha != str(readable_hash):
                regs_h_had_changed = 1
                sha_file = open(sha_file_path, 'w')
                sha_file.write(str(readable_hash))
                sha_file.close()
            else:
                regs_h_had_changed = 1
    version_file_path = "../version_from_git.txt"
    current_path = os.path.dirname(os.path.abspath(__file__))
    git_path = (current_path + "/../")
    version_from_git = get_git_version(git_path)

    if not os.path.exists(version_file_path):
        regs_h_had_changed = 1
        version_file = open(version_file_path, 'w')
        version_file.write(version_from_git)
        version_file.close()
        print("file version_from_git.txt not exist")
    else:
        version_file = open(version_file_path, 'r')
        version_from_file = version_file.read()
        version_file.close()
        if version_from_file != version_from_git:
            print("file version_from_git.txt was changed")
            regs_h_had_changed = 1
            version_file = open(version_file_path, 'w')
            version_file.write(version_from_git)
            version_file.close()
    if regs_h_had_changed:
        print("regs.h file was changed")
        generate(args.type, skip_user_tasks=args.no_user_tasks)
    else:
        fs_save_file = open('../main/http/fs.c', 'a')
        fs_save_file.write(' ')
    print('start makefsdata')
    os.chdir("../main/http/")
    if PLATFORM == "win":
        os.system("makefsdata.exe")
    else:
        os.system("./makefsdata_l")
    os.chdir("../../generator/")


def change_version(git_base_path):
    repo = Repo(git_base_path)
    assert not repo.bare
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_datetime)
    head_commit = repo.head.commit
    commits = list(repo.iter_commits('HEAD'))
    count_commits = len(commits)
    print("current version {}".format(tags[-1]))
    version = str(tags[-1])
    version_array = re.findall(r'\d+', version)
    version = version.replace("v", "")
    version = version.replace(".", ",")
    str_before = r"\#define\s+FW_VERSION\s+\{[\d\,]+\}"
    str_to = "#define FW_VERSION {"+version+"}"
    print(str_before, str_to)
    substitute_reg_exp(REGS_H_PATH, str_before, str_to)
    str_before = r"\#define\s+FW_VERSION_STR\s+\"[\d\.\w\-]+\""
    str_to = "#define FW_VERSION_STR \"{}.{}.{}-beta.{}\"".format(version_array[0],
                                                                  version_array[1],
                                                                  version_array[2],
                                                                  version_array[3])
    print(str_before, str_to)
    substitute_reg_exp(REGS_H_PATH, str_before, str_to)
    str_before = r"\#define\s+FW_FIRMWARE_HASH\s+\"[\d\-\w]+\""
    str_to = "#define FW_FIRMWARE_HASH \"{}-{}\"".format(count_commits, head_commit.hexsha[:30])
    print(str_before, str_to)
    substitute_reg_exp(REGS_H_PATH, str_before, str_to)


def get_git_version(git_base_path):
    repo = Repo(git_base_path)
    assert not repo.bare
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_datetime)
    print("current version {}".format(tags[-1]))
    version = str(tags[-1])
    version = version.replace("v", "")
    version = version.replace(".", ",")
    return version


def change_regs_num(regs_description_header_path, regs_self_num, regs_client_num):
    str_before = r'\#define\s+NUM_OF_MAIN_VARS\s+[\d]+'
    str_to = '#define NUM_OF_MAIN_VARS ' + str(regs_self_num)
    substitute_reg_exp(regs_description_header_path, str_before, str_to)
    str_before = r'\#define\s+NUM_OF_CLIENT_VARS\s+[\d]+'
    str_to = '#define NUM_OF_CLIENT_VARS ' + str(regs_client_num)
    substitute_reg_exp(regs_description_header_path, str_before, str_to)


def substitute_parameter(file_name, str_before, str_to):
    message = ''
    str_before_t = r'([\w\W^])'+str_before+r'([\w\W$\s])'
    str_to_t = r'\g<1>'+str_to+r'\g<2>'
    try:
        temp_buff = ''
        with  open(file_name, 'r', encoding="utf-8") as file_opened:
            for line in file_opened:
                line_temp = line
                line = re.sub(str_before_t, str_to_t, line)
                temp_buff += line
                if line_temp != line:
                    message += 'replace'+str_before+'in file'+file_name+'\n'
        file_write = open(file_name, 'w', encoding="utf-8")
        file_write.write(temp_buff)
        file_write.close()
    except FileNotFoundError:
        sys.stdout.write('don\'t find '+file_name+'\n')
    sys.stdout.write(message)


def substitute_reg_exp(file_name, str_before, str_to):
    message = ''
    str_before_t = str_before
    str_to_t = str_to
    try:
        temp_buff = ''
        with open(file_name, 'r', encoding="utf-8") as file_opened:
            for line in file_opened:
                line_temp = line
                line = re.sub(str_before_t, str_to_t, line)
                temp_buff += line
                if line_temp != line:
                    message += 'replace'+str_before+'in file'+file_name+'\n'
        file_write = open(file_name, 'w', encoding="utf-8")
        file_write.write(temp_buff)
        file_write.close()
    except FileNotFoundError:
        sys.stdout.write('did not find '+file_name+'\n')
    sys.stdout.write(message)


def write_exist_file():
    template_file = open('regs_description.pat', 'r', encoding='UTF-8')
    replace = 0
    number = 0
    file_path = '../main/regs_description.c'
    for line in fileinput.input(file_path, inplace=1):
        if re.search(r"\s*regs_description_t\s+const\s+regs_description_global\[\s*NUM_OF_MAIN_VARS\s*\]\s*\=\s*\{",line):
            replace = 1
            print(line, end='')
        elif replace:
            cancel = re.compile(r'^\s*\}\;', re.ASCII)
            if cancel.match(line):
                replace = 0
                for own_line in template_file:
                    print(own_line, end='')
                    number += 1
                print(line, end='')
        else:
            print(line, end='')
    fileinput.close()
    template_file.close()
    if number:
        print('replace ' + str(number) + ' string in description file' + file_path)
    else:
        print("warning!!! did't find regs_description struct in file" + file_path)


def fill_up_regs_h_main_structure(file_path, struct_descriptions):
    '''fill up regs_main_t in regs.h file, uses {"regs_main":"start_struct"}'''
    replace = 0
    number = 0
    for line in fileinput.input(file_path, inplace=1):
        if replace:
            cancel = re.compile(r'^\s*\}\s*regs_main_t\;', re.ASCII)
            if cancel.match(line):
                replace = 0
                print('typedef struct{\n', end='')
                for description in struct_descriptions:
                    #keep it for future print(f'#if USE_{description["regs_global_name"].upper()}\n', end='')
                    print(f'    {description["struct_type"]} {description["regs_global_name"]}; //!< "{description["struct_type"]}"\n', end='')
                    #keep it for future print('#endif\n', end='')
                    number += 1
                print(line, end='')
        else:
            print(line, end='')
            json_str = regs_handl.RegsHand.check_generator_descriptions(line)
            if len(json_str):
                json_description = json.loads(json_str)
                if "regs_main" in json_description:
                    replace = 1
    print(f'number of structures {number}')

def fill_up_regs_c_main_pointers(regs_c_file_path, struct_descriptions):
    '''fill up global pointer to be used by user tasks in regs.c file, uses {"regs_pointers":"start_struct"}'''
    replace = 0
    for line in fileinput.input(regs_c_file_path, inplace=1):
        if replace:
            cancel = re.compile(r'\/\*\s*\#generator_use_description\s+\{\"regs_pointers\"\:\"end_struct\"\}', re.ASCII)
            if cancel.match(line):
                replace = 0
                for description in struct_descriptions:
                    #TODO print(f'#if USE_{description["regs_global_name"].upper()}\n', end='')
                    print(f'{description["struct_type"]} * const {description["regs_global_name"]} = &regs_main.{description["regs_global_name"]}; //!< "{description["struct_type"]}"\n', end='')
                    #TODO it for future print('#endif\n', end='')
                print(line, end='')
        else:
            print(line, end='')
            json_str = regs_handl.RegsHand.check_generator_descriptions(line)
            if len(json_str):
                json_description = json.loads(json_str)
                if "regs_pointers" in json_description:
                    replace = 1

if __name__ == "__main__":
    main()
