nodejs
======
This repository contains the edition of the open-source Node.js program used by webOS. Node.js is a server-side JavaScript environment that uses an asynchronous event-driven model. This allows Node.js to get excellent performance based on the architectures of many Internet applications.

See the _Resources for Newcomers_ section in README.upstream.md for links to information on the upstream project. Do not use the build instructions found there.

How to Build on Linux
=====================

## Dependencies

Below are the tools and libraries (and their minimum versions) required to build nodejs:

* cmake 2.6
* gcc 4.3
* make (any version)
* python 2.6 or 2.7 (3.0 cannot be used)

## Building

Once you have downloaded the source, execute the following to build it:

    $ mkdir BUILD
    $ cd BUILD
    $ cmake ..
    $ make
    $ sudo make install

The executable will be installed under

    /usr/local/bin

The header files will be installed under

    /usr/local/include/node

and the pkg-config file under

    /usr/local/lib

You can install it elsewhere by supplying a value for _CMAKE\_INSTALL\_PREFIX_ when invoking _cmake_. For example:

    $ cmake -D CMAKE_INSTALL_PREFIX:STRING=$HOME/projects/openwebos ..
    $ make
    $ make install
    
will install the files in subdirectories of $HOME/projects/openwebos instead of subdirectories of /usr/local. 

Specifying _CMAKE\_INSTALL\_PREFIX_ also causes the pkg-config files under it to be used to find headers and libraries. To have _pkg-config_ look in a different tree, set the environment variable _PKG\_CONFIG\_PATH_ to the path to its _lib/pkgconfig_ subdirectory.

To compile with -Wall, configure for debug:

    $ cmake -D CMAKE_BUILD_TYPE:STRING=Debug ..
    $ make

## Uninstalling

From the directory where you originally ran _make install_, invoke:

    $ sudo xargs rm < install_manifest.txt

## Generating documentation

Once you have run _cmake_, execute the following to generate the documentation:

    $ make doc

To view the generated HTML documentation for the API, point your browser to:

    doc/api/index.html

## Testing

To run the test programs, run:

    $ make test

## Building modules for nodejs

If your system has pkg-config then you can just add this to your makefile:

    CFLAGS += $(shell pkg-config --cflags nodejs)

# Copyright and License Information

See the file LICENSE.

