cmake_minimum_required(VERSION 2.8)
project(remoteServer)

find_package(qibuild)

set(_srcs1
    main.cpp
    netNao.h
    netNao.cpp)
    
set(_srcs2
	managerModule.cpp
	manager.h
	manager.cpp)  
	  
#set(_srcs3
#	decoderModule.cpp
#	decoder.h
#	decoder.cpp)
    


# Create a executable named remoteServer
# with the source file: main.cpp
qi_create_bin(remoteServer ${_srcs1})
#qi_create_lib(netnao SHARED ${_srcs} SUBFOLDER naoqi)
qi_use_lib(remoteServer ALCOMMON BOOST)

qi_create_lib(rm_manager SHARED ${_srcs2} SUBFOLDER naoqi)
qi_use_lib(rm_manager ALCOMMON BOOST)



# Add a simple test:
#enable_testing()
#qi_create_test(test_remoteServer $(_srcs))
