#pragma once
#include <GL/glew.h>
#include <string>
#include <unordered_map>
#include <glm/mat4x4.hpp>

namespace eng
{
	class ShaderProgram
	{
	public:
		// OpenGl shader program must have a valid program ID, so it must be passed in

		ShaderProgram() = delete; // default contructor is forbidden (prevents invalid obj)
		ShaderProgram(const ShaderProgram&) = delete; // deletes copy constructor, copy constructor controls what happens when an obj is copied and construction time
		ShaderProgram& operator=(const ShaderProgram&) = delete; // deletes copy assignment operator
		explicit ShaderProgram(GLuint shaderProgramID); // only valid way to construct this obj
		~ShaderProgram(); // destructor

		void Bind();
		GLint GetUniformLocation(const std::string& name);
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, float v0, float v1);

		void SetUniform(const std::string& name, const glm::mat4& mat);

	private:
		std::unordered_map<std::string, GLint> m_uniformLocationCache;
		GLuint m_shaderProgramID = 0;
	};

}