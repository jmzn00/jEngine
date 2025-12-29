#include <input.h>

GLFWwindow* Input::s_Window = NULL;

void Input::Init(GLFWwindow* window)
{
	s_Window = window;
}
bool Input::IsKeyDown(int key)
{
	return glfwGetKey(s_Window, key) == GLFW_PRESS;
}
Vec2 Input::GetMoveAxis()
{
	Vec2 dir(0.0f, 0.0f);

	if (IsKeyDown(GLFW_KEY_A)) dir.x -= 1.0f;
	if (IsKeyDown(GLFW_KEY_D)) dir.x += 1.0f;
	if (IsKeyDown(GLFW_KEY_W)) dir.y += 1.0f;
	if (IsKeyDown(GLFW_KEY_S)) dir.y -= 1.0f;

	return dir;
}

