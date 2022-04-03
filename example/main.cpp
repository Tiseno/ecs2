#include "../module/sparse_set/sparse_set.h"

#include <iostream>

#include "../components.h"
#include "../entities.h"
#include "../ecs.h"

#include "component.h"

float rf(int min, int max) {
	return (float)(min + (rand() % (max - min)));
}

Entity createRandomEntity(Entities& entities, Components& components) {
	Entity e = entities.create();
	components.assign(e.index, Position{rf(-100, 100), rf(-100, 100)});
	if(rand() % 100 < 50)
		components.assign(e.index, Size{1, 1});
	if(rand() % 100 < 50)
		components.assign(e.index, Velocity{rf(-10, 10)/10, rf(-10, 10)/10});
	if(rand() % 100 < 50)
		components.assign(e.index, Acceleration{rf(-10, 10)/10, rf(-10, 10)/10});
	return e;
}

void removeAllComponentsFromEntity(Entity e, Components& components) {
	components.remove<Position>(e.index);
	components.remove<Size>(e.index);
	components.remove<Velocity>(e.index);
	components.remove<Acceleration>(e.index);
}

void removeRandomEntity(Entities& entities, Components& components) {
	Entity e = entities.getRandom();
	entities.remove(e);
	removeAllComponentsFromEntity(e, components);
}

struct CreateEntitiesSystem : System<float> {
	CreateEntitiesSystem() : System{"CreateEntitiesSystem"} {}
	void update(Entities& entities, Components& components, float time) override {
		createRandomEntity(entities, components);
		createRandomEntity(entities, components);
	}
};

struct UpdatePositionSystem : System<float> {
	UpdatePositionSystem() : System{"UpdatePositionSystem"} {}
	void update(Entities& entities, Components& components, float time) override {
		if (!components.hasPool<Velocity>()
				|| !components.hasPool<Position>()
				|| !components.hasPool<Acceleration>()) {
			return;
		}
		auto p = components.getPool<Velocity>();
		auto dkfj = p->sparseSet.dense;
		auto count = 0;
		for(auto& ssd : components.getPool<Velocity>()->sparseSet.dense) {
			auto velocity = &ssd.data;
			auto position = components.get<Position>(ssd.index);
			auto acceleration = components.get<Acceleration>(ssd.index);
			if(position != nullptr && acceleration != nullptr) {
				count++;
				velocity->vec.x += acceleration->vec.x * time;
				velocity->vec.y += acceleration->vec.y * time;
				position->vec.x += velocity->vec.x * time;
				position->vec.y += velocity->vec.y * time;
			}
		}
	}
};

struct RemoveEntitiesSystem : System<float> {
	size_t i = 0;
	RemoveEntitiesSystem() : System{"RemoveEntitiesSystem"} {}
	void update(Entities& entities, Components& components, float time) override {
		i++;
		if(i % 21 == 0) {
			std::cout << "There are " << entities.size() << " entities\n";
			size_t r = rand() % (int)(entities.size());
			for(size_t j = 0; j < r; j++) {
				removeRandomEntity(entities, components);
			}
			std::cout << ANSI::RED << "Removed " << r << " entities!\n" << ANSI::RESET;
			std::cout << "There are now " << entities.size() << " entities!\n";
		}
	}
};

int main() {
	ECS<float> ecs;
	ecs.systems.push_back(new CreateEntitiesSystem());
	ecs.systems.push_back(new RemoveEntitiesSystem());
	ecs.systems.push_back(new UpdatePositionSystem());

	while(ecs.entities.size() < 100) {
		ecs.update(1);
	}

	for(auto e : ecs.entities.entities) {
		std::cout << e;
		std::cout << " " << ecs.components.get<Position>(e.index);
		std::cout << " " << ecs.components.get<Velocity>(e.index);
		std::cout << " " << ecs.components.get<Acceleration>(e.index);
		std::cout << " " << ecs.components.get<Size>(e.index);
		std::cout << "\n";
	}
	return 0;
}
