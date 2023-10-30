#ifndef FontManager_H
#define FontManager_H

class FontManager{
private:

	static FontManager* _manager;

	FontManager();

public:

	static FontManager* Instance();
};



#endif