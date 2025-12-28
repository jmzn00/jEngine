#include <GLFW/glfw3.h>
#include <iostream>

int main()
{
    if (!glfwInit())
    {
        return -1;
    }
    GLFWwindow* window = glfwCreateWindow(1280, 720, "GameDevProject", nullptr, nullptr);

    if (window == nullptr)
    {
        std::cout << "Error Creating window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // optional
    glfwSetWindowPos(window, 2000, 150);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;    
}