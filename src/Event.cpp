// Internals
#include "Event.h"
#include "StringHashMap.h"

// Externals
#include <functional>

namespace Event
{
	Handler* Handler::m_singleton = nullptr;

	Event::Event(Event& value) :
		type(value.type),
		params(value.params.begin(), value.params.end())
	{
	}

	Event::Event(Event&& value) :
		type(std::move(value.type)),
		params(std::move(value.params))
	{
	}

	bool Event::AddParam(const void* value, const size_t size)
	{
		params.emplace_back(std::move(Param{value, size}));
		return true;
	}

	bool Event::AddParam(void* ptr)
	{
		params.push_back(Param{ ptr });
		return true;
	}


	/*void Listener::Receive(Event* event)
	{
		m_event_queue.push(event);
	}*/

	Handler& Handler::GetInstance()
	{
		if (!m_singleton)
			m_singleton = new Handler;
		return *m_singleton;
	}

	void Handler::DispatchImmediate(Event* event)
	{
		for (auto& [listener, immediate] : m_listener_map[event->type])
			if (immediate)
				listener->OnEventImmediate(event);
		/*for (auto it = m_listener_map[event->type].begin(); it != m_listener_map[event->type].end(); ++it)
		{
			//(*it)->Receive(event);
			if ((*it).second)
				(*it).first->OnEventImmediate(event);
		}*/
	}

	void Handler::Dispatch(Event* event)
	{
		for (auto& [listener, immediate] : m_listener_map[event->type])
			if (!immediate)
				listener->OnEvent(event);
		/*for (auto it = m_listener_map[event->type].begin(); it != m_listener_map[event->type].end(); ++it)
		{
			//(*it)->Receive(event);
			if (!(*it).second)
				(*it).first->OnEvent(event);
		}*/
	}

	void Handler::RegisterListener(Listener* listener, const Type type, const bool immediate)
	{
		m_listener_map[type].push_back(std::pair(listener, immediate));
	}

	void Handler::UnregisterListener(Listener* listener, const Type type, const bool immediate)
	{
		for (auto it = m_listener_map[type].begin(); it != m_listener_map[type].end(); ++it)
			if (listener == it->first && immediate == it->second)
			{
				m_listener_map[type].erase(it);
				return;
			}
	}

	void Handler::ProcessEvents()
	{
		while (!m_event_queue.empty())
		{
			auto event = m_event_queue.front();
			Dispatch(event);
			delete event;
			m_event_queue.pop();
		}
	}

	void Handler::ClearQueue()
	{
		while (!m_event_queue.empty())
		{
			auto event = m_event_queue.front();
			delete event;
			m_event_queue.pop();
		}
	}

	Handler& operator<<(Handler& handler, Event* rhs)
	{
		handler.m_event_queue.emplace(std::move(rhs));
		handler.DispatchImmediate(rhs);

		return handler;
	}

	void Close(bool fquit)
	{
		Event* event = new Event(Type::CLOSE);
		event->AddParam(&fquit, sizeof(bool));
		Handler::GetInstance() << event;
	}

	void MainMenu()
	{
		Handler::GetInstance() << new Event(Type::GOTO_MAINMENU);
	}

	void FSMPop()
	{
		Event* event = new Event(Type::FSM_POP);
		Handler::GetInstance() << event;
	}

	void SetKeyBind(uint32_t key, std::function<void(int)> lambda)
	{
		Event* event = new Event(Type::SET_KEYBIND);
		event->AddParam(&key, sizeof(uint32_t));
		event->AddParam(&lambda);
		Handler::GetInstance() << event;
	}

}
