import argparse
import os
import fnmatch
import sys
import re

__description__ = 'add template'


def main():
   parser = argparse.ArgumentParser(description=__description__)
   parser.add_argument('-p', '--path', type=str, default="src",
                       help=('path to where will add file'
                             '(default: %(default)s)'))

   parser.add_argument('-n', '--name', type=str, default="template",
                       help=('name file with extention .c .h .cpp .hpp'
                             '(default: %(default)s)'))

   parser.add_argument('-t', dest='task', action='store_const',
                        const=True, default=False,
                        help='if active add simply standart task')
   parser.add_argument('--cmake', dest='cmake', action='store_const',
                        const=True, default=False,
                        help='if active change cmake file')
   parser.add_argument('--starter', dest='starter', action='store_const',
                        const=True, default=False,
                        help='if active change wifi_slip with the new task to start')

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
   core_name = ""
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
   core_name = args.name[:-(len_t+1)]
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

   file_template.write("#ifndef "+ core_name.upper() +"_"+ type_source+"\n")
   file_template.write("#define "+ core_name.upper() +"_"+ type_source + " 1"+"\n")
   file_template.write(" \n")
   if type_source == "H":
      file_template.write("/*add includes below */\n")
      file_template.write("#include \"type_def.h\"\n")
      file_template.write("#include \"main_config.h\"\n")
      file_template.write("#include \"regs.h\"\n")
      file_template.write("#include \"common.h\"\n")
      file_template.write("#include \"pin_map.h\"\n")
      file_template.write("\n")
      file_template.write("/*add includes before */\n")
      file_template.write("#ifdef __cplusplus \n")           
      file_template.write("   extern \"C\" {\n")
      file_template.write("#endif\n")
      file_template.write("/*add functions and variable declarations below */\n")
      if args.task:
         add_task_dependencies_for_header(file_template,core_name)
      file_template.write("\n")
      file_template.write("/*add functions and variable declarations before */\n")
      file_template.write("#ifdef __cplusplus\n")
      file_template.write("}\n")
      file_template.write("#endif\n")
   elif type_source == "C":
      if args.task:
         add_task_dependencies_for_c_source(file_template,core_name);
   if args.cmake:
      change_cmake_file(args.path, args.name)
   if args.starter:
      change_task_handler(core_name)
   file_template.write("#endif "+"/*"+ core_name.upper() +"_"+ type_source+"*/\n")
   file_template.close()


def add_task_dependencies_for_header(file_template, component_name):
   file_template.write(f"""
extern task_handle_t {component_name}_handle_id;\n
#if {component_name.upper()}\n
extern void {component_name}_task(void *arg);\n
#endif //{component_name.upper()}\n
""")


def add_task_dependencies_for_c_source(file_template, component_name):
   file_template.write(f"""
#include \"{component_name}.h\"
#include \"esp_log.h\"
#include \"esp_check.h\"
#include \"regs_description.h\"
                       
task_handle_t {component_name}_handle_id = NULL;
static const char *TAG = \"{component_name}\";
#define {component_name.upper()}_TASK_PERIOD (100u)
#if {component_name.upper()}
static int {component_name}_init(void);
static int {component_name}_deinit();
""")
   file_template.write(f"""
static int {component_name}_init(){{
   int result = 0;
   regs_global->vars.current_state[0] |= CS0_TASK_ACTIVE_{component_name.upper()};
   return result;
}}""")

   file_template.write(f"""  
static int {component_name}_deinit(){{
   int result = 0;
   regs_global->vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_{component_name.upper()});
   return result;
}}
void {component_name}_task(void *arg){{
   (void)(*arg);
   uint32_t signal_value;
   {component_name}_init();
   u64 task_counter = 0u;
   while(1){{
      if(task_notify_wait(STOP_CHILD_PROCCES, &signal_value, {component_name.upper()}_TASK_PERIOD)==pdTRUE){{
         /*by signal*/
         if (signal_value & STOP_CHILD_PROCCES){{
            {component_name}_deinit();
            task_delete(task_get_id());
         }}
      }}
      task_counter++;
   }}
}}
#endif //{component_name.upper()}
""")
   
def change_cmake_file(cmake_path, file_name):
    try:
      index_string = -1
      with open(cmake_path+'/CMakeLists.txt', 'r', encoding="utf-8") as file_opened:
         index = 0
         for line in file_opened:
            if file_name in line:
               break
            if "INCLUDE_DIRS \".\"" in line:
               index_string = index
               break
            index += 1
      if index_string >= 0:
         cmake_config = []
         with open(cmake_path+'/CMakeLists.txt', "r") as file_opened:
            cmake_config = file_opened.readlines()
         cmake_config.insert(index_string, f"    SRCS \"{file_name}\"\n")
         with open(cmake_path+'/CMakeLists.txt', "w") as file_opened:
            cmake_config_new = "".join(cmake_config)
            file_opened.write(cmake_config_new)
      else:
         sys.stdout.write('cmake config was not changed'+cmake_path+'/CMakeLists.txt'+'\n') 
    except FileNotFoundError:
        sys.stdout.write('did not find '+cmake_path+'/CMakeLists.txt'+'\n')

def change_task_handler(file_name):
    try:
      task_function_end = -1
      include_end = -1
      with open('main/wifi_slip_main.c', 'r', encoding="utf-8") as file_opened:
         index = 0
         function_start = -1
         re_task_funct_temp = re.compile(r"^int\s+main_init_tasks\(esp_sleep_wakeup_cause_t\s*\*\s*esp_sleep_wakeup_cause\)\{", re.UNICODE)
         re_include_temp = re.compile(r"\/\*end\s+include\*\/", re.UNICODE)
         for line in file_opened:
            match = re_include_temp.match(line)
            if match:
               include_end = index
            match = re_task_funct_temp.match(line)
            if match:
               function_start = index
            if function_start >= 0:
               if "return" in line:
                  task_function_end = index
                  break
            index += 1
      if task_function_end >= 0:
         task_config = []
         with open('main/wifi_slip_main.c', "r") as file_opened:
            task_config = file_opened.readlines()
         task_config.insert(include_end, f"""
#if {file_name.upper()}
#include \"{file_name}.h\"
#endif
""")
         task_config.insert(task_function_end, f"""
#if {file_name.upper()}
    res = task_create({file_name}_task, \"{file_name}_task\", 2464, (void *)esp_sleep_wakeup_cause, (tskIDLE_PRIORITY + 2), &{file_name}_handle_id);
    if(res != pdTRUE){{
        main_printf(TAG,"{file_name}_task inited success\\n");
    }}
#endif
""")
         with open('main/wifi_slip_main.c', "w") as file_opened:
            task_config_new = "".join(task_config)
            file_opened.write(task_config_new)
      else:
         sys.stdout.write('task config was not changed'+'main/wifi_slip_main.c'+'\n') 
    except FileNotFoundError:
        sys.stdout.write('did not find '+'main/wifi_slip_main.c'+'\n')



if __name__ == "__main__":
   'add template in project'
   main()

