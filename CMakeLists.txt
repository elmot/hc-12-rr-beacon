cmake_minimum_required(VERSION 3.20)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

set(IAR_EW_PATH "C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.3" )
# Toolchain
list(APPEND CMAKE_PROGRAM_PATH "${IAR_EW_PATH}/stm8/bin/")
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
set(CMAKE_C_COMPILER iccstm8.exe)
set(CMAKE_CXX_COMPILER iccstm8.exe)

project(hc-12-test C ASM)
add_executable(hc-12-test)
target_sources(hc-12-test PUBLIC main.c stm8s_it.c)
target_sources(hc-12-test PUBLIC stm8s_it.h stm8s_conf.h)

target_sources(hc-12-test PUBLIC STM8S_StdPeriph_Driver/src/stm8s_spi.c STM8S_StdPeriph_Driver/src/stm8s_gpio.c)


target_compile_definitions(hc-12-test PUBLIC STM8S003 USE_STDPERIPH_DRIVER)
target_include_directories(hc-12-test PUBLIC STM8S_StdPeriph_Driver/inc .)

target_compile_options(hc-12-test PUBLIC -e -Ol --no_cse --no_unroll --no_inline --no_code_motion --no_tbaa --no_cross_call)
target_compile_options(hc-12-test PUBLIC --debug --code_model small --data_model medium --dlib_config ${IAR_EW_PATH}/stm8/LIB/dlstm8smn.h --vregs 16)
target_link_options(hc-12-test PUBLIC "SHELL:--config_def _HEAP_SIZE=0x100 --config_def _CSTACK_SIZE=0x100")
target_link_options(hc-12-test PUBLIC "SHELL:--redirect _Printf=_PrintfFullNoMb --redirect _Scanf=_ScanfFullNoMb")
target_link_options(hc-12-test PUBLIC --map ${CMAKE_BINARY_DIR}/beacon.map --config ${IAR_EW_PATH}/stm8/config/lnkstm8s003f3.icf)
target_link_options(hc-12-test PUBLIC --entry __iar_program_start --debug_lib --no_out_extension)
