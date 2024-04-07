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
                       help=('name file with extention .c .h .cpp .hpp'
                             '(default: %(default)s)'))

   parser.add_argument('-t', dest='task', action='store_const',
                        const=True, default=False,
                        help='if active add simply standart task')
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
      file_template.write("#include \"main_config.h\"\n")
      file_template.write("#include \"regs.h\"\n")
      file_template.write("#include \"common.h\"\n")
      file_template.write("\n")
      file_template.write("/*add includes before */\n")
      file_template.write("#ifdef __cplusplus \n")           
      file_template.write("   extern \"C\" {\n")
      file_template.write("#endif\n")
      file_template.write("/*add functions and variable declarations below */\n")
      if args.task:
         add_task_dependencies_for_header(file_template,args.name[:-(len_t+1)])
      file_template.write("\n")
      file_template.write("/*add functions and variable declarations before */\n")
      file_template.write("#ifdef __cplusplus\n")
      file_template.write("}\n")
      file_template.write("#endif\n")
   elif type_source == "C":
      if args.task:
         add_task_dependencies_for_c_source(file_template,args.name[:-(len_t+1)]);
       
   file_template.write("#endif "+"/*"+ args.name[:-(len_t+1)].upper() +"_"+ type_source+"*/\n")
   file_template.close()


def add_task_dependencies_for_header(file_template, component_name):
   file_template.write(f"extern task_handle_t {component_name}_handle_id;\n")
   file_template.write(f"extern void {component_name}_task(void *arg);\n")


def add_task_dependencies_for_c_source(file_template, component_name):
   file_template.write(f"""
task_handle_t {component_name}_handle_id = NULL;
static const char *TAG = \"{component_name}\";
#define {component_name.upper()}_TASK_PERIOD (100u)
static int {component_name}_init(void);
static int {component_name}_deinit();
""")
   file_template.write(f"""
static int {component_name}_init(){{
   int result = 0;
   regs_global.vars.current_state[0] |= CS0_TASK_ACTIVE_{component_name.upper()};
   return result;
}}""")

   file_template.write(f"""  
static int {component_name}_deinit(){{
   int result = 0;
   regs_global.vars.current_state[0] &= ~((u32)CS0_TASK_ACTIVE_{component_name.upper()});
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
""")



if __name__ == "__main__":
   'add template in project'
   main()

