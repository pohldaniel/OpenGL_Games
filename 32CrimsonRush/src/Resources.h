#pragma once

#include <string>
#include <unordered_map>
#include <engine/MeshObject/Shape.h>
#include <engine/ObjModel.h>

class Resources{

private:
	std::unordered_map<std::string, ObjModel> models;
public:
	Resources();
	virtual ~Resources();

	void loadPrimitives();

	bool hasModel(std::string name);
	ObjModel* getModel(std::string name);
	void loadModel(std::string path, std::string name);
};

