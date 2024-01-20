//==============================================================================
//	NGE - Neutron Game Engine
//	(C) 2024 Moczulski Alan
//==============================================================================

#pragma once

// engine headers
#include "nge_math.hh"

// GL headers
#include <glad/glad.h>

// std headers
#include <span>

namespace nge::graphics
{
	struct VertexFormat
	{
		vec3 position;
		vec3 normal;
		vec3 tangent;
		vec2 texcoord;
	};

	class Model
	{
		GLuint VAO;
		GLuint VBO;
		GLuint EBO;
		u32 vertex_index_count;

	public:
		Model(const std::span<const VertexFormat>& vertices, const std::span<const u16>& vertex_indices):
			VAO(0), VBO(0), EBO(0), vertex_index_count(vertex_indices.size())
		{
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			glBindVertexArray(VAO);

			// fill vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, (GLuint)vertices.size_bytes(), vertices.data(), GL_STATIC_DRAW);

			// fill index buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLuint)vertex_indices.size_bytes(), vertex_indices.data(),
						 GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)offsetof(VertexFormat, position));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)offsetof(VertexFormat, normal));
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)offsetof(VertexFormat, tangent));
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void *)offsetof(VertexFormat, texcoord));

			glBindVertexArray(0);
		}

		~Model()
		{
			glDeleteBuffers(1, &EBO);
			glDeleteBuffers(1, &VBO);
			glDeleteVertexArrays(1, &VAO);
		}

		void Set()
		{
			glBindVertexArray(VAO);
		}

		void Draw()
		{
			glDrawElements(GL_TRIANGLES, (GLint)vertex_index_count, GL_UNSIGNED_SHORT, nullptr);
		}

		void Unset()
		{
			glBindVertexArray(0);
		}
	};
}
