# --- FindMKL.cmake ---
# Author: Vojtěch Kulvait
# License: GPL-3

# Find MKL library in KCT framework. 
# Priority paths are set to the testing environment of the author, but the default search paths include typical installation locations and environment variables.
# Provides: MKL::MKL imported interface target

set(_mkl_roots "")

#Set those as priority paths to search for MKL
list(APPEND _mkl_roots
    /data/hereon/wp/group/laupy/share/intel_oneapi/mkl/2025.3
    /opt/intel/2025/oneapi/mkl/latest
    /opt/intel/compilers_and_libraries_2019.4.243/linux/mkl
    /opt/intel/compilers_and_libraries_2019.0.117/linux/mkl
    $ENV{HOME}/opt/MKL/2022/mkl/2022.0.1
    /opt/intel/oneapi/mkl/2023.1.0
)

if(DEFINED ENV{MKLROOT})
  list(APPEND _mkl_roots "$ENV{MKLROOT}")
endif()

if(DEFINED ENV{ONEAPI_ROOT})
  list(APPEND _mkl_roots "$ENV{ONEAPI_ROOT}/mkl/latest")
endif()

list(APPEND _mkl_roots
    /opt/intel/oneapi/mkl/latest
    /opt/intel/mkl
)

# include
find_path(MKL_INCLUDE_DIRS
    NAMES mkl.h mkl_lapacke.h
    PATH_SUFFIXES include
    HINTS ${_mkl_roots}
)

# library
find_library(MKL_CORE_LIBRARY
    NAMES mkl_rt
    PATH_SUFFIXES lib lib/intel64 lib/intel64_lin
    HINTS ${_mkl_roots}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MKL
    REQUIRED_VARS MKL_INCLUDE_DIRS MKL_CORE_LIBRARY
)

if(MKL_FOUND)
    add_library(MKL::MKL INTERFACE IMPORTED)

    set_target_properties(MKL::MKL PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${MKL_INCLUDE_DIRS}"
        INTERFACE_LINK_LIBRARIES      "${MKL_CORE_LIBRARY}"
    )
endif()

mark_as_advanced(MKL_INCLUDE_DIRS MKL_CORE_LIBRARY)
