#pragma once

#include <string>
#include <unordered_map>
#include <engine/MeshObject/Shape.h>

class Resources{

private:
	std::unordered_map<std::string, Shape> models;
public:
	Resources();
	virtual ~Resources();

	void loadPrimitives();

	bool hasModel(std::string name);
	Shape* getModel(std::string name);
	void loadModel(std::string path, std::string name);
};

