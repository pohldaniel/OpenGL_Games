#include "Shader2.h"

Shader2::Shader2(){

	_vertexShaderFile = "";
	_fragmentShaderFile = "";

}

void Shader2::SetName(std::string name) {
	_name = name;
}

std::string Shader2::GetName(){
	return _name;
}

Shader2::~Shader2(){

}