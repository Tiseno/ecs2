#include "module/sparse_set/sparse_set.h"

#include <iostream>

#include "components.h"
#include "entities.h"

#include "component.h"

struct Systems {

};

Entity createRandomEntity(Entities& entities, Components& components) {
	Entity e = entities.create();
	components.assign(e.index, Position{0, 0});
	if(rand() % 100 < 80)
		components.assign(e.index, Size{0, 0});
	if(rand() % 100 < 50)
		components.assign(e.index, Velocity{0, 0});
	if(rand() % 100 < 10)
		components.assign(e.index, Acceleration{0, 0});
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

int main() {
	Entities entities;
	Components components;

	while(true){
		createRandomEntity(entities, components);
		createRandomEntity(entities, components);

		removeRandomEntity(entities, components);

		std::cout << "There are now " << entities.size() << " entities!\n";
	}
	return 0;
}
