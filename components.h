#pragma once

#include "module/sparse_set/sparse_set.h"

#include <iostream>

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
	Component* assign(size_t index, Component const& init) {
		if (componentPools.size() <= id<Component>()) {
			componentPools.resize(id<Component>() + 1, nullptr);
		}
		if (componentPools[id<Component>()] == nullptr) {
			ComponentPool<Component>* newPool = new ComponentPool<Component>();
			componentPools[id<Component>()] = (void*)newPool;
			std::cout << ANSI::RED << "new " << ANSI::RESET << *newPool << "\n";
		}

		auto cp = ((ComponentPool<Component>*)componentPools[id<Component>()])->assign(index, init);
		std::cout << ANSI::GREEN_DARK << "assign " << ANSI::RESET << cp << " to " << index << "\n";
		return cp;
	}

	template<typename Component>
	Component* get(size_t index) {
		auto cp = (static_cast<ComponentPool<Component>*>(componentPools[id<Component>()]))->get(index);
		std::cout << ANSI::GREEN_DARK << "get " << ANSI::RESET << cp << " for " << index << "\n";
		return cp;
	}

	template<typename Component>
	void remove(size_t index) {

		std::cout << ANSI::GREEN_DARK << "remove " << ANSI::RESET << Component::NAME << " from " << index << "\n";
		auto pool = getPool<Component>();
		if (pool == nullptr) {
			return;
		}
		pool->remove(index);
	}

	template<typename Component>
	ComponentPool<Component>* getPool() {
		if (id<Component>() >= componentPools.size()) {
			return nullptr;
		}
		return static_cast<ComponentPool<Component>*>(componentPools[id<Component>()]);
	}

	template<typename Component>
	size_t size() {
		if (getPool<Component>() == nullptr) {
			return 0;
		}
		return getPool<Component>()->sparseSet.dense.size();
	}
};
