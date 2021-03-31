import argparse
import os
import fnmatch

license_copy = "\
/*\n\
 * Copyright (c) 2018 Snema Service\n\
 * All rights reserved.\n\
 *\n\
 * Redistribution and use in source and binary forms, with or without modification,\n\
 * are permitted provided that the following conditions are met:\n\
 *\n\
 * 1. Redistributions of source code must retain the above copyright notice,\n\
 *    this list of conditions and the following disclaimer.\n\
 * 2. Redistributions in binary form must reproduce the above copyright notice,\n\
 *    this list of conditions and the following disclaimer in the documentation\n\
 *    and/or other materials provided with the distribution.\n\
 * 3. The name of the author may not be used to endorse or promote products\n\
 *    derived from this software without specific prior written permission.\n\
 *\n\
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED\n\
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n\
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT\n\
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,\n\
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT\n\
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS\n\
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN\n\
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING\n\
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY\n\
 * OF SUCH DAMAGE.\n\
 *\n\
 * This file is part of the sofi PLC.\n\
 *\n\
 * Author: Shoma Gane <shomagan@gmail.com>\n\
 *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>\n\
 */\n"


__description__ = 'add template'


def main():
    parser = argparse.ArgumentParser(description=__description__)
    parser.add_argument('-p', '--path', type=str, default="src",
                        help=('path to where will add file'
                              '(default: %(default)s)'))

    parser.add_argument('-n', '--name', type=str, default="template",
                        help=('name file with extens=cion .c .h .cpp .hpp'
                              '(default: %(default)s)'))

    args = parser.parse_args()
    if not os.path.exists(args.path):
        print("path not exist")
        exit()
    if os.path.exists(args.path + '/' +args.name):
        print("file exist")
        exit()
    for root, dirnames, filenames in os.walk('./'):
        for filename in fnmatch.filter(filenames, '*.*'):
            if filename == args.name:
                print("file exist", os.path.join(root, filename))
                exit()
    
    if args.name == "template":
        print("name not set")
        exit()
    if args.name[-2] == '.':
        if args.name[-1] == 'c':
            type_source = "C"
            len_t = 1
        elif args.name[-1] == 'h':
            type_source = "H"
            len_t = 1
        else:
            print("type not support")
            exit()

    else:
        if args.name[-3:] == 'cpp':
            type_source = "CPP"
            len_t = 3
        elif args.name[-3:] == 'hpp':
            type_source = "HPP"
            len_t = 3
        else:
            print("type not support")
            exit()

    file_template = open(args.path+'/'+args.name,'w', encoding="utf-8")
    file_template.write("/**\n")
    file_template.write(" * @file "+args.name+"\n")
    file_template.write(" * @author Shoma Gane <shomagan@gmail.com>\n")
    file_template.write(" *         Ayrat Girfanov <girfanov.ayrat@yandex.ru>\n")
    file_template.write(" * @defgroup "+args.path+"\n")
    file_template.write(" * @ingroup "+args.path+"\n")
    file_template.write(" * @version 0.1 \n")
    file_template.write(" * @brief  TODO!!! write brief in \n")
    file_template.write(" */\n")
    file_template.write(license_copy)
    file_template.write("#ifndef "+ args.name[:-(len_t+1)].upper() +"_"+ type_source+"\n")
    file_template.write("#define "+ args.name[:-(len_t+1)].upper() +"_"+ type_source + " 1"+"\n")
    file_template.write(" \n")
    if type_source == "H":
        file_template.write("/*add includes below */\n")
        file_template.write("#include \"type_def.h\"\n")
        file_template.write("#include \"sofi_debug.h\"\n")
        file_template.write("#include \"sofi_config.h\"\n")
        file_template.write("\n")
        file_template.write("/*add includes before */\n")
        file_template.write("#ifdef __cplusplus \n")           
        file_template.write("   extern \"C\" {\n")
        file_template.write("#endif\n")
        file_template.write("/*add functions and variable declarations below */\n")
        file_template.write("\n")
        file_template.write("/*add functions and variable declarations before */\n")
        file_template.write("#ifdef __cplusplus\n")
        file_template.write("}\n")
        file_template.write("#endif\n")

        
    file_template.write("#endif "+"//"+ args.name[:-(len_t+1)].upper() +"_"+ type_source+"\n")
    file_template.close()

if __name__ == "__main__":
    'add template in project'
    main()

