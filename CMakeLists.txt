cmake_minimum_required(VERSION 3.11)
project(CTIOL)

set(CMAKE_INSTALL_PREFIX $ENV{HOME}/bin)
set(CMAKE_CXX_STANDARD 17)#Supported values are 98, 11 and 14, 17.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
message("Current value of flags is ${CMAKE_CXX_FLAGS} and debug is ${CMAKE_CXX_FLAGS_DEBUG} and release is ${CMAKE_CXX_FLAGS_RELEASE}")
#Default CMAKE_CXX_FLAGS are empty, default for CMAKE_CXX_FLAGS_DEBUG is -g and CMAKE_CXX_FLAGS_RELEASE are not empty
set(CMAKE_CXX_FLAGS "-Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_BUILD_TYPE Debug)
set(BUILD_SHARED_LIBS True)

#ITK
find_package(ITK HINTS $ENV{HOME}/BIG/local/lib/cmake/ITK-5.4)
IF(ITK_FOUND)
	add_compile_definitions(WITHITK)
	include( ${ITK_USE_FILE} )
ENDIF()

#OPENCV
#find_package(OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})
#message(${OpenCV_INCLUDE_DIRS})

#OpenCL
find_package(OpenCL)
IF(OpenCL_FOUND)
	message("OpenCL include ${OpenCL_INCLUDE_DIRS}")
	add_definitions("-DCL_HPP_TARGET_OPENCL_VERSION=300")
#CL_HPP_TARGET_OPENCL_VERSION != 100 && \
#CL_HPP_TARGET_OPENCL_VERSION != 110 && \
#CL_HPP_TARGET_OPENCL_VERSION != 120 && \
#CL_HPP_TARGET_OPENCL_VERSION != 200 && \
#CL_HPP_TARGET_OPENCL_VERSION != 210 && \
#CL_HPP_TARGET_OPENCL_VERSION != 220 && \
#CL_HPP_TARGET_OPENCL_VERSION != 300
	include_directories(${OpenCL_INCLUDE_DIRS})
ENDIF()

#CLI11 comand line parser library
include_directories(${CMAKE_SOURCE_DIR}/submodules/CLI11/include)

#Header directories for the project
set(GLOBAL_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
include_directories(${GLOBAL_INCLUDE_DIR})

#Includes relative to submodules directories
include_directories(${CMAKE_SOURCE_DIR}/submodules)

#Plog logger
include_directories(${CMAKE_SOURCE_DIR}/submodules/plog/include)

#C++ String Toolkit Library
#(2) If the Boost libraries(random, regex, lexical_cast etc) are not
#available or it not wished they be used then the following
#preprocessor directive needs defining, either in code before strtk.hpp
#is included or as a compiler switch:
#(*) strtk_no_tr1_or_boost
#include_directories(${CMAKE_SOURCE_DIR} / include / strtk)

#CLI11 comand line parser library
#include_directories(${CMAKE_SOURCE_DIR} / submodules / CLI11 / include)

#Thread pool management lib ctpl from https:  // github.com/vit-vit/ctpl
#include_directories(${CMAKE_SOURCE_DIR} / submodules / ctpl)
#find_package(Threads) #include pthreads

#Intel MKL
find_package(MKL)
include_directories(${MKL_INCLUDE_DIRS})

#Projection matrix comparator by Robert Frysch
#To enable Frobenius norm just define ENABLE_FROBENIUS_NORM
#add_definitions("-DENABLE_FROBENIUS_NORM")
#include_directories(${CMAKE_SOURCE_DIR} / submodules / projection - matrix - \
#		     comparator)
#SET(PMC_SRC ${CMAKE_SOURCE_DIR} / submodules / projection - matrix -         \
#     comparator / pmatcachedfilereader.cpp ${CMAKE_SOURCE_DIR} / submodules / \
#	 projection -                                                         \
#     matrix - comparator / pmatcomparator.cpp)
#add_library(pmc ${PMC_SRC})

#file(GLOB GLOBAL_SOURCES "src/*.cpp")
#message("Version opencv " ${OpenCV_VERSION_MAJOR})
#if (${OpenCV_VERSION_MAJOR} GREATER 2)

add_definitions("-Dstrtk_no_tr1_or_boost")
FILE( GLOB CTIOLSRC ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/PROG/*.cpp )
add_library(CTIOL ${CTIOLSRC})
set_target_properties(
    CTIOL 
    PROPERTIES 
        OUTPUT_NAME "ctiol.so"
        SUFFIX ""
)
target_link_libraries(CTIOL stdc++fs)#include <experimental/filesystem>

FILE( GLOB CTMAL_SRC ${CMAKE_SOURCE_DIR}/submodules/CTMAL/src/*.cpp )
include_directories(${CMAKE_SOURCE_DIR}/submodules/CTMAL/include)
add_library(ctmal ${CTMAL_SRC})
target_link_libraries(ctmal CTIOL)
target_link_libraries(ctmal ${MKL_CORE_LIBRARY})
set_target_properties(
    ctmal
    PROPERTIES 
        OUTPUT_NAME "ctmal.so"
        SUFFIX ""
)

IF(ITK_FOUND)
FILE( GLOB LIBDENITKSRC ${CMAKE_SOURCE_DIR}/src/DENITK/*.cpp )
add_library(CTIOL_DENITK ${LIBDENITKSRC})
set_target_properties(
    CTIOL_DENITK
    PROPERTIES 
        OUTPUT_NAME "ctiol_denitk.so"
        SUFFIX ""
)
target_link_libraries(CTIOL_DENITK ${ITK_LIBRARIES})
ENDIF()

IF(OpenCL_FOUND)
FILE( GLOB LIBCTIOLOPENCLSRC ${CMAKE_SOURCE_DIR}/src/OPENCL/*.cpp )
add_library(CTIOL_OPENCL ${LIBCTIOLOPENCLSRC})
set_target_properties(
	CTIOL_OPENCL
    	PROPERTIES 
        OUTPUT_NAME "ctiol_opencl.so"
        SUFFIX ""
)
target_link_libraries(CTIOL_OPENCL OpenCL::OpenCL)
target_link_libraries(CTIOL_OPENCL CTIOL)
ENDIF()

#target_link_libraries(ImageRegistration utils)
#target_link_libraries(ImageRegistration io)
#target_link_libraries(ImageRegistration pmc)
#target_link_libraries(ImageRegistration ${CMAKE_THREAD_LIBS_INIT})
#target_link_libraries(ImageRegistration ${ITK_LIBRARIES} )
#target_link_libraries(ImageRegistration ${OpenCV_LIBS} )
#install (TARGETS ImageRegistration RUNTIME DESTINATION .)

#else()
#	message("Opencv is outdated on this system, can not compile ImageRegistration program.")
#endif()


#Catch lib
include_directories(${CMAKE_SOURCE_DIR}/tests)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CMAKE_SOURCE_DIR}/submodules/Catch2/single_include/catch2)
FILE( GLOB TST_SRC ${CMAKE_SOURCE_DIR}/tests/*.cpp )
add_executable(test_ctiol ${TST_SRC})
target_link_libraries(test_ctiol Catch)
target_link_libraries(test_ctiol CTIOL)
IF(ITK_FOUND)
target_link_libraries(test_ctiol CTIOL_DENITK)
ENDIF()
target_link_libraries(test_ctiol ctmal)
target_link_libraries(test_ctiol stdc++fs)
add_custom_target(formatWebkit
./formatWebkit
WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#Documentation target
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/doc/doxygen.conf.in)
    set(DOXYGEN_OUT ${CMAKE_SOURCE_DIR}/doc/doxygen.conf)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT})

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doxygen_doc
        ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating documentation with Doxygen"
        VERBATIM )

    add_custom_target( doxygen
        make
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc/latex
        COMMENT "Generating PDF manual"
        VERBATIM )

        add_dependencies(doxygen doxygen_doc)

else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)
