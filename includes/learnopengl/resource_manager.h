/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include <learnopengl/texture.h>
#include <learnopengl/shader.h>

#include <iostream>
#include <sstream>
#include <fstream>

#include "stb_image.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no
// public constructor is defined.
class ResourceManager
{
public:
    // resource storage
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;

    // retrieves a stored sader
    static Shader GetShader(std::string name){
    return Shaders[name];
}
    // loads (and generates) a texture from file
    static Texture2D LoadTexture(const char *file, bool alpha, std::string name){
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}
    // retrieves a stored texture
    static Texture2D GetTexture(std::string name){
    return Textures[name];
}
    // properly de-allocates all loaded resources
    static void      Clear(){
    // (properly) delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // (properly) delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}
private:
    // private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
    ResourceManager() { }
    public:
    // loads a single texture from file
    static Texture2D loadTextureFromFile(const char *file, bool alpha){
    // create texture object
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // load image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // now generate texture
    texture.Generate(width, height, data);
    // and finally free image data
    stbi_image_free(data);
    return texture;
}
};



// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>  ResourceManager::Shaders;


#endif