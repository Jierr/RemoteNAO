cmake_minimum_required(VERSION 2.8)
project(remoteServer)

find_package(qibuild)

set(_srcs
    main.cpp
    netNao.h
    netNao.cpp)

# Create a executable named remoteServer
# with the source file: main.cpp
qi_create_bin(remoteServer ${_srcs})

qi_use_lib(remoteServer ALCOMMON)

# Add a simple test:
#enable_testing()
#qi_create_test(test_remoteServer $(_srcs))
