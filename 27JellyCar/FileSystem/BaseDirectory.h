#ifndef BaseDirectory_H
#define BaseDirectory_H

#include <string>
#include <vector>


class FileManager;

class BaseDirectory{

protected:

	std::string _dirName;

	BaseDirectory();
	BaseDirectory(std::string dirName);

public:

	virtual ~BaseDirectory() = default;

	virtual bool Open() = 0;
	virtual void Close() = 0;

	virtual std::vector<std::string> GetFiles() = 0;
	virtual std::vector<std::string> GetFiles(std::string extension) = 0;

	friend class FileManager;
};
#endif