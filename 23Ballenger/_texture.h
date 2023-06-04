#pragma once

#include <string>
#include <iostream>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);
unsigned int loadCubemap(vector<std::string> faces);
unsigned int generateTexture2D(int w, int h);
unsigned int generateTexture3D(int w, int h, int d);
void bindTexture2D(unsigned int tex, int unit = 0);