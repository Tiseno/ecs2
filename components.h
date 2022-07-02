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
		<< " #" << m.sparseSet.sparse.size()
		<< "}" << ANSI::RESET;
}

template<typename Component>
std::ostream &operator<<(std::ostream &os, ComponentPool<Component> const* m) {
	if (m == nullptr) {
		return os << ANSI::GRAY << "ComponentPool{INVALID}" << ANSI::RESET;
	}
	return os << *m;
}

struct Components;
typedef void (Components::*RemoveFnPtrType)(size_t);

struct Components {
	std::vector<void*> componentPools;
	std::vector<RemoveFnPtrType> componentPoolRemovers;

	Components() {}

	template<typename Component>
	void registerComponent() {
		if (componentPools.size() <= id<Component>()) {
			componentPools.resize(id<Component>() + 1, nullptr);
		}
		if (componentPools[id<Component>()] == nullptr) {
			ComponentPool<Component>* newPool = new ComponentPool<Component>();
			componentPools[id<Component>()] = (void*)newPool;
			std::cout << ANSI::RED << "new " << ANSI::RESET << *newPool << "\n";
			RemoveFnPtrType fp = &Components::remove<Component>;
			componentPoolRemovers.push_back(fp);
			std::cout << ANSI::RED << "new " << ANSI::RESET << "remover for " << Component::NAME << " " << fp << " " << &fp << "\n";
		}
	}

	void removeAll(size_t index) {
		for(auto fp : componentPoolRemovers) {
			((*this).*fp)(index);
		}
	}

	template<typename Component>
	Component* assign(size_t index, Component const& init) {
		registerComponent<Component>();
		auto c = getPool<Component>()->assign(index, init);
		// std::cout << ANSI::GREEN_DARK << "assign " << ANSI::RESET << c << " to " << index << "\n";
		return c;
	}

	template<typename Component>
	Component* get(size_t index) {
		if (hasPool<Component>()) {
			auto c = getPool<Component>()->get(index);
			// std::cout << ANSI::GREEN_DARK << "get " << ANSI::RESET << c << " for " << index << "\n";
			return c;
		}
		return nullptr;
	}

	template<typename Component>
	bool hasPool() {
		return id<Component>() < componentPools.size() && componentPools[id<Component>()] != nullptr;
	}

	template<typename Component>
	void remove(size_t index) {
		// std::cout << ANSI::GREEN_DARK << "remove " << ANSI::RESET << Component::NAME << " from " << index << "\n";
		if(hasPool<Component>()) {
			getPool<Component>()->remove(index);
		}
	}

	template<typename Component>
	ComponentPool<Component>* getPool() {
		if (hasPool<Component>()) {
			return static_cast<ComponentPool<Component>*>(componentPools[id<Component>()]);
		}
		return nullptr;
	}

	template<typename Component>
	size_t size() {
		if (hasPool<Component>()) {
			return getPool<Component>()->sparseSet.dense.size();
		}
		return 0;
	}

	// template<typename Component...>
	// bool hasPools() {
	// 	// Check all pools
	// 	return id<Component>() < componentPools.size();
	// }
};
