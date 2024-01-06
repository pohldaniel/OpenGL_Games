#include <iostream>

#include "Resources.h"

Resources::Resources(){
}

Resources::~Resources(){
	//for (auto& element : this->models)
		//UnloadModel(element.second);
}

void Resources::loadPrimitives(){
	//this->models.insert(std::pair<std::string, Model>("Cube", LoadModelFromMesh(GenMeshCube(1.0f, 0.5f, 1.0f))));
	//this->models.insert(std::pair<std::string, Model>("Sphere", LoadModelFromMesh(GenMeshSphere(0.5f, 16, 16))));
	//this->models.insert(std::pair<std::string, Model>("Bullet", LoadModelFromMesh(GenMeshSphere(0.1f, 16, 16))));
	//this->models["Bullet"].materials[0].maps[MaterialMapIndex::MATERIAL_MAP_ALBEDO].color = GRAY;
	//this->models.insert(std::pair<std::string, Model>("Cylinder", LoadModelFromMesh(GenMeshCylinder(1.0f, 2.0f, 16))));
	//this->models.insert(std::pair<std::string, Model>("Plane", LoadModelFromMesh(GenMeshPlane(1.0f, 1.0f, 1, 1))));
}

bool Resources::hasModel(std::string name){
	return this->models.count(name) > 0;
}

Shape* Resources::getModel(std::string name){
	Shape* model = nullptr;
	if (this->hasModel(name))
		model = &models[name];

	return model;
}

void Resources::loadModel(std::string path, std::string name){
	if (this->hasModel(name))
	{
		std::cout << name << " has already been loaded\n";
		return;
	}

	//Model model = LoadModel(("Resources/Models/" + path).c_str());
	//this->models.insert(std::pair<std::string, Model>(name, model));
}
