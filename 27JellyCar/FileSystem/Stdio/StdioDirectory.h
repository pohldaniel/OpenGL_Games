#ifndef StdioDirectory_H
#define StdioDirectory_H

#include "FileSystem/BaseDirectory.h"
#include "FileSystem/dirent.h"

class FileManager;

class StdioDirectory : public BaseDirectory {

private:

	DIR* _directory;

protected:

	StdioDirectory();
	StdioDirectory(std::string dirName);

public:

	bool Open() override;
	void Close() override;

	std::vector<std::string> GetFiles() override;
	std::vector<std::string> GetFiles(std::string extension) override;

	friend class FileManager;
};

#endif
