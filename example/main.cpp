#include "../module/sparse_set/sparse_set.h"

#include <iostream>

#include "../components.h"
#include "../entities.h"
#include "../ecs.h"

#include "component.h"

float rf(int min, int max) {
	return (float)(min + (rand() % (max - min)));
}

Entity createRandomEntity(ECS<float>& ecs) {
	Entity e = ecs.createEntity(Position{rf(-100, 100), rf(-100, 100)});
	if(rand() % 100 < 50)
		ecs.components.assign(e.index, Size{1, 1});
	if(rand() % 100 < 50)
		ecs.components.assign(e.index, Velocity{rf(-10, 10)/10, rf(-10, 10)/10});
	if(rand() % 100 < 50)
		ecs.components.assign(e.index, Acceleration{rf(-10, 10)/10, rf(-10, 10)/10});
	return e;
}

void removeRandomEntity(ECS<float>& ecs) {
	Entity e = ecs.entities.getRandom();
	ecs.removeEntity(e);
}

struct CreateEntitiesSystem : System<float> {
	CreateEntitiesSystem() : System{"CreateEntitiesSystem"} {}
	void update(ECS<float>& ecs, float& time) override {
		createRandomEntity(ecs);
		createRandomEntity(ecs);
	}
};

struct UpdatePositionSystem : System<float> {
	UpdatePositionSystem() : System{"UpdatePositionSystem"} {}
	void update(ECS<float>& ecs, float& time) override {
		if (!ecs.components.hasPool<Velocity>()
				|| !ecs.components.hasPool<Position>()
				|| !ecs.components.hasPool<Acceleration>()) {
			return;
		}
		auto p = ecs.components.getPool<Velocity>();
		auto dkfj = p->sparseSet.dense;
		auto count = 0;
		for(auto& ssd : ecs.components.getPool<Velocity>()->sparseSet.dense) {
			auto velocity = &ssd.data;
			auto position = ecs.components.get<Position>(ssd.index);
			auto acceleration = ecs.components.get<Acceleration>(ssd.index);
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
	void update(ECS<float>& ecs, float& time) override {
		i++;
		if(i % 21 == 0) {
			std::cout << "There are " << ecs.entities.size() << " entities\n";
			size_t r = rand() % (int)(ecs.entities.size());
			for(size_t j = 0; j < r; j++) {
				removeRandomEntity(ecs);
			}
			std::cout << ANSI::RED << "Removed " << r << " entities!\n" << ANSI::RESET;
			std::cout << "There are now " << ecs.entities.size() << " entities!\n";
		}
	}
};

int main() {
	ECS<float> ecs;
	ecs.systems.push_back(new CreateEntitiesSystem());
	ecs.systems.push_back(new RemoveEntitiesSystem());
	ecs.systems.push_back(new UpdatePositionSystem());

	while(ecs.entities.size() < 100) {
		float time = 1;
		ecs.update(time);
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
