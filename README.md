# CT Input Output Library

Input output routines for asynchronous thread safe reading/writing CT data.
Currently, mainly the DEN format read/write is implemented.
This repository actually contains one basic library and separate library that depends on ITK and header files.

## Cloning repository:

Clone over ssh, for those with ssh keys ready in gitlab
```
git clone ssh://git@gitlab.stimulate.ovgu.de:2200/vojtech.kulvait/CTIOL.git
```
Basic clone can be performed over https
```
git clone https://gitlab.stimulate.ovgu.de/vojtech.kulvait/CTIOL.git
```
However to populate submodules directories it is then needed to issue
```
git submodule init
git submodule update
```

## How to build:
For build process, make and cmake utilities are required. Install them using
```
apt-get install make cmake
```


Please create separate directory and build there
```
mkdir build
cd build
cmake ..
make
```

## Submodules

Submodules lives in the submodules directory. To clone project including submodules one have to use the following commands

```
git submodule init
git submodule update
```
or use the following command when cloning repository

```
git clone --recurse-submodules
```

We use relative [url](https://www.gniibe.org/memo/software/git/using-submodule.html) to improve experience for http protocol users.

### [Plog](https://github.com/SergiusTheBest/plog) logger

Logger Plog is used for logging. It is licensed under the Mozilla Public License Version 2.0.

### [Catch2](https://github.com/catchorg/Catch2)

Testing framework. Licensed under Boost Software License 1.0.

### [CTMAL](https://gitlab.stimulate.ovgu.de/vojtech.kulvait/CTMAL)

Mathematic/Algebraic algorithms for supporting CT data manipulation.


## Documentation

Documentation is generated using doxygen and lives in doc directory.
First the config file for doxygen was prepared runing doxygen -g.
Doc files and this file can be written using [Markdown syntax](https://daringfireball.net/projects/markdown/syntax), JAVADOC_AUTOBRIEF is set to yes to treat first line of the doc comment as a brief description, comments are of the format 
```
/**Brief description.
*
*Long description
*thay might span multiple lines.
*/
```
.


## Dependencies

The library ctiol\_denitk uses [ITK library](https://itk.org/Doxygen410/html/index.html). It is required to have this library in the system installed and headers reachable. On Debian it is sufficient to run
```
apt-get install libinsighttoolkit4-dev
```

## Using IDEs

### Eclipse
For eclipse to work, create out of source project that is not child or ancesor of the parent directory.
Run the following to generate eclipse project
```
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug path_to_project
```
After each change in CMakeLists.txt remove eclipse project and regenerate it.
See more at https://gitlab.kitware.com/cmake/community/wikis/doc/editors/Eclipse-CDT4-Generator

