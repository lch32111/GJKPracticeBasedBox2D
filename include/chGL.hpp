// MIT License

// Copyright(c) 2022 Chanhaeng Lee

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this softwareand associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :

// The above copyright noticeand this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#ifndef __CH_GL_H__
#define __CH_GL_H__

#include <iostream>
#include <string>
#include <map>

#include <glad/glad.h>

#include "chMath.hpp"
#include "Distance2D.hpp"

class CGRenderLine;
void insertPolygon(CGRenderLine& lR, 
	const Chan::Polygon& p, 
	const Chan::ChTransform& t,
	const Chan::ChVector3& color);

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

	GLuint m_VAO;
	GLuint m_VBO[2];

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

	GLuint m_VAO;
	GLuint m_VBO[3];

	GLuint m_program;
	GLuint m_ProjViewLoc;

	void prepareGLObject();
};

class CGRenderText
{
public:
	CGRenderText() = delete;
	CGRenderText(int& Screen_Width, int& Screen_Height);
	~CGRenderText();

	void renderText(const std::string& text, Chan::ChReal x, Chan::ChReal y, Chan::ChReal scale, const Chan::ChVector3& color);

private:
	int& m_scrWidth;
	int& m_scrHeight;

	GLuint m_VAO;
	GLuint m_VBO;

	GLuint m_program;
	GLuint m_projLoc;
	GLuint m_textSamplerLoc;
	GLuint m_textColorLoc;
	
	struct Character
	{
		GLuint TextureID;			// ID handle of the glyph texture
		Chan::ChVector2 Size;		// Size of glyph
		Chan::ChVector2 Bearing;	// Offset from baseline to left/top of glyph
		GLuint Advance;				// Offset to advance to next glyph
	};
	std::map<GLchar, Character> m_characters;

	void prepareGLObject();
};

bool checkCompileErrors(GLuint shader, std::string type);

#endif