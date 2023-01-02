#pragma once

namespace Lua
{
	class Engine
	{
	protected:
		static Engine* m_singleton;

		Engine() = default;
		Engine(Engine&) = delete;
	public:
		static inline auto& GetInstance() { if (!m_singleton) m_singleton = new Engine; return *m_singleton; }
	};
}
