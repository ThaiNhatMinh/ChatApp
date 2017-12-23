#pragma once
#include "header.h"

// base shader class
class Shader
{
public:
	GLuint m_iProgramID;
	GLuint m_iVertexShader;
	GLuint m_iFragmentShader;
	std::map<std::string, GLuint> m_UniformLists;
	
public:

public:
	Shader() {};
	Shader(const char* vertexShaderFile, const char* fragmentShaderFile);

	virtual ~Shader();
	virtual void LinkShader();

	bool Load(const char* vertexShaderFile, const char* fragmentShaderFile);
	inline void Use() { glUseProgram(m_iProgramID); };
	inline void UnUse() { glUseProgram(0); };

	inline GLuint GetAttribLocation(const char* name) { return glGetAttribLocation(m_iProgramID, name); };
	inline GLuint GetUniformLocation(const char* name) { return glGetUniformLocation(m_iProgramID, name); };


	inline void SetUniform(const char* name, float val) { glUniform1f(m_UniformLists[name], val); }
	inline void SetUniform(const char* name, float x, float y) { glUniform2f(m_UniformLists[name], x, y); }
	inline void SetUniform(const char* name, float x, float y, float z) { glUniform3f(m_UniformLists[name], x, y, z); }
	inline void SetUniform(const char* name, vec3 val) { glUniform3f(m_UniformLists[name], val.x, val.y, val.z); }
	inline void SetUniform(const char* name, vec2 val) { glUniform2f(m_UniformLists[name], val.x, val.y); }
	inline void SetUniform(const char* name, int val) { glUniform1i(m_UniformLists[name], val); }
	inline void SetUniformMatrix(GLuint location, const float* data) { glUniformMatrix4fv(location, 1, GL_FALSE, data); }
	inline void SetUniformMatrix(const char* name, const float* data) { glUniformMatrix4fv(m_UniformLists[name], 1, GL_FALSE, data); }


	
};

