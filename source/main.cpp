#include <snake.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vec.h>
#include <vector>
#include <string>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include<input.h>

enum class Behaviour {Static, PlayerInput, Animated};
struct Mesh
{
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLsizei indexCount = 0;
};
struct Object2D
{
	Vec2 pos;
	Vec2 basePos;
	Vec3 color;
	Behaviour behaviour;
	Mesh* mesh;
};


int main()
{	
	if (!glfwInit())
	{
		cerr << "Error Initializing glfw" << endl;
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "jEngine", NULL, NULL);
	if(window == NULL)
	{
		cerr << "Error Creating Window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	Input::Init(window);

	if(glewInit() != GLEW_OK)
	{
		glfwTerminate();
		return -1;
	}
	GLint success;

	std::string vertexShaderSource = R"(
		#version 330 core		
		layout (location = 0) in vec3 aPos;

		uniform vec2 uOffset;	

		void main()
		{
			gl_Position = vec4(aPos.xy + uOffset, aPos.z, 1.0);
		}
	)";
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexShaderCStr = vertexShaderSource.c_str();
	glShaderSource(vertexShader, 1, &vertexShaderCStr, NULL);
	glCompileShader(vertexShader);
	
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		cerr << "ERROR: VERTEX_SHADER_COMPILATION_FAILED: " << infoLog << endl;
	}

	std::string fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;

		uniform vec3 uColor;		
		void main()
		{
			FragColor = vec4(uColor, 1.0);
		}
	)";
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		cerr << "ERROR: FRAGMENT_SHADER_COMPILATION_FAILED: " << infoLog << endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		cerr << "ERROR: SHADER_PROGRAM_LINKING_FAILED: " << infoLog << endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	/*
	float vertices[] =
	{
		// pos				// colors
		0.0f, 0.5f, 0.0f,	1.0f, 0.0f, 0.0f, 
	   -0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,
	    0.5f,-0.5f, 0.0f,   0.0f, 0.0f, 1.0f
	};
	*/
	Mesh rectMesh;

	float rectVerts[] =
	{
		-0.08f, 0.08f, 0.0f,
		0.08f, 0.08f, 0.0f,
		0.08f, -0.08f, 0.0f,
		-0.08f, -0.08f, 0.0f
	};
	unsigned int rectIdx[] =
	{
		0,1,2,
		2,3,0
	};

	GLuint VAO = 0, VBO = 0, EBO = 0;
	glGenVertexArrays(1, &rectMesh.vao);
	glGenBuffers(1, &rectMesh.vbo);
	glGenBuffers(1, &rectMesh.ebo);

	glBindVertexArray(rectMesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, rectMesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectVerts), rectVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectMesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectIdx), rectIdx, GL_STATIC_DRAW);

	// position attribute (location = 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	rectMesh.indexCount = 6;

	Mesh triMesh;

	float triVerts[]
	{
		0.0f, 0.10f, 0.0f,
		-0.10f, -0.10f, 0.0f,
		0.10f, -0.10f, 0.0f
	};
	unsigned int triIdx[] = { 0,1,2 };

	glGenVertexArrays(1, &triMesh.vao);
	glGenBuffers(1, &triMesh.vbo);
	glGenBuffers(1, &triMesh.ebo);

	glBindVertexArray(triMesh.vao);

	glBindBuffer(GL_ARRAY_BUFFER, triMesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(triVerts), triVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triMesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triIdx), triIdx, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	triMesh.indexCount = 3;

	std::vector<Object2D> objects = {
	{{-0.6f,  0.4f}, {-0.6f,  0.4f}, {1,0,0}, Behaviour::Static, &rectMesh},
	{{-0.2f,  0.4f}, {-0.2f,  0.4f}, {0,1,0}, Behaviour::Animated, &triMesh},
	{{ 0.2f,  0.4f}, { 0.2f,  0.4f}, {0,0,1}, Behaviour::Animated, &rectMesh},
	{{-0.2f,  0.0f}, {-0.2f,  0.0f}, {1,1,0}, Behaviour::Static, &triMesh},
	{{ 0.2f,  0.0f}, { 0.2f,  0.0f}, {1,0,1}, Behaviour::PlayerInput, &rectMesh},
	};	

	glUseProgram(shaderProgram);
	GLint uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
	GLint uColorLoc = glGetUniformLocation(shaderProgram, "uColor");

	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();							
		
		float t = (float)glfwGetTime();

		glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		for (auto& obj : objects)
		{
			if (obj.behaviour == Behaviour::PlayerInput)
			{
				Vec2 move = Input::GetMoveAxis();
				obj.pos.x += move.x * 0.01f;
				obj.pos.y += move.y * 0.01f;
			}
			else if (obj.behaviour == Behaviour::Animated)
			{
				obj.pos.y = obj.basePos.y + sinf(t) * 0.15f;
			}

			glUniform2f(uOffsetLoc, obj.pos.x, obj.pos.y);
			glUniform3f(uColorLoc, obj.color.x, obj.color.y, obj.color.z);

			glBindVertexArray(obj.mesh->vao);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);		
		glfwSwapBuffers(window);				
	}

	// cleanup
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}