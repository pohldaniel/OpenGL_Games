#pragma once

#include "Globals.h"

//Model array size
#define NUM_MODELS 3

//Model identifiers
enum {
	MODEL_KEY,
	MODEL_PORTAL,
	MODEL_COLUMN
};

class cModel
{
public:
	cModel();
	~cModel();
	void Load();
	void Draw(int model_id);

private:
	int models[NUM_MODELS]; //dentro guardare los identificadores a las displaylist de cada modelo

	int GetDisplayList(char* path);
};