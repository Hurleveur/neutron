#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;


const float PI = 3.14159265359f;

// Function to generate sphere geometry
void generateSphere(float radius, int sectorCount, int stackCount, vector<float>& vertices, vector<float>& normals, vector<float>& texCoords) {
    float sectorStep = 2.0f * PI / static_cast<float>(sectorCount);
    float stackStep = PI / static_cast<float>(stackCount);

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * stackStep;
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;

            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);

            // Vertex position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Vertex normal (for simplicity, we assume it's the same as the position)
            normals.push_back(x);
            normals.push_back(y);
            normals.push_back(z);

            // Texture coordinates
            float u = static_cast<float>(j) / sectorCount;
            float v = static_cast<float>(i) / stackCount;
            texCoords.push_back(u);
            texCoords.push_back(v);
        }
    }
}

// Function to generate mipmapped texture
GLuint generateMipmappedTexture(const char* imagePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    if (!data) {
        cerr << "Failed to load texture: " << imagePath << endl;
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