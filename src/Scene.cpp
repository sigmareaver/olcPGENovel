#include "Scene.h"

Entity::Entity(entt::entity& entity, Scene& scene) :
	m_entity(entity),
	m_scene(scene)
{
}

Entity::~Entity()
{
}

Scene::Scene()
{
}

Scene::~Scene()
{
}

Entity& Scene::CreateEntity()
{
	entt::entity id = m_registry.create();
	return (*m_entity_map.insert({id, Entity{id, *this}}).first).second;
}

void Scene::DestroyEntity(Entity& entity)
{
	m_entity_map.erase(entity.id());
}

void Scene::DestroyEntity(entt::entity& entity)
{
	m_entity_map.erase(entity);
}
