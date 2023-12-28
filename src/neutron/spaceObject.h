#include <vector>
#include <cmath>

#pragma once

class spaceObject {
	inline spaceObject(int mass, int radius, int posX, int posY, int posZ, int speedX, int speedY, int speedZ);
	inline ~spaceObject();
public:
	inline void Tick(double time);
	inline double DistanceFrom(const spaceObject& object) const;

public:
	int mass;
	int radius;
	int x;
	int y;
	int z;
	int vX;
	int vY;
	int vZ;

	static std::vector<spaceObject*> objectList;
	static spaceObject* biggestMass;
};

spaceObject *spaceObject::biggestMass = nullptr;
std::vector<spaceObject*> spaceObject::objectList = {};

spaceObject::spaceObject(int mass, int radius, int posX, int posY, int posZ, int speedX, int speedY, int speedZ) : 
		mass(mass), x(posX), y(posY), z(posZ), vX(speedX), vY(speedY), vZ(speedZ) {
	objectList.emplace_back(this);
	if (!biggestMass || this->mass > biggestMass->mass)
		biggestMass = this;
}


spaceObject::~spaceObject() {
	for (auto it = objectList.begin(); it != objectList.end(); ++it)
		if (*it == this)
			objectList.erase(it);
}


void spaceObject::Tick(double time) {
	x += vX;
	y += vY;
	z += vZ;
}


double spaceObject::DistanceFrom(const spaceObject& object) const
{
	return std::sqrt(
		(object.x - x) * (object.x - x) +
		(object.y - y) * (object.y - y) +
		(object.z - z) * (object.z - z)
	);
}
