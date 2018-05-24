cmake_minimum_required(VERSION 3.7)
project(WirelessCharging)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        inc/Charger.cpp
        inc/Charger.h
        src/cr_cpp_config.cpp
        src/cr_startup_lpc175x_6x.cpp
        src/crp.c
        src/main.cpp
        src/sysinit.c)

include_directories(C:/nxp/MCUXpressoIDE_10.1.1_606/ide/Examples/LPCOpen/lpcopen_2_10_lpcxpresso_nxp_lpcxpresso_1769/lpc_board_nxp_lpcxpresso_1769/inc)
include_directories(C:/nxp/MCUXpressoIDE_10.1.1_606/ide/Examples/LPCOpen/lpcopen_2_10_lpcxpresso_nxp_lpcxpresso_1769/lpc_chip_175x_6x/inc)
include_directories(.)
include_directories(inc)
include_directories(src)

add_executable(WirelessCharging ${SOURCE_FILES})