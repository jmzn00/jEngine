#pragma once
#include <array>
#include <glm/vec2.hpp>
namespace eng
{
	class InputManager
	{
	private:
		InputManager() = default;
		InputManager(const InputManager&) = delete;
		InputManager(InputManager&&) = delete;
		InputManager& operator=(const InputManager&) = delete;
		InputManager& operator=(InputManager&&) = delete;
	public:
		void SetKeyPressed(int key, bool pressed);
		bool IsKeyPressed(int key);

		void SetMouseButtonPressed(int button, bool pressed);
		bool IsMouseButtonPressed(int button);

		void SetMousePoitionOld(const glm::vec2& pos);
		const glm::vec2& GetMousePoistionOld() const;

		void SetMousePositionCurrent(const glm::vec2& pos);
		const glm::vec2& GetMousePositionCurrent() const;
	private:
		std::array<bool, 256> m_keys = { false };
		std::array<bool, 16> m_mouseKeys { false };
		glm::vec2 m_mousePostionOld = glm::vec2(0.0f, 0.0f);
		glm::vec2 m_mousePostionCurrent = glm::vec2(0.0f, 0.0f);
		friend class Engine;
	};
}