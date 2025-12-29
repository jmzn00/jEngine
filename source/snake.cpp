#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <map>
#include <string>
#include <chrono>
#include <snake.h>
struct Vec2
{
    float x, y;
    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}
};
struct Vec2i
{
    int x, y;
    Vec2i() : x(0), y(0) {}
    Vec2i(int x, int y) : x(x), y(y) {}
    bool operator==(const Vec2i& other) const
    {
        return x == other.x && y == other.y;
    }
};
struct Vec3
{
    float r, g, b;
    Vec3() : r(0.0f), g(0.0f), b(0.0f) {}
    Vec3(float r, float g, float b) : r(r), g(g), b(b) {}
};

// Game Constants
const int GRID_WIDTH = 20;
const int GRID_HEIGHT = 20;
const float UPDATE_INTERVAL = 0.15f; //seconds
const float CELL_WIDTH = 2.0f / GRID_WIDTH;
const float CELL_HEIGHT = 2.0f / GRID_HEIGHT;
enum class Direction
{
    Up,
    Down,
    Left,
    Right,
    None
};
Direction snakeDir = Direction::None;
std::vector <Vec2i> snake = { Vec2i(5, 10), Vec2i(4, 10), Vec2i(3, 10) };
Vec2i fruit;
int score = 0;
bool gameOver = false;
bool gameStarted = false;
float timeSinceLastUpdate = 0.0f;
float snakeSpeed = UPDATE_INTERVAL;

std::string vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform vec2 uOffset;
    uniform vec2 uScale;

    void main() {
        vec2 position = (aPos * uScale) + uOffset;
        gl_Position = vec4(position, 0.0, 1.0);
    }
)";
std::string fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec3 uColor;

    void main(){
        FragColor = vec4(uColor, 1.0);
    }
)";

// OpenGL objects
GLuint shaderProgram;
GLuint VAO, VBO;
GLuint uOffsetLoc, uScaleLoc, uColorLoc;

// bitmap font -- each character is 5x5 pixels
const int FONT_WIDTH = 5;
const int FONT_HEIGHT = 5;
const int FONT_SPACING = 1;

int gFps;


