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
Mesh CreateMesh(const float* verts, size_t vertFloatCount,
	const unsigned int* idx, size_t indexCount)
{
	// vao describes how to interpret vertex data
	// vbo holds vertex data
	// ebo holds inices that tell openGl which vertices in the vbo to use and in what order

	Mesh m{};
	m.indexCount = (GLsizei)indexCount;

	glGenVertexArrays(1, &m.vao);
	glGenBuffers(1, &m.vbo);
	glGenBuffers(1, &m.ebo);

	glBindVertexArray(m.vao);

	glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
	glBufferData(GL_ARRAY_BUFFER, vertFloatCount * sizeof(float), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), idx, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	return m;
}
void DeleteMesh(Mesh* m)
{
	if (!m) return;

	if (m->vao) glDeleteVertexArrays(1, &m->vao);
	if (m->vbo) glDeleteBuffers(1, &m->vbo);
	if (m->ebo) glDeleteBuffers(1, &m->ebo);

	m->vao = m->vbo = m->ebo = 0;
}

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

	// ---------- RECTANGLE MESH ----------
	float rectVerts[] =
	{
		-0.08f, 0.08f, 0.0f,
		0.08f, 0.08f, 0.0f,
		0.08f, -0.08f, 0.0f,
		-0.08f, -0.08f, 0.0f
	};
	unsigned int rectIdx[] =
	{
		0,1,2, // tri 1
		2,3,0 // tri 2
	};		
	Mesh rectMesh = CreateMesh(rectVerts, sizeof(rectVerts) / sizeof(float),
		rectIdx, sizeof(rectIdx) / sizeof(unsigned int));

	// ---------- TRIANGLE MESH ----------
	float triVerts[]
	{
		0.0f, 0.10f, 0.0f,
		-0.10f, -0.10f, 0.0f,
		0.10f, -0.10f, 0.0f
	};
	unsigned int triIdx[] = { 0,1,2 };
	Mesh triMesh = CreateMesh(triVerts, sizeof(triVerts) / sizeof(float),
		triIdx, sizeof(triIdx) / sizeof(unsigned int));

	// ---------- RHOMBUS MESH ----------
	float rhoVerts[] = 
	{
		-0.1f, 0.0f, 0.0f,
		0.1f, 0.0f, 0.0f,
		0.0f, 0.2f, 0.0f,
		0.0f, -0.2f, 0.0f		
	};
	unsigned int rhoIdx[] = 
	{
		0, 2, 1,
		0, 1, 3
	};
	Mesh rhoMesh = CreateMesh(rhoVerts, sizeof(rhoVerts) / sizeof(float),
		rhoIdx, sizeof(rhoIdx) / sizeof(unsigned int));		

	// ---------- PENTAGON MESH ----------
	float pentaVerts[] =
	{
		0.0f, 0.15f, 0.0f, // top 0
		0.15f, 0.0f, 0.0f, // right 1
		0.1f, -0.2f, 0.0f, // bottom right 2
		-0.1f, -0.2, 0.0f, // bottom left 3
		-0.15f, 0.0f, 0.0f // left 4
	};
	unsigned int pentaIdx[] =
	{
		0, 1, 2,
		0, 2, 3,
		0, 3, 4
	};
	Mesh pentaMesh = CreateMesh(pentaVerts, sizeof(pentaVerts) / sizeof(float),
		pentaIdx, sizeof(pentaIdx) / sizeof(unsigned int));

	std::vector<Object2D> objects = {
	{{-0.6f,  0.4f}, {-0.6f,  0.4f}, {1,0,0}, Behaviour::Static, &rectMesh},
	{{-0.2f,  0.4f}, {-0.2f,  0.4f}, {0,1,0}, Behaviour::Animated, &triMesh},
	{{ 0.2f,  0.4f}, { 0.2f,  0.4f}, {0,0,1}, Behaviour::Animated, &rectMesh},
	{{-0.2f,  0.0f}, {-0.2f,  0.0f}, {1,1,0}, Behaviour::Static, &pentaMesh},
	{{ 0.2f,  0.0f}, { 0.2f,  0.0f}, {1,0,1}, Behaviour::PlayerInput, &rhoMesh},
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

			glBindVertexArray(obj.mesh -> vao);
			glDrawElements(GL_TRIANGLES, obj.mesh -> indexCount, GL_UNSIGNED_INT, 0);
		}
		glBindVertexArray(0);		
		glfwSwapBuffers(window);				
	}

	// -- cleanup --	

	DeleteMesh(&rectMesh);
	DeleteMesh(&triMesh);
	DeleteMesh(&rhoMesh);
	DeleteMesh(&pentaMesh);
	
	glDeleteProgram(shaderProgram);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}