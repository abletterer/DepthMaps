#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include "shader.hpp"

ShaderProgram::ShaderProgram()
{
	m_program_id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{}

void ShaderProgram::loadShader(GLenum shaderType, const char* shader_file_path)
{
	GLuint shader_id = glCreateShader(shaderType);

	std::string shaderCode;
	std::ifstream shaderStream(shader_file_path, std::ios::in);
	if(shaderStream.is_open())
	{
		std::string Line = "";
		while(getline(shaderStream, Line))
			shaderCode += "\n" + Line;
		shaderStream.close();
	}
	else
	{
		printf("Impossible to open %s. Are you in the right directory ?\n", shader_file_path);
		getchar();
		return;
	}

	GLint result = GL_FALSE;
	int infoLogLength;

	// Compile Shader
	printf("Compiling shader : %s\n", shader_file_path);
	char const * sourcePointer = shaderCode.c_str();
	glShaderSource(shader_id, 1, &sourcePointer , NULL);
	glCompileShader(shader_id);

	// Check Shader
	glGetShaderiv(shader_id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> shaderErrorMessage(infoLogLength+1);
		glGetShaderInfoLog(shader_id, infoLogLength, NULL, &shaderErrorMessage[0]);
		printf("%s\n", &shaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	glAttachShader(m_program_id, shader_id);
	glLinkProgram(m_program_id);

	// Check the program
	glGetProgramiv(m_program_id, GL_LINK_STATUS, &result);
	glGetProgramiv(m_program_id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if ( infoLogLength > 0 ){
		std::vector<char> programErrorMessage(infoLogLength+1);
		glGetProgramInfoLog(m_program_id, infoLogLength, NULL, &programErrorMessage[0]);
		printf("%s\n", &programErrorMessage[0]);
	}

	glDeleteShader(shader_id);
}


