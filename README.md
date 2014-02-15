# Introduction

This is a library to access CAN Databases (aka CANdb, aka DBC files) from Vector Informatik.

# Build on Linux (e.g. Debian Testing)

Building under Linux works as usual:

<pre>
mkdir build
cd build
cmake ..
make
make install DESTDIR=..
make package
</pre>

# Build on Windows (e.g. Windows 7 64-Bit)

Building under Windows contains the following steps:

* Use cmake-gui
* Set "Where is the source code:" to the root directory.
* Set "Where to build the binaries:" to folder build below the root directory. Eventually create it.
* Set option OPTION_USE_CPP11_REGEX
* Configure and Generate
* Open the Visual Studio Solution (.sln) file in the build folder.
* Compile it.

# Test

Static tests are

* Cppcheck
* CCCC

Dynamic tests are

* Unit tests

The test execution can be triggered using

<pre>
make test
</pre>

# Package

The package generation can be triggered using

<pre>
make package
</pre>

# Repository Structure

The following files are part of the source code distribution:

* src/_project_/
* src/_project_/tests/

The following files are working directories for building and testing:

* build/_project_/

The following files are products of installation and building:

* bin/
* lib/
* share/doc/_project_/
* share/man/
* include/_project_/
