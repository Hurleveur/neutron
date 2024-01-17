#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cmath>
#include <span>
#include <glm/geometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace std;

constexpr float PI = std::acos(-1.0f);

using glm::vec2;
using glm::vec3;
using glm::vec4;

void makeTangents(uint32_t nIndices, uint32_t* indices,
    const vec3* positions, const vec3* normals,
    const vec2* texCoords, vec3* tangents) {
    for (uint32_t l = 0; l < nIndices; l += 3) {
        // Get indices of the triangle
        uint32_t i = indices[l];
        uint32_t j = indices[l + 1];
        uint32_t k = indices[l + 2];
        vec3 vertex_normal = normals[i];
        vec3 v1 = positions[i] - positions[j], v2 = positions[j] - positions[k];
        vec2 t1 = texCoords[i] - texCoords[j], t2 = texCoords[j] - texCoords[k];

        // Is the texture flipped?
        float uv2xArea = t1.x * t2.y - t1.y * t2.x;
        if (std::abs(uv2xArea) < 0x1p-20)
            continue;  // Smaller than 1/2 pixel at 1024x1024
        float flip = uv2xArea > 0 ? 1 : -1;

        // Project triangle onto tangent plane
        v1 -= vertex_normal * dot(v1, vertex_normal);
        v2 -= vertex_normal * dot(v2, vertex_normal);
        // Tangent is object space direction of texture coordinates
        vec3 s = normalize((t2.x * v1 - t1.x * v2) * flip);

        // Use angle between projected v1 and v2 as weight
        float angle = std::acos(dot(v1, v2) / (length(v1) * length(v2)));
        tangents[i] += s * angle;
    }
    for (uint32_t l = 0; l < nIndices; ++l) {
        vec3& t = tangents[indices[l]];
        t = normalize(t);
    }
}

struct Triangle
{
    uint32_t i0, i1, i2;

    constexpr Triangle(uint32_t i0, uint32_t i1, uint32_t i2):
        i0(i0), i1(i1), i2(i2)
    {}
};
void ComputeTangentsForMesh(const std::span<const vec3>& vertices, const std::span<const vec3>& normals,
    const std::span<const vec2>& texcoords, const std::span<const Triangle>& triangles, std::vector<vec3>& tangents) {
    vec3 *tan1 = new vec3[vertices.size() * 2];
    memset(tan1, 0, vertices.size_bytes() * 2);
    vec3 *tan2 = tan1 + vertices.size();

    for (long a = 0; a < triangles.size(); a++)
    {
        long i1 = triangles[a].i0;
        long i2 = triangles[a].i1;
        long i3 = triangles[a].i2;

        const auto& v1 = vertices[i1];
        const auto& v2 = vertices[i2];
        const auto& v3 = vertices[i3];

        const auto& w1 = texcoords[i1];
        const auto& w2 = texcoords[i2];
        const auto& w3 = texcoords[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (uint32_t a = 0; a < vertices.size(); a++)
    {
        const auto& n = normals[a];
        const auto& t = tan1[a];

        // Gram-Schmidt orthogonalize
        tangents[a] = normalize(t - n * glm::dot(n, t));

        // Calculate handedness
//        float handedness = (glm::dot(glm::cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
//        tangents[a] *= handedness;
    }

    delete[] tan1;
}

// Function to generate sphere geometry comes from https://www.songho.ca/opengl/gl_sphere.html
void generateSphere(float radius, int sectorCount, int stackCount,
    vector<glm::vec3>& vertices, vector<glm::vec3>& normals,
    vector<glm::vec2>& texCoords, vector<Triangle>& triangles, vector<glm::vec3>& tangents) {
    // clear memory of prev arrays
    // TODO: this is really bad, we need to move this to a PlanetFactory class!!!
    vertices.clear();
    normals.clear();
    texCoords.clear();
    triangles.clear();
    tangents.clear();

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
            vertices.push_back(glm::vec3(x, y, z));

            // normalized vertex normal (nx, ny, nz)
            vec3 normal(x, y, z);
            normal *= lengthInv;
            normals.push_back(normal);

            // vertex tex coord (s, t) range between [0, 1]
            s = (float)j / sectorCount;
            t = (float)i / stackCount;
            texCoords.push_back(glm::vec2(s, t));
        }
    }

    for(int i = 0; i < stackCount; ++i)
    {
        auto k1 = i * (sectorCount + 1);     // beginning of current stack
        auto k2 = k1 + sectorCount + 1;      // beginning of next stack

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if(i != 0)
                triangles.push_back(Triangle(k1, k2, k1 + 1));

            // k1+1 => k2 => k2+1
            if(i != (stackCount-1))
                triangles.push_back(Triangle(k1 + 1, k2, k2 + 1));
        }
    }
    tangents.resize(vertices.size());
    ComputeTangentsForMesh(vertices, normals, texCoords, triangles, tangents);
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
