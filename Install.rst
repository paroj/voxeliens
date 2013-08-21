Installing Voxeliens
====================

Getting the dependencies
------------------------

Beyond a basic C++ comiler,  Voxeliens has a few standard dependencies. It requires CMake to create Makefiles and the `Qt <https://qt-project.org/>`_, `SDL_mixer <http://www.libsdl.org/projects/SDL_mixer/>`_ and `Ogre <http://www.ogre3d.org/>`_ libraries to compile.

Commands to install the dependencies for some major distributions are given below.

openSUSE:
	``sudo zypper install make gcc-c++ cmake libqt4-devel libSDL-devel libSDL_mixer-devel libOgreMain-devel``

Fedora
	``yum install make gcc-c++ cmake qt-devel SDL-devel SDL_mixer-devel ogre-devel``

Arch
	``pacman -S gcc make cmake ogre qt4 sdl sdl_mixer boost``

Debian/Ubuntu:
	``apt-get install cmake libqt4-dev libsdl1.2-dev libsdl-mixer1.2-dev libogre-1.8-dev``

Compiling
---------

Once you have all the dependencies,  compiling the application should be a simple process

.. code:: bash

	mkdir build
	cd build
	cmake .. # add -DCMAKE_INSTALL_PREFIX=/path/to/install to specify install prefix
	make
	make install

and Voxeliens should be compiled and installed to the correct location.
