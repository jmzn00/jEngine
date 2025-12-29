#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <vec.h>

class Input
{
public:
	static void Init(GLFWwindow* window);

	static Vec2 GetMoveAxis();
	static bool IsKeyDown(int key);
private:
	static GLFWwindow* s_Window;
};

#endif