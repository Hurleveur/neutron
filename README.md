# Neutron
A solar system written in C++17 using OpenGL.

## Libraries list & Credits
The project includes the following libs (`/lib` directory): assimp.lib, freetype.lib, glew32s.lib, glfw3.lib, irrKlang.lib, SOIL.lib
And the following (pre-)compiled for windows dlls (`/dlls` directory): assimp-vc140-mt.dll, ikpMP3.dll, irrKlang.dll
And some utility scripts from [LearnOpenGL](https://learnopengl.com), a function from [Songho OpenGL Sphere](https://www.songho.ca/opengl/gl_sphere.html) and another from [StackOverflow](www.stackoverflow.com/questions/5255806) allowing to generate spheres with tangents.

Textures in `/bin/textures` are either royalty free or made by us.

## Building the project for Windows/Linux
Run the CMake script:
```shell
mkdir build
cmake -S . -B build
make -C build
```
The built executable will be placed at:
`bin/neutron`
