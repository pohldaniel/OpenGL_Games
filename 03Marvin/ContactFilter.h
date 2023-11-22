#pragma once

#include <iostream>
#include <Box2D\Box2D.h>

class ContactFilter : public b2ContactFilter {

public:
	bool ShouldCollide(b2Fixture* fixtureA, b2Fixture* fixtureB) override {
		/*std::cout << fixtureA->GetFilterData().maskBits << std::endl;
		std::cout << fixtureA->GetFilterData().maskBits << std::endl;
		return fixtureA->GetFilterData().maskBits == fixtureB->GetFilterData().maskBits;*/

		const b2Filter& filterA = fixtureA->GetFilterData();
		const b2Filter& filterB = fixtureB->GetFilterData();

		if (filterA.groupIndex == filterB.groupIndex && filterA.groupIndex != 0){
			return filterA.groupIndex > 0;
		}
		//std::cout << filterA.maskBits << "  " << filterB.categoryBits << std::endl;
		//std::cout << (filterA.categoryBits & 1) << std::endl;
		//std::cout << (filterB.categoryBits & 1) << std::endl;

		bool collide = (filterA.maskBits & filterB.categoryBits) != 0 && (filterA.categoryBits & filterB.maskBits) != 0 && !(filterA.categoryBits & 1) && !(filterB.categoryBits & 1);

		//std::cout << (filterA.maskBits & filterB.categoryBits) << std::endl;
		//std::cout << (filterA.categoryBits & filterB.maskBits) << std::endl;

		return collide;
	}


};