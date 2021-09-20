import argparse
import os
import fnmatch


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
    file_template.write(" *         \n")
    file_template.write(" * @defgroup "+args.path+"\n")
    file_template.write(" * @ingroup "+args.path+"\n")
    file_template.write(" * @version 0.1 \n")
    file_template.write(" * @brief  TODO!!! write brief in \n")
    file_template.write(" */\n")

    file_template.write("#ifndef "+ args.name[:-(len_t+1)].upper() +"_"+ type_source+"\n")
    file_template.write("#define "+ args.name[:-(len_t+1)].upper() +"_"+ type_source + " 1"+"\n")
    file_template.write(" \n")
    if type_source == "H":
        file_template.write("/*add includes below */\n")
        file_template.write("#include \"type_def.h\"\n")
        file_template.write("#include \"main_debug.h\"\n")
        file_template.write("#include \"main_config.h\"\n")
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

