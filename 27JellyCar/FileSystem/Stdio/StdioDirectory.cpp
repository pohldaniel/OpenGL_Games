#include "StdioDirectory.h"

StdioDirectory::StdioDirectory() : BaseDirectory(){
	_directory = nullptr;
}

StdioDirectory::StdioDirectory(std::string dirName) : BaseDirectory(dirName){
	_directory = nullptr;
}

bool StdioDirectory::Open(){
	_directory = opendir(_dirName.c_str());

	return _directory != nullptr;
}

void StdioDirectory::Close(){
	if (_directory != nullptr){
		closedir(_directory);
	}
}

std::vector<std::string> StdioDirectory::GetFiles(){
	std::vector<std::string> files;

	if (_directory != nullptr){

		struct dirent *ent;

		while ((ent = readdir(_directory)) != nullptr){
			if (ent->d_type == DT_REG){
						files.push_back(ent->d_name);
			}
		}

		//rewind to the first file
		rewinddir(_directory);
	}

	return files;
}

std::vector<std::string> StdioDirectory::GetFiles(std::string extension){
	std::vector<std::string> allFiles = GetFiles();
	std::vector<std::string> files;
			
	for (size_t i = 0; i < allFiles.size(); i++){
		if (allFiles[i].find(extension) != std::string::npos){
			files.push_back(allFiles[i]);
		}
	}

	return files;
}