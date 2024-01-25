# Neutron
A solar system written in C++20 using OpenGL.

## Architecture
While still a work-in-progress, the game and engine architecture is a combination of modern C++ techniques straight from the ISO C++ guidelines
and recommendations. First of all, the codebase is organized in a header-only fashion, which makes it easy to migrate to C++20 modules once
that feature is accepted by all compilers (as of January 2024, it is still not the case). Moreover, the codebase relies heavily on
_RAII_ techniques to make object lifetimes explicit and easier to understand.
The codebase uses modern features like `constexpr`, `std::initializer_list`, `std::string_view` and `std::span` wherever possible to optimize things.

## Credits
+ Neutron Game Development - @Hurleveur
+ NGE Engine Development - @felinis (Moczulski Alan)
+ Some utility scripts from [LearnOpenGL](https://learnopengl.com).

Textures in `/bin/textures` are either royalty free or made by us.

## Building the project for Windows or Linux
Run the following commands to build in Release mode:
```shell
mkdir build
cmake -S . -B build
make -C build
```
The built executable will be placed at:
`bin/neutron`
