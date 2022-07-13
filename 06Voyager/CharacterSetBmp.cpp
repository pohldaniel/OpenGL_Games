
#include <experimental/filesystem>
#include "CharacterSetBmp.h"

CharacterSetBmp::CharacterSetBmp(CharacterSetBmp const& rhs) {
	characters = rhs.characters;
	maxWidth = rhs.maxWidth;
	maxHeight = rhs.maxHeight;
	lineHeight = rhs.lineHeight;
}

CharacterSetBmp& CharacterSetBmp::operator=(const CharacterSetBmp& rhs) {
	characters = rhs.characters;
	maxWidth = rhs.maxWidth;
	maxHeight = rhs.maxHeight;
	lineHeight = rhs.lineHeight;
	return *this;
}

CharacterSetBmp::~CharacterSetBmp() {
	/*if (spriteSheet) {
		glDeleteTextures(1, &spriteSheet);
		spriteSheet = 0;
	}*/
}

void CharacterSetBmp::loadFromFile(const std::string& path) {
	
	FILE * pFile = fopen(path.c_str(), "r");
	if (pFile == NULL) {
		std::cout << "File not found" << std::endl;
		return ;
	}

	char buffer[250];

	while (fscanf(pFile, "%s", buffer) != EOF) {

		unsigned int scaleWidth;
		unsigned int scaleHeight;

		if (strcmp(buffer, "common") == 0) {
			char lineHeight[50], base[50], scaleW[50], scaleH[50];
			fgets(buffer, sizeof(buffer), pFile);
			sscanf(buffer, "%s\t %s\t %s\t %s\t", &lineHeight, &base, &scaleW, &scaleH);

			char *token = strtok(scaleW, "=");
			token = strtok(NULL, " ");
			scaleWidth = atoi(token);

			token = strtok(scaleH, "=");
			token = strtok(NULL, " ");
			scaleHeight = atoi(token);

		}else if(strcmp(buffer, "char") == 0) {

			CharacterBmp character;

		
			char id[50], x[50], y[50], width[50], height[50], xoffset[50], yoffset[50], xadvance[50];
			fgets(buffer, sizeof(buffer), pFile);
			sscanf(buffer, "%s\t %s\t %s\t %s\t %s\t %s\t%s\t %s\t", &id, &x, &y, &width, &height, &xoffset, &yoffset, &xadvance);

			
			char *token = strtok(x, "=");
			token = strtok(NULL, " ");
			character.textureOffset[0] = atof(token) / static_cast<float>(scaleWidth);

			token = strtok(y, "=");
			token = strtok(NULL, " ");
			character.textureOffset[1] = 1.0f - atof(token) / static_cast<float>(scaleHeight);

			token = strtok(width, "=");
			token = strtok(NULL, " ");
			character.size[0] = atoi(token);
			character.textureSize[0] = character.size[0] / static_cast<float>(scaleWidth);

			token = strtok(height, "=");
			token = strtok(NULL, " ");
			character.size[1] = atoi(token);
			character.textureSize[1] = character.size[1] / static_cast<float>(scaleHeight);

			token = strtok(xoffset, "=");
			token = strtok(NULL, " ");	
			character.bearing[0] = atoi(token);

			token = strtok(yoffset, "=");
			token = strtok(NULL, " ");			
			character.bearing[1] = atoi(token);

			token = strtok(xadvance, "=");
			token = strtok(NULL, " ");
			character.xAdvance = atoi(token);

			token = strtok(id, "=");
			token = strtok(NULL, " ");

			characters.insert(std::pair<char, CharacterBmp>(atoi(token), character));

		}else if (strcmp(buffer, "page") == 0) {
			char id[50], file[50];
			fgets(buffer, sizeof(buffer), pFile);
			sscanf(buffer, "%s\t %s\t", &id, &file);

			char *token = strtok(file, "=");
			token = strtok(NULL, " ");
			std::string fileName = token;
			fileName.erase(std::remove(fileName.begin(), fileName.end(), '\"'), fileName.end());

			std::experimental::filesystem::path path = std::experimental::filesystem::current_path();

			Globals::textureManager.loadTexture(fileName, path.u8string() + "/res/" + fileName, true);
			texture = Globals::textureManager.get(fileName);	
		}

		
	}
}