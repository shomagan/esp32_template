import sys
import os
import re
import fileinput
import msvcrt as m
import regs_handl
import argparse
import base_object
from git import Repo
import hashlib
# Define your own exception


class Error(Exception):
    pass


__description__ = 'generate regs description file'


def generate(type_module):
    regs_file_path = '../../inc/regs.h'
    regs_file = open(regs_file_path, 'r', errors='ignore')
    user_describe_html = open('describe.html', 'w', encoding='UTF-8')
    user_describe_rst = open('address_space_' + type_module + '.rst', 'w', encoding='UTF-8')
    regs_description = open('regs_description.pat', 'w', encoding='UTF-8')
    regs_description_header_path = '../../inc/regs_description.h'
    temp_html = open('temp.shtml', 'r')
    struct = 0
    markdown_desc_table = "|index|name|type|size|byte|mdb|flags|description|\n" \
                          "|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|\n"
    rst_desc_table = ".. csv-table::\n"\
                     "   :header: \"index\",\"name\",\"type\",\"size\",\"byte\",\"mdb\",\"flags\", " \
                     "\"description\"\n"\
                     "   :widths: 3, 5, 3, 5, 5, 12, 10, 30\n\n"
    for line_full in regs_file:
        if '//' in line_full:
            start = line_full.find('//')
            line = line_full[:start]
        else:
            line = line_full
        p = re.compile('\Wmdb_addr\W')
        if re.search(p, line):
            struct = 1
            regs_hand = regs_handl.RegsHand()
        if struct == 1 and '}' in line:
            struct = 0
        if struct:
            regs_hand.check(line_full, type_module)
            if type(regs_hand.size_array) == str:
                regs_file_temp = open(regs_file_path, 'r', errors='ignore')
                regs_hand.size_array = find_define(regs_hand.size_array, regs_file_temp)
                regs_file_temp.close()
            if regs_hand.size_array:
                if regs_hand.is_struct:
                    struct_num = regs_hand.size_array
                    print('struct_num', struct_num)
                    description = regs_hand.description
                    for i in range(struct_num):
                        regs_hand.description = description + str(i)
                        regs_hand.additional_name = regs_hand.internal_name + str(i)
                        regs_hand.size_array = regs_hand.size_value
                        markdown_desc_table += regs_hand.add_variable_desc(type_module)
                        rst_desc_table += regs_hand.add_variable_desc_rst(type_module)
                        regs_hand.add_variable_to_regs_description(regs_description, i, type_module)
                        regs_hand.guid += regs_hand.size_value * regs_handl.REGS_SIZE[regs_hand.type]
                        if regs_hand.flag & regs_hand.SAVE_TYPE:
                            regs_hand.saved_address += regs_hand.size_value * regs_handl.REGS_SIZE[regs_hand.type]
                else:
                    markdown_desc_table += regs_hand.add_variable_desc(type_module)
                    rst_desc_table += regs_hand.add_variable_desc_rst(type_module)
                    regs_hand.add_variable_to_regs_description(regs_description, 0, type_module)
                    regs_hand.guid += regs_hand.size_array * regs_handl.REGS_SIZE[regs_hand.type]
                    if regs_hand.flag & regs_hand.SAVE_TYPE:
                        regs_hand.saved_address += regs_hand.size_array * regs_handl.REGS_SIZE[regs_hand.type]
    regs_file.close()
    user_describe_rst.write(rst_desc_table)
    print("first config variable " + str(regs_hand.guid))
    change_regs_num(regs_description_header_path, regs_hand.regs_num)
    current_path = os.path.dirname(os.path.abspath(__file__))
    file_test = open('../../inc/regs.h', 'a', encoding='UTF-8')
    file_test.close()
    change_version(current_path + "/../../")
    print("find " + str(regs_hand.regs_num) + " own network variable")
    print("last bkram address " + str(regs_hand.saved_address))
    regs_description.close()
    # rewrite exist file with new network variable settings
    write_exist_file()
    user_describe_html.close()
    fs_save_file = open('../../lwip/src/apps/http/fs.c', 'a')
    fs_save_file.write(' ')
    fs_save_file.close()
    temp_html.close()
    print('close index.html in project')
    print('start makefsdata.exe')
    os.system("make_fs.bat")


def main():
    parser = argparse.ArgumentParser(description=__description__)
    parser.add_argument('-t', '--type', type=str, default="isimfw400",
                        help=('type of device'
                              '(default: %(default)s)'))
    args = parser.parse_args()
    print("module type - {}".format(args.type))
    regs_h_had_changed = 1
    with open("../../inc/regs.h", "rb") as f:
        sha_file_path = "../../regs_sha256.txt"
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
                regs_h_had_changed = 0
    version_file_path = "../../version_from_git.txt"
    current_path = os.path.dirname(os.path.abspath(__file__))
    git_path = (current_path + "/../../")
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
        fs_save_file = open('../../lwip/src/apps/http/fs.c', 'a')
        fs_save_file.write(' ')
        fs_save_file.close()
        print("regs.h file had not changed")
    os.system("make_fs.bat")


def find_define(name, file):
    for line in file:
        define_temp = re.compile('^\s*\#define\s+'+name+'\s+(?P<value>[\d]+).*', re.ASCII)
        match = define_temp.match(line)
        if match:
            search = define_temp.search(line)
            if search.group('value'):
                if search.group('value').isdecimal():
                    print('find define ', name)
                    return int(search.group('value'))
                else:
                    print("warning find not decimal define")
    print("did't find  define", name)
    return 0


def change_version(git_base_path):
    repo = Repo(git_base_path)
    assert not repo.bare
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_datetime)
    print("current version {}".format(tags[-1]))
    version = str(tags[-1])
    version = version.replace("v", "")
    version = version.replace(".", ",")
    str_before = "\#define\s+OS_VERSION\s+\{[\d\,]+\}"
    str_to = "#define OS_VERSION {"+version+"}"
    print(str_before, str_to)
    substitute_reg_exp("../../inc//regs.h", str_before, str_to)


def get_git_version(git_base_path):
    repo = Repo(git_base_path)
    assert not repo.bare
    tags = sorted(repo.tags, key=lambda t: t.commit.committed_datetime)
    print("current version {}".format(tags[-1]))
    version = str(tags[-1])
    version = version.replace("v", "")
    version = version.replace(".", ",")
    return version


def change_regs_num(regs_description_header_path, regs_number):
    str_before = '\#define\s+NUM_OF_SELF_VARS\s+[\d]+'
    str_to = '#define NUM_OF_SELF_VARS ' + str(regs_number)
    substitute_reg_exp(regs_description_header_path, str_before, str_to)


def substitute_parameter(file_name, str_before, str_to):
    message = ''
    str_before_t = '([\w\W^])'+str_before+'([\w\W$\s])'
    str_to_t = '\g<1>'+str_to+'\g<2>'
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
    file_path = '../../src/regs_description.c'
    for line in fileinput.input(file_path, inplace=1):
        if re.search("\s*regs_description_t\s+const\s+regs_description\[\s*NUM_OF_SELF_VARS\s*\]\s*\=\s*\{",line):
            replace = 1
            print(line, end='')
        elif replace:
            cancel = re.compile('^\s*\}\;', re.ASCII)
            test = cancel.match(line)
            if test:
                replace = 0
                for own_line in template_file:
                    print(own_line, end='')
                    number += 1
                print('};')
        else:
            print(line, end='')
    fileinput.close()
    if number:
        print('replace ' + str(number) + ' string in description file' + file_path)
    else:
        print("don't find regs_description_t struct in file" + file_path)


if __name__ == "__main__":
    main()
