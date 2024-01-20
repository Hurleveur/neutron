# TODO
1. Top Priority ⚠⚠⚠
- ...
2. Medium Priority ⚠⚠
- ...
3. Low Priority ⚠
- Create mipmaps from textures and render them using linear or anisotropic filtering (to prevent Moire effect).

# ONGOING
- Refactor codebase for better cohesion and reduced coupling.
- Apply good code practices (like RAII for explicit resources lifetime management).

# DONE
- Get rid of unused code and libraries like irrKlang or Assimp. Do some cleanup.
- Fix issues with normal mapping (too extreme and visible on opposite site of lit polygons).

# Discussions and Ideas
- [nge] The codebase uses old-style C++ that is not recommended anymore, see [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).
Does @Hurleveur want to modernize the code using C++11 and beyond? Is it worth it?
- [nge] Does @Hurleveur want to use a specific engine architecture like entity-component-system or multi-threaded message-passing?
- [neutron] What are @Hurleveur's future plans about the game? Will this project only ever be a solar system simulator, or something more ambitious is planned?
- [nge] We could add a console that can be toggled with '~' and that can interpret commands.
