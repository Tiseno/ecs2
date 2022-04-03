#pragma once

#include <iostream>
#include <vector>

#include "ansi.h"

typedef size_t EntityIndex;
typedef size_t EntityVersion;

const EntityIndex INVALID_ENTITY_INDEX(-1);

struct Entity {
	EntityIndex index;
	EntityVersion version;

	bool isValid() const {
		return index != INVALID_ENTITY_INDEX;
	}
};

std::ostream &operator<<(std::ostream &os, Entity const& m) {
	os << ANSI::MAGENTA << "Entity{";
	if (m.index == INVALID_ENTITY_INDEX) {
		os << "INVALID";
	} else {
		os << "#" << m.index;
	}
	return os << " v" << m.version << "}" << ANSI::RESET;
}

struct Entities {
	std::vector<Entity> entities;
	std::vector<EntityIndex> freeEntityIndexes;

	Entity create() {
		if (freeEntityIndexes.size() > 0) {
			EntityIndex index = freeEntityIndexes.back();
			freeEntityIndexes.pop_back();
			entities[index].index = index;
			entities[index].version += 1;
			return entities[index];
		} else {
			Entity e{entities.size(), 0};
			entities.push_back(e);
			return e;
		}
	}

	void remove(Entity e) {
		if (!e.isValid() || entities[e.index].version != e.version) {
			return;
		}
		entities[e.index].index = INVALID_ENTITY_INDEX;
		freeEntityIndexes.push_back(e.index);
	}

	Entity getRandom() {
		return entities[rand() % entities.size()];
	}

	size_t size() {
		return entities.size() - freeEntityIndexes.size();
	}
};
