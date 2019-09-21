#pragma once
#ifndef __CH_GL_H__
#define __CH_GL_H__

#include <iostream>
#include <string>

#include <glad/glad.h>

#include "chMath.h"

bool checkCompileErrors(GLuint shader, std::string type);

class CGRenderLine
{
public:
	CGRenderLine();
	~CGRenderLine();

	void insertLine(const Chan::ChVector3& From, const Chan::ChVector3& To, const Chan::ChVector3& Color);
	void renderLine(const Chan::ChMat44& proj, const Chan::ChMat44& view, float lineWidth = 1.0);
private:
	enum { e_maxVertices = 2 * 512 };
	Chan::ChVector3 m_vertices[e_maxVertices];
	Chan::ChVector3 m_colors[e_maxVertices];

	unsigned m_count;

	unsigned m_VAO;
	unsigned m_VBO[2];

	GLuint m_program;
	GLuint m_ProjViewLoc;

	void prepareGLObject();
};

class CGRenderPoint
{
public:
	CGRenderPoint();
	~CGRenderPoint();

	void insertPoint(const Chan::ChVector3& P, const Chan::ChVector3& Color, float Size);
	void renderPoint(const Chan::ChMat44& proj, const Chan::ChMat44& view);

private:
	enum { e_maxVertices = 2 * 512 };
	Chan::ChVector3 m_vertices[e_maxVertices];
	Chan::ChVector3 m_colors[e_maxVertices];
	float m_sizes[e_maxVertices];

	unsigned m_count;

	unsigned m_VAO;
	unsigned m_VBO[3];

	GLuint m_program;
	GLuint m_ProjViewLoc;

	void prepareGLObject();
};

#endif