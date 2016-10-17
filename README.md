# Basic OpenGL with GLFW: Improved Particle System

Instead of using a spritebatcher, this example use geometry shaders to generate quads on the gpu.
With more work of the work happening in parallel on the gpu, we can render even more particles.
This example also builds from the materials example.

# Setup

You will need to have CMake installed on your computer, and properly added to your path.
In order to setup, run the following in a shell, then open the project in your preferred editor.
Windows setup has been configured for use with Visual Studio.

Windows:
```
cd path/to/folder
setup.cmd
```
Linux:
```
cd path/to/folder
./setup
```
