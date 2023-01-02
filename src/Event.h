#pragma once

#include <functional>
#include <list>
#include <map>
#include <queue>

namespace Event
{
	
	enum class Type
	{
		// System related event types
		CLOSE, GOTO_MAINMENU, LOG, BIND_PLAYER, BIND_ROOM, FSM_PUSH, FSM_POP, FSM_TRANSITION,
		SET_KEYBIND,
		// Input related event types
		KEY_DOWN, KEY_UP, MOUSE_DOWN, MOUSE_UP, MOUSE_MOVE,
		// Developer mode event types
		BIND_ROOM_INDEX,
		// Gameplay related event types
		MOVE, SHOW_NPC, SHOW_ITEM, SHOW_SHOP
	};

	class Handler;
	class Listener;

	class Event
	{
	public:
		class Param
		{
		protected:
			bool m_gc;
		public:
			Param(const void* value_, const size_t size_) :
				size(size_)
			{
				if (size_ && value_)
				{
					m_gc = true;
					value = new char[size_];
					memcpy_s(value, size_, value_, size_);
				}
				else
				{
					m_gc = false;
					size = 0;
					value = nullptr;
				}
			};

			Param(void* value_) :
				size(sizeof(void*)),
				value(value_)
			{
				m_gc = false;
			}

			Param(Param& other) :
				size(other.size),
				m_gc(other.m_gc)
			{
				if (size && other.value)
				{
					value = new char[size];
					memcpy_s(value, size, other.value, other.size);
				}
				else
				{
					size = 0;
					value = nullptr;
				}
			};

			Param(Param&& other) noexcept(true) :
				size(other.size),
				value(other.value),
				m_gc(other.m_gc)
			{
				other.m_gc = false;
				other.size = 0;
				other.value = nullptr;
			};

			~Param()
			{
				if (value && m_gc)
					delete value;
			};

			size_t size;
			void* value;
		};
	protected:
		friend Handler;
	public:
		Event(Type type_) : type(type_) {};
		Event(Event& value);
		Event(Event&& value);

		bool AddParam(const void* value, const size_t size);
		bool AddParam(void* ptr);

		Type type;
		std::list<Param> params;
	};

	Handler& operator<<(Handler& handler, Event* rhs);

	class Handler
	{
	protected:
		static Handler* m_singleton;
		std::map<Type, std::list<std::pair<Listener*, bool>>> m_listener_map;
		std::queue<Event*> m_event_queue;
	protected:
		Handler() {};
		void DispatchImmediate(Event* event);
		void Dispatch(Event* event);
	public:
		static Handler& GetInstance();
		friend Handler& operator<<(Handler& handler, Event* rhs);

		void RegisterListener(Listener* listener, const Type type, const bool immediate);
		void UnregisterListener(Listener* listener, const Type type, const bool immediate);
		void ProcessEvents();
		void ClearQueue();
	};

	class Listener
	{
	protected:
		friend Handler;
		Handler& m_event_handler;
		std::list<std::pair<Type, bool>> m_event_types;
	protected:
		void RegisterEvent(Type type, bool immediate) { m_event_handler.RegisterListener(this, type, immediate); m_event_types.push_back({type, immediate}); }
		void UnregisterEvent(Type type, bool immediate) { m_event_handler.UnregisterListener(this, type, immediate); m_event_types.remove({type, immediate}); }
		virtual void OnEventImmediate(Event* event) {}
		virtual void OnEvent(Event* event) {}
	public:
		Listener() : m_event_handler(Handler::GetInstance()) {}
		~Listener() { for (auto& [type, immediate] : m_event_types) m_event_handler.UnregisterListener(this, type, immediate); }
	};

	void Close(bool fquit);
	void MainMenu();
	void SetKeyBind(uint32_t key, std::function<void(int)> lambda);
	
	void FSMPop();

	template<class T>
	void FSMPush()
	{
		Event* event = new Event(Type::FSM_PUSH);
		event->AddParam(new T());
		Handler::GetInstance() << event;
	}

	template<class T>
	void FSMTransition(bool clear)
	{
		Event* event = new Event(Type::FSM_TRANSITION);
		event->AddParam(new T());
		event->AddParam(&clear, sizeof(bool));
		Handler::GetInstance() << event;
	}
}
