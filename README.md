A Solar system in OpenGL

## Libraries list & credits
The project includes the following libs (/lib directory): assimp.lib, freetype.lib, glew32s.lib, glfw3.lib, irrKlang.lib, SOIL.lib
And the following (pre-)compiled for windows dlls (/dlls directory): assimp-vc140-mt.dll, ikpMP3.dll, irrKlang.dll
And some utility scripts from [https://learnopengl.com](https://learnopengl.com), plus a function from [https://www.songho.ca/opengl/gl_sphere.html](https://www.songho.ca/opengl/gl_sphere.html) and another from [www.stackoverflow.com/questions/5255806](www.stackoverflow.com/questions/5255806) to generate the spheres with their tangents.

Textures in /resources are either royalty free or made by us.

## Building the project for Windows
1. Run the CMake script
```
cd /path/to/project
mkdir build
cd build
cmake ..
cmake --build .
```
2. Run the executable /bin/Debug/neutron.exe