// Character definitions (0 = empty, 1 = filled)
std::map <char, std::vector<int>> fontMap =
{
    {' ', {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0}},
    {'A', {0,1,1,0,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}},
    {'B', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0}},
    {'C', {0,1,1,1,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 0,1,1,1,0}},
    {'D', {1,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,1,1,0,0}},
    {'E', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,1,1,1,0}},
    {'F', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}},
    {'G', {0,1,1,1,0, 1,0,0,0,0, 1,0,1,1,0, 1,0,0,1,0, 0,1,1,1,0}},
    {'H', {1,0,0,1,0, 1,0,0,1,0, 1,1,1,1,0, 1,0,0,1,0, 1,0,0,1,0}},
    {'I', {0,1,1,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,1,0}},
    {'J', {0,0,1,1,0, 0,0,0,1,0, 0,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'K', {1,0,0,1,0, 1,0,1,0,0, 1,1,0,0,0, 1,0,1,0,0, 1,0,0,1,0}},
    {'L', {1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,0,0,0,0, 1,1,1,1,0}},
    {'M', {1,0,0,0,1, 1,1,0,1,1, 1,0,1,0,1, 1,0,0,0,1, 1,0,0,0,1}},
    {'N', {1,0,0,0,1, 1,1,0,0,1, 1,0,1,0,1, 1,0,0,1,1, 1,0,0,0,1}},
    {'O', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'P', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,0,0,0, 1,0,0,0,0}},
    {'Q', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,1,0,0, 0,1,0,1,0}},
    {'R', {1,1,1,0,0, 1,0,0,1,0, 1,1,1,0,0, 1,0,1,0,0, 1,0,0,1,0}},
    {'S', {0,1,1,1,0, 1,0,0,0,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'T', {1,1,1,1,1, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}},
    {'U', {1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'V', {1,0,0,0,1, 1,0,0,0,1, 0,1,0,1,0, 0,1,0,1,0, 0,0,1,0,0}},
    {'W', {1,0,0,0,1, 1,0,0,0,1, 1,0,1,0,1, 1,0,1,0,1, 0,1,0,1,0}},
    {'X', {1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,1,0,1,0, 1,0,0,0,1}},
    {'Y', {1,0,0,0,1, 0,1,0,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,0,1,0,0}},
    {'Z', {1,1,1,1,1, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,1}},
    {'0', {0,1,1,0,0, 1,0,0,1,0, 1,0,0,1,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'1', {0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,1,0}},
    {'2', {0,1,1,0,0, 1,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,1,1,1,0}},
    {'3', {1,1,1,0,0, 0,0,0,1,0, 0,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'4', {0,0,1,1,0, 0,1,0,1,0, 1,0,0,1,0, 1,1,1,1,1, 0,0,0,1,0}},
    {'5', {1,1,1,1,0, 1,0,0,0,0, 1,1,1,0,0, 0,0,0,1,0, 1,1,1,0,0}},
    {'6', {0,1,1,0,0, 1,0,0,0,0, 1,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'7', {1,1,1,1,0, 0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 1,0,0,0,0}},
    {'8', {0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0, 1,0,0,1,0, 0,1,1,0,0}},
    {'9', {0,1,1,0,0, 1,0,0,1,0, 0,1,1,1,0, 0,0,0,1,0, 0,1,1,0,0}},
    {':', {0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0, 0,0,1,0,0, 0,0,0,0,0}},
    {'-', {0,0,0,0,0, 0,0,0,0,0, 1,1,1,1,0, 0,0,0,0,0, 0,0,0,0,0}},
    {'.', {0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,0,0,0, 0,0,1,0,0}}
};

//function declarations
void SpawnFruit();
void InitGame();
void ResetGame();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void DrawCell(const Vec2i& position, const Vec3& color);
void DrawChar(char c, float x, float y, float scale, const Vec3& color);
void DrawText(const std::string& text, float x, float y, float scale, const Vec3& color);
void RenderGame(GLFWwindow* window);
void UpdateGame(float deltaTime);
void DrawBorder();
void DrawSnake();
void DrawScore();
void DrawGameOver();
void DrawStartScreen();

int runSnake()
{
    if (!glfwInit())
    {
        std::cout << "Error Initialising GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "jEngine", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Error creating window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwSetWindowPos(window, 2000, 150);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, KeyCallback);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Error initialising GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexShaderCStr = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vertexShaderCStr, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR:VERTEX_SHADER_COMPILATION_FAILED: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentShaderSourceCStr = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fragmentShaderSourceCStr, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR:FRAGMENT_SHADER_COMPILATION_FAILED: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR:SHADER_PROGRAM_LINKING_FAILED: " << infoLog << std::endl;
        glfwTerminate();
        return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    uOffsetLoc = glGetUniformLocation(shaderProgram, "uOffset");
    uScaleLoc = glGetUniformLocation(shaderProgram, "uScale");
    uColorLoc = glGetUniformLocation(shaderProgram, "uColor");

    const float verticies[] =
    {
        -0.5f, -0.5f,
        0.5f, -0.5f,
        -0.5f, 0.5f,
        0.5f, 0.5f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticies), verticies, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    InitGame();

    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        // Calculate Delta Time
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
        lastTime = currentTime;

        if (deltaTime > 0.0f)
            gFps = (int)(1.0f / deltaTime);

        glfwPollEvents();

        UpdateGame(deltaTime);

        RenderGame(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
void UpdateGame(float deltaTime)
{
    if(gameStarted && !gameOver)
    {
        timeSinceLastUpdate += deltaTime;        

        if (timeSinceLastUpdate >= snakeSpeed)
        {
            timeSinceLastUpdate = 0.0f;

            Vec2i newHead = snake[0];

            switch (snakeDir)
            {
            case Direction::Up:
                newHead.y++;
                break;
            case Direction::Down:
                newHead.y--;
                break;
            case Direction::Left:
                newHead.x--;
                break;
            case Direction::Right:
                newHead.x++;
                break;
            case Direction::None:
                return;

            }

            if (newHead.x < 0 || newHead.x >= GRID_WIDTH ||
                newHead.y < 0 || newHead.y >= GRID_HEIGHT)
            {
                gameOver = true;
                return;
            }
            for (const auto& segment : snake)
            {
                if (newHead == segment)
                {
                    gameOver = true;
                    return;
                }
            }

            snake.insert(snake.begin(), newHead);

            if (newHead == fruit)
            {
                score += 10;
                SpawnFruit();

                if (score % 50 == 0 && snakeSpeed > 0.05f)
                {
                    snakeSpeed -= 0.01f;
                }
            }
            else
            {
                snake.pop_back();
            }
        }
    }
}
void RenderGame(GLFWwindow* window)
{
    glClearColor(0.08f, 0.1f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    DrawBorder();

    if (!gameStarted)
    {
        DrawStartScreen();
    }
    else if (gameOver)
    {
        DrawGameOver();
    }
    else
    {
        DrawSnake();
        DrawScore();
    }
    DrawText("FPS: " + std::to_string(gFps), 0.65f, 0.95f, 0.007f, Vec3(0.9f, 0.9f, 0.9f));
    glBindVertexArray(0);
    glfwSwapBuffers(window);
}


void DrawCell(const Vec2i& position, const Vec3& color)
{
    Vec2 offset(
        -1.0f +  position.x * CELL_WIDTH + CELL_WIDTH * 0.5f,
        -1.0f + position.y * CELL_HEIGHT + CELL_HEIGHT * 0.5f
    );

    Vec2 scale(CELL_WIDTH * 0.9f, CELL_HEIGHT * 0.9f);

    glUniform3f(uColorLoc, color.r, color.g, color.b);
    glUniform2f(uOffsetLoc, offset.x, offset.y);
    glUniform2f(uScaleLoc, scale.x, scale.y);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
void DrawChar(char c , float x, float y, float scale, const Vec3& color)
{
    c = std::toupper(c);

    auto it = fontMap.find(c);
    if(it == fontMap.end())
    {
        it = fontMap.find(' ');
    }
    const std::vector<int>& bitmap = it->second;

    float charWidth = FONT_WIDTH * scale;
    float charHeight = FONT_HEIGHT * scale;

    for (int i = 0; i < FONT_HEIGHT; i++)
    {
        for (int j = 0; j < FONT_WIDTH; j++)
        {
            if (bitmap[i * FONT_WIDTH + j])
            {
                Vec2 offset(
                    x + j * scale - charWidth / 2.0f,
                    y - i * scale - charHeight / 2.0f
                );
                glUniform3f(uColorLoc, color.r, color.g, color.b);
                glUniform2f(uOffsetLoc, offset.x, offset.y);
                glUniform2f(uScaleLoc, scale, scale);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            }
        }
    }
}
void DrawText(const std::string& text, float x, float y, float scale, const Vec3& color)
{
    float charWidth = FONT_WIDTH * scale;
    float spacing = FONT_SPACING * scale;
    float totalWidth = text.size() * (charWidth + spacing) - spacing;

    float startX = x - totalWidth / 2.0f;
     
    for (size_t i = 0; i < text.size(); i++)
    {
        DrawChar(text[i], startX + i * (charWidth + spacing), y, scale, color);
    }

}
void DrawBorder()
{
    Vec3 borderColor(0.3f, 0.3f, 0.5f);

    for (int x = -1; x < GRID_WIDTH; x++)
    {
        DrawCell(Vec2i(x, GRID_HEIGHT), borderColor);
    }
    for (int x = -1; x <= GRID_WIDTH; x++)
    {
        DrawCell(Vec2i(x, -1), borderColor);
    }
    for (int y = -1; y <= GRID_HEIGHT; y++)
    {
        DrawCell(Vec2i(-1, y), borderColor);
    }
    for (int y = -1; y < GRID_HEIGHT; y++)
    {
        DrawCell(Vec2i(GRID_WIDTH, y), borderColor);
    }

    Vec3 gridColor(0.15f, 0.17f, 0.2f);
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            if((x+y) % 2 == 0)
            {
                DrawCell(Vec2i(x, y), gridColor);
            }
        }
    }
}
void DrawSnake()
{
    Vec3 headColor(0.0f, 0.95f, 0.3f);
    Vec3 bodyColor(0.0f, 0.7f, 0.1f);

    for (size_t i = 1; i< snake.size(); i++)
    {
        float factor = static_cast<float>(i) / snake.size();
        Vec3 segmentColor(
            bodyColor.r * (1.0f - factor) + 0.1f * factor,
            bodyColor.g * (1.0f - factor) + 0.8f * factor,
            bodyColor.b * (1.0f - factor)
        );
        DrawCell(snake[i], segmentColor);
    }

    DrawCell(snake[0], headColor);
    DrawCell(fruit, Vec3(1.0f, 0.3f, 0.3f));
}
void DrawScore()
{
    std::string scoreText = "SCORE: " + std::to_string(score);
    DrawText(scoreText, 0.0f, 0.9f, 0.02f, Vec3(0.9f, 0.9f, 0.9f));
}
void DrawGameOver()
{
    for (int x = 0; x < GRID_WIDTH; x++)
    {
        for (int y = 0; y < GRID_HEIGHT; y++)
        {
            DrawCell(Vec2i(x, y), Vec3(0.2f, 0.1f, 0.1f));
        }
    }
    DrawText("GAME OVER", 0.0f, 0.1f, 0.03f, Vec3(1.0f, 0.3f, 0.3f));
    // draw Instructions

    DrawText("PRESS R TO RESTART", 0.0f, -0.3f, 0.01f, Vec3(1.0f, 0.3f, 0.3f));
}
void DrawStartScreen()
{
    DrawSnake();

    DrawText("SNAKE GAME", 0.0f, 0.3f, 0.025f, Vec3(0.2f, 0.8f, 0.3f));

    // Draw Instructions

    DrawText("made with jEngine", 0.0f, -0.7f, 0.007f, Vec3(0.2f, 0.8f, 0.3f));
}
void SpawnFruit()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distX(0, GRID_WIDTH - 1);
    std::uniform_int_distribution<> distY(0, GRID_HEIGHT - 1);

    while (true)
    {
        Vec2i newFruit(distX(gen), distY(gen));

        bool validPosition = true;
        for (const auto& segment : snake)
        {
            if (segment == newFruit)
            {
                validPosition = false;
                break;
            }
        }
        if (validPosition)
        {
            fruit = newFruit;
            break;
        }
    }
}
void InitGame()
{
    ResetGame();
    SpawnFruit();
}
void ResetGame()
{
    snake = { Vec2i(5, 10), Vec2i(4, 10), Vec2i(3, 10) };
    snakeDir = Direction::None;
    gameOver = false;
    gameStarted = false;
    score = 0;
    timeSinceLastUpdate = 0.0f;
    snakeSpeed = UPDATE_INTERVAL;
}
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        if (!gameStarted && key != GLFW_KEY_R)
        {
            gameStarted = true;
            snakeDir = Direction::Right;
            return;
        }

        if (gameOver && key == GLFW_KEY_R)
        {
            ResetGame();
            SpawnFruit();
            return;
        }
    }
    if (!gameOver && gameStarted)
    {
        switch (key)
        {
        case GLFW_KEY_UP:
        {
            if (snakeDir != Direction::Down)
                snakeDir = Direction::Up;
        }
        break;
        case GLFW_KEY_DOWN:
        {
            if (snakeDir != Direction::Up)
                snakeDir = Direction::Down;
        }
        break;
        case GLFW_KEY_LEFT:
        {
            if (snakeDir != Direction::Right)
                snakeDir = Direction::Left;
        }
        break;
        case GLFW_KEY_RIGHT:
        {
            if (snakeDir != Direction::Left)
                snakeDir = Direction::Right;
        }
        break;
        default:
            break;
        }
    }
}
