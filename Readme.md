# OpenGL IHM Poject - EI3 -
The binary is in the bin/Windows folder for the Microsoft Windows executable, or in the bin/Linux for the linux executable. Please keep the binary file with each additionnal folder/file provided.

The source code is in the maze/ folder.

## Running on Windows
You have to make sure you have installed the Microsoft Visual C++ 2017 redistributable.
If not, please download and install it with this link:
https://aka.ms/vs/15/release/vc_redist.x86.exe

## Running on Linux
Make sure that you have the ``` freeglut-devel ``` library installed

* On a debian-based linux :
   * ``` sudo apt-get install freeglut3-dev ```
* On a redhat-based linux :
   * ``` sudo yum install freeglut-devel ```
## Building on Windows
The provided folder is a Visual Studio 2017 project, so you can open the project with it and launch a debug or
release build with it.

## Building on Linux
The provided makefile will help you build the project on a linux distribution, if you have installed the correct
libraries. To build a debug version, please launch ``` make debug ```. To build a release version, please launch
``` make release ```. To clean the project, please launch ``` make clean ```.