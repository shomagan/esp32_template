WIFI_SLIP 
---------
1.Start or Restart
2.Request by slip(uart2) protocol form plc:type(access point or client),WiFi SSID,WiFi Password
3.Save settings
4. Loop stage every 5 seconds
    Request settings for check changes, if some changes are happend then make full system restart
    (full restart every 30 min if we had't have packets )
add_new_task
------------
python add_template.py -p main -n morse.c -t --cmake
python add_template.py -p main -n morse.h -t --starter    
