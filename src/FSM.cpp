#include "FSM.h"

namespace FSM
{

	Machine::~Machine()
	{
		while (!m_state_stack.empty())
			Pop();
	}

	void Machine::Initialize()
	{
		//m_event_handler.RegisterListener(this, Event::Type::FSM_PUSH, false);
		//m_event_handler.RegisterListener(this, Event::Type::FSM_POP, false);
		//m_event_handler.RegisterListener(this, Event::Type::FSM_TRANSITION, false);
		RegisterEvent(Event::Type::FSM_PUSH, false);
		RegisterEvent(Event::Type::FSM_POP, false);
		RegisterEvent(Event::Type::FSM_TRANSITION, false);
	}

	void Machine::Exit()
	{
		while (m_state_stack.size())
			Pop();
	}

	void Machine::Update(float fElapsedTime)
	{
		if (!m_state_stack.empty())
			m_state_stack.top()->Update(fElapsedTime);
	}

	void Machine::Render(float fElapsedTime)
	{
		if (!m_state_stack.empty())
			m_state_stack.top()->Render(fElapsedTime);
	}

	void Machine::Transition(State* new_state, const bool clear)
	{
		if (!m_state_stack.empty())
		{
			do
			{
				auto state = m_state_stack.top();
				state->OnExit();
				delete state;
				m_state_stack.pop();
			} while (clear && !m_state_stack.empty());
		}
		m_state_stack.push(new_state);
		m_state_stack.top()->OnEnter();
	}

	void Machine::Push(State* new_state)
	{
		if (!m_state_stack.empty())
			m_state_stack.top()->OnExit();
		m_state_stack.push(new_state);
		m_state_stack.top()->OnEnter();
	}

	void Machine::Pop()
	{
		if (!m_state_stack.empty())
		{
			auto state = m_state_stack.top();
			state->OnExit();
			delete state;
			m_state_stack.pop();
			if (!m_state_stack.empty())
				m_state_stack.top()->OnEnter();
		}
	}

	size_t Machine::Size()
	{
		return m_state_stack.size();
	}

	State* Machine::CurrentState()
	{
		if (!m_state_stack.empty())
			return m_state_stack.top();
		return nullptr;
	}

	void Machine::OnEvent(Event::Event* event)
	{
		switch (event->type)
		{
		case Event::Type::FSM_PUSH:
			Push(static_cast<State*>(event->params.front().value));
			break;
		case Event::Type::FSM_POP:
			Pop();
			break;
		case Event::Type::FSM_TRANSITION:
			{
				auto it = event->params.begin();
				State* new_state = static_cast<State*>(it->value);
				it++;
				bool clear = *static_cast<bool*>(it->value);
				Transition(new_state, clear);
			}
			break;
		default:
			break;
		}
	}
}
