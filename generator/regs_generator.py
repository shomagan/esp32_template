﻿import sys
import os
import re
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


def generate(type_module):
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
    # rewrite exist file with new network variable settings
    write_exist_file()
    fill_up_regs_h_main_structure(regs_h_file_path, regs_hand.modbus_structures_description)
    fill_up_regs_c_main_pointers(regs_c_file_path, regs_hand.modbus_structures_description)

    fs_save_file = open('../main/http/fs.c', 'a')
    fs_save_file.write(' ')
    fs_save_file.close()


def main():
    parser = argparse.ArgumentParser(description=__description__)
    parser.add_argument('-t', '--type', type=str, default="isimfw400",
                        help=('type of device'
                              '(default: %(default)s)'))
    args = parser.parse_args()
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
        generate(args.type)
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
    str_before = r'\#define\s+NUM_OF_SELF_VARS\s+[\d]+'
    str_to = '#define NUM_OF_SELF_VARS ' + str(regs_self_num)
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
        if re.search(r"\s*regs_description_t\s+const\s+regs_description\[\s*NUM_OF_SELF_VARS\s*\]\s*\=\s*\{",line):
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

    template_file = open('regs_description_client.pat', 'r', encoding='UTF-8')
    replace = 0
    number = 0
    file_path = '../main/regs_description.c'
    for line in fileinput.input(file_path, inplace=1):
        if re.search(r"\s*regs_description_t\s+const\s+regs_description_user\[\s*NUM_OF_CLIENT_VARS\s*\]\s*\=\s*\{",line):
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
        print("warning!!! did't find regs_description_user struct in file" + file_path)

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
