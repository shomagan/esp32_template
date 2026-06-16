ESP-IDF
-------
  version 6.0

Description
-----------
Main OS(main folder) system checks user task(user_tasks folder) and run it if exist.

User task(only for esp32c3 at the moment)
has access to main function via '''link_functions_t link_functions'''
steps
 add user registers to table via add_user_registers function


add_new_task
------------

  python add_template.py -p main -n morse.c -t --cmake
  python add_template.py -p main -n morse.h -t --starter
