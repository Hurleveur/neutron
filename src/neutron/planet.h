#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace std;


const float PI = acos(-1.0f);


using glm::vec2;
using glm::vec3;
using glm::vec4;

void makeTangents(uint32_t nIndices, uint32_t* indices,
    const vec3* positions, const vec3* normals,
    const vec2* texCoords, vec4* tangents) {
    uint32_t inconsistentUvs = 0;
    for (uint32_t l = 0; l < nIndices; ++l) tangents[indices[l]] = vec4(0);
    for (uint32_t l = 0; l < nIndices; ++l) {
        uint32_t i = indices[l];
        uint32_t j = indices[(l + 1) % 3 + l / 3 * 3];
        uint32_t k = indices[(l + 2) % 3 + l / 3 * 3];
        vec3 n = normals[i];
        vec3 v1 = positions[j] - positions[i], v2 = positions[k] - positions[i];
        vec2 t1 = texCoords[j] - texCoords[i], t2 = texCoords[k] - texCoords[i];

        // Is the texture flipped?
        float uv2xArea = t1.x * t2.y - t1.y * t2.x;
        if (std::abs(uv2xArea) < 0x1p-20)
            continue;  // Smaller than 1/2 pixel at 1024x1024
        float flip = uv2xArea > 0 ? 1 : -1;
        // 'flip' or '-flip'; depends on the handedness of the space.
        if (tangents[i].w != 0 && tangents[i].w != -flip) ++inconsistentUvs;
        tangents[i].w = -flip;

        // Project triangle onto tangent plane
        v1 -= n * dot(v1, n);
        v2 -= n * dot(v2, n);
        // Tangent is object space direction of texture coordinates
        vec3 s = normalize((t2.y * v1 - t1.y * v2) * flip);

        // Use angle between projected v1 and v2 as weight
        float angle = std::acos(dot(v1, v2) / (length(v1) * length(v2)));
        tangents[i] += vec4(s * angle, 0);
    }
    for (uint32_t l = 0; l < nIndices; ++l) {
        vec4& t = tangents[indices[l]];
        t = vec4(normalize(vec3(t.x, t.y, t.z)), t.w);
    }
    // std::cerr << inconsistentUvs << " inconsistent UVs\n";
}


// Function to generate sphere geometry comes from https://www.songho.ca/opengl/gl_sphere.html
void generateSphere(float radius, int sectorCount, int stackCount,
                    vector<float>& vertices, vector<float>& normals, vector<float>& texCoords, vector<unsigned int>& indices, vector<float>& tangents) {
    // clear memory of prev arrays
    vector<float>().swap(vertices);
    vector<float>().swap(normals);
    vector<float>().swap(texCoords);
    vector<unsigned int>().swap(indices);

    float x, y, z, xy;                              // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
    float s, t;                                     // vertex texCoord

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    for(int i = 0; i <= stackCount; ++i) {
        stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
        xy = radius * cosf(stackAngle);             // r * cos(u)
        z = radius * sinf(stackAngle);              // r * sin(u)

        // add (sectorCount+1) vertices per stack
        // first and last vertices have same position and normal, but different tex coords
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;           // starting from 0 to 2pi

            // vertex position (x, y, z)
            x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
            y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // normalized vertex normal (nx, ny, nz)
            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            texCoords.push_back(s);
            texCoords.push_back(t);
        }
    }

    unsigned int k1, k2;
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);     // beginning of current stack
        k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if(i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    tangents.resize(indices.size());
    makeTangents(indices.size(), indices.data(), (glm::vec3*)(vertices.data()), (glm::vec3*)(normals.data()), (glm::vec2*)(texCoords.data()), (glm::vec4*)tangents.data());
}




GLuint generateMipmappedTexture(const std::string_view& imagePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.data(), &width, &height, &nrChannels, 4);
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

GLuint loadNormalMap(const std::string_view& imagePath) {
    int width, height, nrChannels;
    unsigned char* data = stbi_load(imagePath.data(), &width, &height, &nrChannels, 4);

    if (!data) {
        cerr << "Failed to load normal map: " << imagePath << endl;
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
