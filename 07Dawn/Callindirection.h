#pragma once
#include <string>

class CallIndirection {
public:
	virtual void call() = 0;
	virtual std::string getLuaSaveText() const = 0;
};

class LuaCallIndirection : public CallIndirection {
public:
	LuaCallIndirection();

	virtual void call();
	virtual std::string getLuaSaveText() const;

	void setExecuteText(std::string text);

private:
	std::string luaText;
};