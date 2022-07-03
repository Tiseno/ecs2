#pragma once

#include <vector>

#include "components.h"
#include "entities.h"

template<typename SystemArg>
struct ECS;

template<typename SystemArg>
struct System {
	std::string name;
	System(std::string const& _name) : name(_name) {}
	virtual void update(ECS<SystemArg>& ecs, SystemArg& systemArg) = 0;
};

template<typename SystemArg>
struct ECS {
	Entities entities;
	Components components;
	std::vector<System<SystemArg>*> systems;

	System<SystemArg>* addSystem(System<SystemArg>* system) {
		systems.push_back(system);
		return system;
	}

	void update(SystemArg& arg) {
		for(auto s : systems) {
			s->update(*this, arg);
		}
	}

	bool removeEntity(Entity e) {
		if (entities.remove(e)) {
			components.removeAll(e.index);
			return true;
		}
		return false;
	}

	template<typename... Components>
	Entity createEntity(Components... args) {
		Entity e = entities.create();
		(components.assign(e.index, args), ...);
		return e;
	}

	// TODO entitiesWithComponents<Components...>()
};

