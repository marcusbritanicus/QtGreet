#!/usr/bin/python3

"""
Setup - A simple python script to build and install QtGreet and greetd

###================ Program Info ================###
    Program Name : setup.py
    Version : 1.0
    Platform : Linux/Unix
    Requriements :
        Must :
            modules os, sys
    Python Version : Python 3.0 or higher
    Author : Marcus Britanicus
    Email : marcusbritanicus@gmail.com
    License : GPL version 3
###==============================================###
"""

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

    #
    # Copyright 2020 Britanicus <marcusbritanicus@gmail.com>
    #

    #
    # This program is free software; you can redistribute it and/or modify
    # it under the terms of the GNU General Public License as published by
    # the Free Software Foundation; either version 2 of the License, or
    # ( at your option ) any later version.
    #

    #
    # This program is distributed in the hope that it will be useful,
    # but WITHOUT ANY WARRANTY; without even the implied warranty of
    # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    # GNU General Public License for more details.
    #

    #
    # You should have received a copy of the GNU General Public License
    # along with this program; if not, write to the Free Software
    # Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
    # MA 02110-1301, USA.
    #

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= #

import os, sys
from os.path import exists, isdir, isfile, abspath

def ls():
    os.system( "pwd" )
    os.system( "exa --group-directories-first" )

# Clean the source tree
def clean() :

    if not "--no-greetd" in sys.argv :
        # Clean greetd
        print( "Cleaning greetd..." )
        os.chdir( "3rdparty/greetd/" )
        os.system( "cargo clean" )
        os.chdir( "../../" )

    # Clean QtGreet
    os.chdir( "src" )
    print( "Cleaning QtGreet..." )
    if exists( "build" ) and isfile( "build/Makefile" ) :
        os.chdir( "build" )
        os.system( "make clean" )
        os.chdir( "../../" )

    else:
        os.chdir( "../" )

# Prepare the sources for building - basically update
def prepare() :

    print( "Updating QtGreet..." )
    os.system( "git pull" )

# Build greetd and QtGreet
def build() :

    # Compiling greetd is default. Unless the user has sepcified
    # --no-greetd we will go on with it
    if not "--no-greetd" in sys.argv :
        print( "Building greetd" )
        os.chdir( "3rdparty/greetd" )
        os.system( "cargo build --release" )
        os.chdir( "../../" )

    print( "Building qtgreet" )
    os.chdir( "src" )
    if not exists( "build" ):
        os.mkdir( "build" )

    os.chdir( "build" )
    os.system( "cmake .. && make -kj$(nproc)" )
    os.chdir( "../../" )

def install() :
    if ( os.geteuid() ):
        print( "You'll need super-user previleges to install" )
        exit( 1 )

    idx = -1
    try:
        idx = sys.argv.index( "--prefix" )
    except ValueError:
        pass

    # --prefix has to be the second arg or greater
    prefix = ""
    if idx > 0:
        prefix = abspath( sys.argv[ idx + 1 ] )

    try:
        print( f"Installing to prefix: {prefix}" )
        os.mkdir( prefix )
    except FileExistsError:
        pass

    if not "--no-greetd" in sys.argv :
        print( "Installing greetd..." )

        os.mkdir( "" )

        # Install
        os.chdir( "3rdparty/greetd" )
        os.system( "cp target/release/greetd %s/usr/bin/" % prefix )
        os.system( "cp target/release/agreety %s/usr/bin/" % prefix )
        os.system( "cp greetd.service %s/etc/systemd/system/greetd.service" % prefix )
        if not exists( "%s/etc/greetd" % prefix ):
            os.mkdir( "%s/etc/greetd" % prefix )

        os.chdir( "../../" )
        os.system( "cp configs/config.toml %s/etc/greetd/config.toml" % prefix )

    print( "Installing qtgreet..." )
    os.chdir( "src" )
    os.system( "install -D -m 0755 -o root --strip build/qtgreet --target-directory %s/usr/bin" % prefix )
    os.chdir( "../" )
    os.system( "install -D -m 0644 -o root backgrounds/* --target-directory %s/usr/share/qtgreet/backgrounds" % prefix )
    os.system( "install -D -m 0644 -o root configs/config.ini --target-directory %s/etc/qtgreet/" % prefix )
    os.system( "install -D -m 0644 -o root configs/wayfire.ini --target-directory %s/etc/qtgreet/" % prefix )
    os.system( "install -D -m 0644 -o root README.md Changelog ReleaseNotes --target-directory %s/usr/share/qtgreet/" % prefix )

if __name__ == "__main__":

    def printUsage():
        print( "QtGreet Installer v1.0" )
        print( "Usage:" )
        print( "    setup.py [command] [switch]" )
        print( "" )
        print( "  Commands:" )
        print( "    help                Print this help text and exit" )
        print( "    version             Print the version text and exit" )
        print( "    prepare             Update and prepare the sources for building" )
        print( "    build               Build greetd and QtGreet" )
        print( "    clean               Clean the sources" )
        print( "    install             Install the binaries and sources (needs root previleges)" )
        print( "    uninstall           Uninstall all the installed files (needs root previleges)" )
        print( "" )
        print( "  Switches" )
        print( "    --greetd            Build/install/... greetd sources as well (default)" )
        print( "    --no-greetd         Do not do anything with greetd" )

    def handleArgs():
        if sys.argv[ 1 ] == "help":
            printUsage();
            exit( 0 )

        elif sys.argv[ 1 ] == "version":
            print( "QtGreet" )
            print( "Installer version 1.0.0" )
            print( "QtGreet version 1.0.0" )
            exit( 0 )

        elif sys.argv[ 1 ] == "prepare":
            prepare()
            exit( 0 )

        elif sys.argv[ 1 ] == "build":
            build()
            exit( 0 )

        elif sys.argv[ 1 ] == "install":
            install()
            exit( 0 )

        elif sys.argv[ 1 ] == "uninstall":
            # uninstall()
            exit( 0 )

        elif sys.argv[ 1 ] == "clean":
            clean()
            exit( 0 )

        elif sys.argv[ 1 ] == "project":
            clean()
            prepare()
            build()

            exit( 0 )

        else:
            print( "QtGreet Installer v1.0" )
            print( "Please provide a valid command. Type 'setup.py help' to see the usage." )
            exit( 1 )


    # If no arguments were provided, print usage
    if len( sys.argv ) == 1:
        printUsage()
        exit( 0 )

    # One command given
    elif ( len( sys.argv ) == 2 ):
        if sys.argv[ 1 ] == "--no-greetd":
            print( "QtGreet Installer v1.0" )
            print( "Please provide a command. Type 'setup.py help' to see the usage." )
            exit( 1 )

        else:
            handleArgs()

    else:
        # greetd = "--no-greetd" in sys.argv
        # greetd |= "--greetd" in sys.argv

        # if not greetd:
        #     print( "QtGreet Installer v1.0" )
        #     print( "Invalid usage. Type 'setup.py help' to see the usage." )
        #     exit( 1 )

        # else:
        handleArgs()
