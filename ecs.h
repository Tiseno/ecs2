#pragma once

#include <vector>

#include "components.h"
#include "entities.h"

template<typename SystemArg>
struct System {
	std::string name;
	System(std::string const& _name) : name(_name) {}
	virtual void update(Entities& entities, Components& components, SystemArg& arg) = 0;
};

template<typename SystemArg>
struct ECS {
	Entities entities;
	Components components;
	std::vector<System<SystemArg>*> systems;

	void update(SystemArg& arg) {
		for(auto s : systems) {
			s->update(entities, components, arg);
		}
	}

	bool removeEntity(Entity e) {
		if (entities.remove(e)) {
			components.removeAll(e.index);
			return true;
		}
		return false;
	}

	// TODO createEntity<Components...>(Components...);
	// TODO entitiesWithComponents<Components...>()
};

