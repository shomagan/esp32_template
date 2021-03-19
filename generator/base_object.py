import re
import os
import sys

STRATIFY_OS = 0x01
FREERTOS_OS = 0x02

# Define your own exception
class Error(Exception):
    pass

class StrClass(object):
    def __init__(self, clear_line, restline):
        self.clear_line = clear_line
        self.restline = restline


class Base(object):
    """
    Attributes:
        path_global - were will be compile projeckt, should have template path
        path_project -
    """
    error_message = "error handling file"
    error_num = 0
    deleted_message = "$deleted after handing SNEMA generator"

    def __init__(self, os_type):
        # now used for plc_main.c plc_main.h main.cpp kernel.c
        self.os_type = os_type


    def print_error(self, info):
        self.error()
        msg = '\# error {} : {}\n'.format(self.error_num, info)
        raise Error(msg)

    def error(self):
        self.error_num += 1


    @staticmethod
    def add_matiec_include(line):
        line = re.sub('^\s*\#include\s+[\<\"]((iec_(std|types)\w{0,9})|(accessor))\.h[\>\"]',
                      '#include \"matiec/\g<1>.h\"', line)
        return line

    def make_comment(self, line):
        line = "//" + line[:-1] + self.deleted_message + '\n'
        return line

    def substitute(self, file_name, str_before, str_to):
        message = ''
        str_before_t = '([\W^])' + str_before + '([\W$])'
        str_to_t = '\g<1>' + str_to + '\g<2>'
        try:
            temp_buff = ''
            file_opened = open(file_name, 'r', encoding="utf-8")
            for line in file_opened:
                line_temp = line
                line = re.sub(str_before_t, str_to_t, line)
                temp_buff += line
                if line_temp != line:
                    message += 'replace ' + str_before + ' in file ' + file_name + '\n'
            file_opened.close()

            file_write = open(file_name, 'w', encoding="utf-8")
            file_write.write(temp_buff)
            file_write.close()
        except FileNotFoundError:
            self.print_error('don\'t find ' + file_name + '\n')
        if len(message):
            self.print_progress(message)

    def find_multi_stroke_comment(self, str_object):
        if self.comment:
            start = str_object.restline.find('*/')
            if start == -1:
                return str_object
            else:
                self.comment = 0
                if start < len(str_object.restline)-3:
                    str_object.restline = str_object.restline[start+2:]
                    self.find_multi_stroke_comment(str_object)
                else:
                    return str_object
        else:
            start = str_object.restline.find('/*')
            if start == -1:
                str_object.clear_line = str_object.clear_line + str_object.restline
                return str_object
            else:
                if start > 0:
                    str_object.clear_line = str_object.clear_line + str_object.restline[:start]
                    str_object.restline = str_object.restline[start+2:]
                self.comment = 1
                start = str_object.restline.find('*/')
                if start == -1:
                    return str_object
                else:
                    self.comment = 0
                    if start < len(str_object.restline)-3:
                        str_object.restline = str_object.restline[start+2:]
                        self.find_multi_stroke_comment(str_object)
                    else:
                        return str_object

    def find_sentence(self, file_name, sentence):
        global comment
        comment = 0
        num_lines = sum(1 for line in open(file_name, encoding="utf-8"))
        num_lines = num_lines-1
        try:
            file_opened = open(file_name, 'r', encoding="utf-8")
            for line in file_opened:
                if '//' in line:
                    start = line.find('//')
                    line_temp = line[:start]
                else:
                    line_temp = line
                clear_line = ''
                str_object = StrClass(clear_line, line_temp)
                self.find_multi_stroke_comment(str_object)
                if len(str_object.clear_line):
                    if sentence in str_object.clear_line:
                        return 1
                if num_lines:
                    num_lines -= 1
            file_opened.close()
        except FileNotFoundError:
            self.print_error('don\'t find ' + file_name + '\n')
            return 0
        return 0
    def iec_type_size(self,iec_type):
        return self.iec_type[iec_type]
    def get_description_type(self,iec_type):
        return self.iec_type_to_description[iec_type]


