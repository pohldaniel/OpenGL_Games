#include <iostream>

#include "Resources.h"

Resources::Resources(){
}

Resources::~Resources(){
	

	ObjModel::Cleanup();
}

void Resources::loadPrimitives(){
	shapes["Bullet"].buildSphere(0.1f, Vector3f(0.0f, 0.0f, 0.0f), 16, 16, false, true, false);
	shapes["Bullet"].createBoundingBox();
	shapes["Bullet"].getAABB().createBuffer();
}

bool Resources::hasModel(std::string name){
	return this->models.count(name) > 0;
}

bool Resources::hasShape(std::string name) {
	return this->shapes.count(name) > 0;
}

ObjModel* Resources::getModel(std::string name){
	ObjModel* model = nullptr;
	if (this->hasModel(name))
		model = &models[name];

	return model;
}

Shape* Resources::getShape(std::string name) {
	Shape* shape = nullptr;
	if (this->hasShape(name))
		shape = &shapes[name];

	return shape;
}

void Resources::loadModel(std::string path, std::string name){
	if (this->hasModel(name)){
		std::cout << name << " has already been loaded\n";
		return;
	}
	models[name].loadModel(("Resources/Models/" + path).c_str());
	models[name].initShader();
	models[name].createAABB();
	//Model model = LoadModel(("Resources/Models/" + path).c_str());
	//this->models.insert(std::pair<std::string, Model>(name, model));
}
