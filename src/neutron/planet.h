#pragma once

#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// std headers
#include <iostream>
#include <cmath>

using glm::vec2;
using glm::vec3;
using glm::vec4;

constexpr float PI = std::acos(-1.0f);

GLuint generateMipmappedTexture(const std::string_view& imagePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.data(), &width, &height, &nrChannels, 4);
    if (!data) {
        std::cerr << "Failed to load texture: " << imagePath << '\n';
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters and generate mipmaps
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}

GLuint loadNormalMap(const std::string_view& imagePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.data(), &width, &height, &nrChannels, 4);

    if (!data) {
        std::cerr << "Failed to load normal map: " << imagePath << '\n';
        return 0;
    }

    GLuint normalMapID;
    glGenTextures(1, &normalMapID);
    glBindTexture(GL_TEXTURE_2D, normalMapID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Generate normal map texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return normalMapID;
}
