#ifndef VFRAME_NO_3D

#include <GL/glew.h>

#include "V3DModel.h"
#include "V3DShader.h"
#include "V3DCamera.h"
#include "V3DLight.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "depend/glm/glm.hpp" 
#include "depend/glm/gtx/transform.hpp"

GLuint V3DModel::DefaultTexture = 0;

V3DModel::V3DModel(sf::Vector3f position, sf::Vector3f rotation, sf::Vector3f scale) :
	V3DObject(position, rotation), vao(0), vertexVBO(0), indexVBO(0), Scale(scale)
{
	Material = new V3DMaterial();
}

V3DModel::V3DModel(float posX, float posY, float posZ,
	float rotX, float rotY, float rotZ,
	float scaleX, float scaleY, float scaleZ) :
	V3DObject(posX, posY, posZ, rotX, rotY, rotZ), vao(0), vertexVBO(0), indexVBO(0), Scale(scaleX, scaleY, scaleZ)
{
	Material = new V3DMaterial();
}

void V3DModel::updateTransform()
{
	//S * R * T
	glm::mat4 matrix_pos = glm::translate(glm::vec3(Position.x, Position.y, Position.z));
	glm::mat4 matrix_scale = glm::scale(glm::vec3(Scale.x, Scale.y, Scale.z));
	// Represent each stored rotation as a different matrix, because 
	// we store angles. 
	//          x  y  z 
	glm::mat4 matrix_rotX = glm::rotate(Rotation.x * (3.1415926f / 180.0f), glm::vec3(1, 0, 0));
	glm::mat4 matrix_rotY = glm::rotate(Rotation.y * (3.1415926f / 180.0f),	glm::vec3(0, 1, 0));
	glm::mat4 matrix_rotZ = glm::rotate(Rotation.z * (3.1415926f / 180.0f),	glm::vec3(0, 0, 1));
	// Create a rotation matrix. 
	// Multiply in reverse order it needs to be applied. 
	glm::mat4 matrix_rotation = matrix_rotZ * matrix_rotY * matrix_rotX;
	// Apply transforms in reverse order they need to be applied in. 
	transform = matrix_pos * matrix_rotation * matrix_scale;
}

bool V3DModel::LoadModelData(const V3DVertexArray& vertexArray, const std::vector<unsigned int>& indexArray)
{
	drawCount = indexArray.size() > 0 ? indexArray.size() : vertexArray.size();

	auto stride = sizeof(vertexArray[0]);
	auto normalOffset = sizeof(vertexArray[0].position);
	auto colorOffset = normalOffset + sizeof(vertexArray[0].normal);
	auto texCoordOffset = colorOffset + sizeof(vertexArray[0].color);

	if (vertexVBO)
	{
		glCheck(glDeleteBuffers(1, &vertexVBO));
		glCheck(glDeleteVertexArrays(1, &vao));
	}

	glCheck(glGenVertexArrays(1, &vao));
	glCheck(glBindVertexArray(vao));
	glCheck(glGenBuffers(1, &vertexVBO));

	glCheck(glBindBuffer(GL_ARRAY_BUFFER, vertexVBO));
	glCheck(glBufferData(GL_ARRAY_BUFFER, drawCount * stride, vertexArray.data(), GL_STATIC_DRAW));
	glCheck(glEnableVertexAttribArray(static_cast<GLuint>(V3DVertexAttribute::Position)));
	glCheck(glVertexAttribPointer(static_cast<GLuint>(V3DVertexAttribute::Position),	3, GL_FLOAT, GL_FALSE,	stride, 0));
	glCheck(glEnableVertexAttribArray(static_cast<GLuint>(V3DVertexAttribute::Normal)));
	glCheck(glVertexAttribPointer(static_cast<GLuint>(V3DVertexAttribute::Normal),		3, GL_FLOAT, GL_TRUE,	stride, (void*)normalOffset));
	glCheck(glEnableVertexAttribArray(static_cast<GLuint>(V3DVertexAttribute::Color)));
	glCheck(glVertexAttribPointer(static_cast<GLuint>(V3DVertexAttribute::Color),		4, GL_FLOAT, GL_FALSE,	stride, (void*)colorOffset));
	glCheck(glEnableVertexAttribArray(static_cast<GLuint>(V3DVertexAttribute::TexCoord)));
	glCheck(glVertexAttribPointer(static_cast<GLuint>(V3DVertexAttribute::TexCoord),	2, GL_FLOAT, GL_FALSE,	stride, (void*)texCoordOffset));

	if (indexArray.size() > 0)
	{
		if (indexVBO)
			glCheck(glDeleteBuffers(1, &indexVBO));

		glCheck(glGenBuffers(1, &indexVBO));
		glCheck(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO));
		glCheck(glBufferData(GL_ELEMENT_ARRAY_BUFFER, drawCount * sizeof(indexArray[0]), indexArray.data(), GL_STATIC_DRAW));
	}

	glCheck(glBindVertexArray(0));
	return true;
}

bool V3DModel::LoadTexture(const sf::String& filename)
{
	return texture.loadFromFile(filename);
}

void V3DModel::UpdateShader(V3DShader* shader, V3DCamera* camera)
{
	if (camera)
	{
		glm::mat4 viewProj = camera->PVMatrix() * transform;
		shader->UpdateUniform(UniformType::TransformPVM, &viewProj[0][0]);
		shader->UpdateUniform(UniformType::TransformVM, &camera->ViewMatrix()[0][0]);
		shader->UpdateUniform(UniformType::TransformM, &transform[0][0]);
	}
	else
	{
		glm::mat4 identity;
		shader->UpdateUniform(UniformType::TransformPVM, &transform[0][0]);
		shader->UpdateUniform(UniformType::TransformVM, &identity[0][0]);
		shader->UpdateUniform(UniformType::TransformM, &transform[0][0]);
	}

	shader->UpdateUniform(UniformType::Material, Material);

	if (texture.getSize().x == 0 || texture.getSize().y == 0)
	{
		GenerateDefaultTexture();
	}
}

void V3DModel::Destroy()
{
	VSUPERCLASS::Destroy();
	glCheck(glDeleteBuffers(1, &vertexVBO));
	glCheck(glDeleteBuffers(1, &indexVBO));
	glCheck(glDeleteVertexArrays(1, &vao));

	delete Material;
}

void V3DModel::Draw(sf::RenderTarget& RenderTarget)
{
	if (texture.getSize().x == 0 || texture.getSize().y == 0)
		glCheck(glBindTexture(GL_TEXTURE_2D, DefaultTexture));
	else
		sf::Texture::bind(&texture);

	glCheck(glBindVertexArray(vao));

	if (indexVBO)
		glCheck(glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0));
	else
		glCheck(glDrawArrays(GL_TRIANGLES, 0, drawCount));

	glCheck(glBindVertexArray(0));
	glCheck(glBindTexture(GL_TEXTURE_2D, 0));
}

void V3DModel::GenerateDefaultTexture()
{
	if (DefaultTexture == 0)
	{
		glCheck(glGenTextures(1, &DefaultTexture));

		GLubyte data[] = { 255, 255, 255, 255 };

		glCheck(glBindTexture(GL_TEXTURE_2D, DefaultTexture));

		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		glCheck(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

		glCheck(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));

		glCheck(glBindTexture(GL_TEXTURE_2D, 0));
	}
}
#endif