cmake_minimum_required(VERSION 3.10)
project(Engine)

add_library(${PROJECT_NAME}
    source/Engine.h
    source/Engine.cpp
    source/Application.h
    source/Application.cpp
    source/input/InputManager.h
    source/input/InputManager.cpp
    source/graphics/ShaderProgram.h
    source/graphics/ShaderProgram.cpp
    source/graphics/GraphicsAPI.h
    source/graphics/GraphicsAPI.cpp
    source/render/Material.h
    source/render/Material.cpp
    source/render/Mesh.h
    source/render/Mesh.cpp
    source/render/RenderQueue.h
    source/render/RenderQueue.cpp
    source/scene/GameObject.h
    source/scene/GameObject.cpp
    source/scene/Scene.h
    source/scene/Scene.cpp
    source/scene/Component.h
    source/scene/Component.cpp
    source/scene/components/MeshComponent.h
    source/scene/components/MeshComponent.cpp
    source/scene/components/CameraComponent.h
    source/scene/components/CameraComponent.cpp
    source/scene/components/PlayerControllerComponent.h
    source/scene/components/PlayerControllerComponent.cpp
 "source/scene/Component.h" "source/scene/Component.cpp" "source/scene/components/MeshComponent.h" "source/scene/components/MeshComponent.cpp" "source/scene/components/CameraComponent.h" "source/scene/components/CameraComponent.cpp" "source/scene/components/PlayerControllerComponent.h" "source/scene/components/PlayerControllerComponent.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glfw-3.4/include
)

# GLFW
add_subdirectory(thirdparty/glfw-3.4)

# GLEW
add_compile_definitions(GLEW_NO_GLU)
set(BUILD_UTILS OFF CACHE BOOL "utilities" FORCE)
add_subdirectory(thirdparty/glew/build/cmake "${CMAKE_CURRENT_BINARY_DIR}/glew_build")

# GLM (this path depends on where the GLM CMakeLists.txt actually is)
# If your GLM CMakeLists is in thirdparty/glm-1.0.1, keep this:
add_subdirectory(thirdparty/glm-1.0.1)

# If instead it’s in thirdparty/glm-1.0.1/glm, use this:
# add_subdirectory(thirdparty/glm-1.0.1/glm)

target_link_libraries(${PROJECT_NAME} PUBLIC
    glfw
    glew_s
    glm::glm
)

set_property(TARGET glew_s PROPERTY FOLDER GLEW)
set_property(TARGET glew PROPERTY FOLDER GLEW)
