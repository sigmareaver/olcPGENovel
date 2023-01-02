#pragma once

// vcpkg
#include <entt/entt.hpp>

// External
#include <map>

class Entity;
class Scene
{
	friend Entity;
protected:
	entt::registry m_registry;
	std::map<entt::entity, Entity> m_entity_map;
public:
	Scene();
	~Scene();

	Entity& CreateEntity();
	void DestroyEntity(Entity& entity);
	void DestroyEntity(entt::entity& entity);
};

class Entity
{
	friend Scene;
protected:
	entt::entity m_entity;
	Scene& m_scene;
public:
	Entity(entt::entity& entity, Scene& scene);
	Entity() = delete;
	~Entity();

	const inline entt::entity& id() const { return m_entity; }
	template<class T, class... Args>
	T& AddComponent(Args&&... args)
	{
		return m_scene.m_registry.emplace_or_replace<T>(m_entity, std::forward(args));
	}

	template<class T>
	void RemoveComponent()
	{
		m_scene.m_registry.remove<T>(m_entity);
	}

	template<class T>
	T& GetComponent()
	{
		return m_scene.m_registry.try_get<T>(m_entity);
	}
};
