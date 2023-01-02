#pragma once

// Internal
#include "Event.h"

// External
#include <functional>
#include <stack>

namespace FSM
{
	class State
	{
	public:
		virtual ~State() {}
		virtual void OnEnter() = 0;
		virtual void OnExit() = 0;
		virtual void Update(float fElapsedTime) = 0;
		virtual void Render(float fElapsedTime) = 0;

	};

	class LState : public State
	{
	protected:
		std::function<void()> m_lambda_OnEnter;
		std::function<void()> m_lambda_OnExit;
		std::function<void(float)> m_lambda_Update;
		std::function<void(float)> m_lambda_Render;
	public:
		LState(std::function<void()> enter, std::function<void(float)> update, std::function<void(float)> render, std::function<void()> exit) :
			m_lambda_OnEnter(std::move(enter)), m_lambda_Update(std::move(update)), m_lambda_Render(std::move(render)), m_lambda_OnExit(std::move(exit)) {};
		virtual ~LState() {}
		virtual void OnEnter() override { m_lambda_OnEnter(); };
		virtual void OnExit() override { m_lambda_OnExit(); };
		virtual void Update(float fElapsedTime) override { m_lambda_Update(fElapsedTime); };
		virtual void Render(float fElapsedTime) override { m_lambda_Render(fElapsedTime); };
	};

	class Machine : public Event::Listener
	{
	protected:
		std::stack<State*> m_state_stack;
	public:
		~Machine();
		void Initialize();
		void Exit();
		void Update(float fElapsedTime);
		void Render(float fElapsedTime);
		void Transition(State* new_state, const bool clear = false);
		void Push(State* new_state);
		void Pop();
		size_t Size();
		State* CurrentState();

		// Inherited via Listener
		virtual void OnEvent(Event::Event* event) override;
	};

}
