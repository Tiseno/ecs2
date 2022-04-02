#include <iostream>

#include "module/sparse_set/sparse_set.h"

#include "component.h"
#include "ansi.h"

std::string to_trimmed_string(double n) {
	std::string r = std::to_string(n);
	r.erase(r.find_last_not_of('0') + 1, std::string::npos);
	r.erase(r.find_last_not_of('.') + 1, std::string::npos);
	return r;
}

auto humanReadableBytes(long long bytes) {
	if (bytes < 1000) {
		return std::to_string(bytes) + "B";
	}
	if (bytes < 100000) {
		return to_trimmed_string(static_cast<double>(bytes / 100) / 10) + "KB";
	}
	if (bytes < 1000000) {
		return std::to_string(bytes / 1000) + "KB";
	}
	if (bytes < 100000000) {
		return to_trimmed_string(static_cast<double>(bytes / 100000) / 10) + "MB";
	}
	if (bytes < 1000000000) {
		return std::to_string(bytes / 1000000) + "MB";
	}
	return to_trimmed_string(static_cast<double>(bytes / 100000000) / 10) + "GB";
}

typedef size_t ComponentId;
ComponentId COMPONENT_ID = 0;
template<typename Component>
ComponentId id() {
	static ComponentId i = COMPONENT_ID++;
	return i;
}

typedef size_t EntityIndex;
typedef unsigned int EntityVersion;

const EntityIndex INVALID_ENTITY_INDEX(-1);

// TODO Remove this, as an entity is now only an id
struct EntityId {
	EntityIndex index;
	EntityVersion version;

	bool isValid() const {
		return index != INVALID_ENTITY_INDEX;
	}
};

std::ostream &operator<<(std::ostream &os, EntityId const& m) {
	os << ANSI::GRAY << "EntityId{";
	if (m.index == INVALID_ENTITY_INDEX) {
		os << "INVALID";
	} else {
		os << m.index;
	}
	return os << " " << m.version << "}" << ANSI::RESET;
}

const EntityId INVALID_ENTITY_ID{INVALID_ENTITY_INDEX, 0};

struct Entity {
	EntityId id;

	explicit Entity(EntityId _id) : id(_id) {}

	bool isValid() const {
		return id.isValid();
	}
};

std::ostream &operator<<(std::ostream &os, Entity const& m) {
	os << ANSI::MAGENTA << "Entity{";
	if (m.id.index == INVALID_ENTITY_INDEX) {
		os << "INVALID";
	} else {
		os << m.id.index;
	}
	return os<< " " << m.id.version << "}" << ANSI::RESET;
}

struct Entities {

};

struct Systems {

};

template<typename Component>
struct ComponentPool {
	SparseSet<Component> sparseSet;

	ComponentPool() {}

	~ComponentPool() {}

	Component* get(size_t i) {
		return sparseSet.get(i);
	}

	void remove(size_t i) {
		sparseSet.remove(i);
	}

	Component* assign(size_t i, Component c) {
		auto mu = memoryUsage();
		auto a = sparseSet.assign(i, c);
		if (mu != memoryUsage()) {
			std::cout << "Reallocated " << this << "\n";
		}
		return a;
	}

	long long memoryUsage() const {
		return sparseSet.dense.capacity() * sizeof(SparseSetData<Component>) + sparseSet.sparse.capacity() * sizeof(size_t);
	}
};

template<typename Component>
std::ostream &operator<<(std::ostream &os, ComponentPool<Component> const& m) {
	return os << ANSI::GREEN << "ComponentPool{"
		<< Component::NAME
		<< " " << humanReadableBytes(m.memoryUsage())
		<< " #" << m.sparseSet.dense.size()
		<< "}" << ANSI::RESET;
}

template<typename Component>
std::ostream &operator<<(std::ostream &os, ComponentPool<Component> const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << "ComponentPool{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

struct Components {
	std::vector<void*> componentPools;

	Components() {}

	template<typename Component>
	Component* assign(Entity& entity, Component const& init) {
		if (componentPools.size() <= id<Component>()) {
			componentPools.resize(id<Component>() + 1, nullptr);
		}
		if (componentPools[id<Component>()] == nullptr) {
			ComponentPool<Component>* newPool = new ComponentPool<Component>();
			componentPools[id<Component>()] = (void*)newPool;
			std::cout << "new " << *newPool << "\n";
		}

		auto cp = ((ComponentPool<Component>*)componentPools[id<Component>()])->assign(entity.id.index, init);
		// std::cout << "assign " << cp << " to " << entity << "\n";
		return cp;
	}

	template<typename Component>
	Component* get(Entity const& entity) {
		auto cp = (static_cast<ComponentPool<Component>*>(componentPools[id<Component>()]))->get(entity.id.index);
		// std::cout << "get " << cp << " for " << entity << "\n";
		return cp;
	}

	template<typename Component>
	void remove(Entity const& entity) {
		// std::cout << "remove " << Component::NAME << " from " << entity << "\n";
		(static_cast<ComponentPool<Component>*>(componentPools[id<Component>()]))->remove(entity.id.index);
	}

	void removeAllComponentsForEntity(Entity& entity) {
		// TODO remove all components for entity
		// How do we do this?
		// Probably some sort of template<Pack...> trickery
	}

	template<typename Component>
	ComponentPool<Component>* getPool() {
		return static_cast<ComponentPool<Component>*>(componentPools[id<Component>()]);
	}
};

int main() {
	Components components;
	Entity entity{EntityId{0,1}};
	components.assign(entity, Position{0.1, 0.3});
	components.assign(entity, Velocity{0.11, 0.11});

	auto c = components.get<Position>(entity);
	std::cout << "We got " << c << "!\n";
	components.remove<Position>(entity);
	auto c2 = components.get<Position>(entity);
	if (c2 == nullptr) {
		std::cout << "We got an expected nullptr" << "!\n";
	} else {
		std::cout << "We got " << c << " even though we removed it...\n";
	}

	size_t i = 0;
	while(++i){
		Entity e{EntityId{i,1}};
		components.assign(e, Position{0.1, 0.3});
	}
	return 0;
}
