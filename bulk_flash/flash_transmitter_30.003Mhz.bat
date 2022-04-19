Echo prepare
:loop
timeout 5
"C:\Program Files (x86)\STMicroelectronics\st_toolset\stvp\STVP_CmdLine.exe"  -Device=STM8S003F3 -no_loop -no_verif -no_erase -FileOption="OPTION_BYTES.hex" -verbose
"C:\Program Files (x86)\STMicroelectronics\st_toolset\stvp\STVP_CmdLine.exe"  -Device=STM8S003F3 -no_loop -FileProg="hc-12-rr-beacon_30.003Mhz.hex" -verbose
goto loop