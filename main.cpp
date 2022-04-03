#include "module/sparse_set/sparse_set.h"

#include <iostream>

#include "components.h"
#include "entities.h"
#include "ecs.h"

#include "component.h"

float rf(int min, int max) {
	return (float)(min + (rand() % (max - min)));
}

Entity createRandomEntity(Entities& entities, Components& components) {
	Entity e = entities.create();
	components.assign(e.index, Position{rf(-100, 100), rf(-100, 100)});
	if(rand() % 100 < 10)
		components.assign(e.index, Size{1, 1});
	if(rand() % 100 < 10)
		components.assign(e.index, Velocity{rf(-10, 10)/10, rf(-10, 10)/10});
	if(rand() % 100 < 10)
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
		removeRandomEntity(entities, components);
	}
};

struct UpdatePositionSystem : System<float> {
	UpdatePositionSystem() : System{"UpdatePositionSystem"} {}
	void update(Entities& entities, Components& components, float time) override {
		if (!components.hasPool<Velocity>()
				|| !components.hasPool<Position>()
				|| !components.hasPool<Acceleration>()) {
			std::cout << "Not all pools are initialized yet!\n";
			return;
		}
		auto p = components.getPool<Velocity>();
		auto dkfj = p->sparseSet.dense;
		for(auto& ssd : components.getPool<Velocity>()->sparseSet.dense) {
			auto velocity = &ssd.data;
			auto position = components.get<Position>(ssd.index);
			auto acceleration = components.get<Acceleration>(ssd.index);
			if(position != nullptr && acceleration != nullptr) {
				std::cout << "UpdatePositionSystem iteration " << ssd << "\n";
				velocity->vec.x += acceleration->vec.x; // *time
				velocity->vec.y += acceleration->vec.y;
				position->vec.x += velocity->vec.x;
				position->vec.y += velocity->vec.y;
				std::cout << "\tUpdated " << position << " and " << velocity << " using " << acceleration << "\n";
			}
		}
	}
};

int main() {
	ECS<float> ecs;
	ecs.systems.push_back(new CreateEntitiesSystem());
	ecs.systems.push_back(new UpdatePositionSystem());

	size_t i = 0;
	while(++i < 1000){
		std::cout <<"############################################\n";
		std::cout <<"############################################\n";
		std::cout <<"############################################\n";
		std::cout <<"############################################\n";
		std::cout <<"############################################\n";
		std::cout <<"############################################\n";
		ecs.update(1);
	}
	std::cout << "There are now " << ecs.entities.size() << " entities!\n";
	return 0;
}
