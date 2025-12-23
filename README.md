# Pierrot le fou engine

A very little and low-level 3d engine made for learning purpose.

## Installation
### Windows

C'mon just install linux or you have to set-up wls.

### Linux

To compile the project on linux you need to install few package :
 * vulkan
 * glfw3

#### Ubuntu

'''
sudo apt install libvulkan-dev vulkan-validationlayers libglfw3 libglfw3-dev
'''

#### Arch

'''
sudo pacman -S vulkan-devel glfw
'''

## Compilation

When you are in the project directory just do :

'''
mkdir build \
cd build \
cmake -D DEBUG=on .. \
make
'''

Then the binary while be in bin/pierrot.
