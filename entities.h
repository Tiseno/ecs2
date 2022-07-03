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
} InvalidEntity{INVALID_ENTITY_INDEX, 0};

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
	long long entity_counter = 0;

	Entity create() {
		entity_counter++;
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

	bool isValidIndex(EntityIndex i) {
		return i < entities.size() && isValid(entities[i]);
	}

	Entity getByIndex(EntityIndex i) {
		if (isValidIndex(i)) {
			return entities[i];
		}
		return InvalidEntity;
	}

	bool isValid(Entity e) {
		return e.index < entities.size() && e.index != INVALID_ENTITY_INDEX && entities[e.index].index != INVALID_ENTITY_INDEX && entities[e.index].version == e.version;
	}

	bool remove(Entity e) {
		if (isValid(e)) {
			entities[e.index].index = INVALID_ENTITY_INDEX;
			freeEntityIndexes.push_back(e.index);
			return true;
		}
		return false;
	}

	Entity getRandom() {
		if(entities.size() == 0) {
			return InvalidEntity;
		}
		auto i = rand() % entities.size();
		// Return the first valid found in any direction from the random index
		for(int j = i; j >= 0; j--) {
			if(isValid(entities[j])) {
				return entities[j];
			}
		}
		for(size_t j = i; j < entities.size(); j++) {
			if(isValid(entities[j])) {
				return entities[j];
			}
		}
		return InvalidEntity;
	}

	size_t size() {
		return entities.size() - freeEntityIndexes.size();
	}
};
