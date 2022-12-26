#include <sstream>

#include "Callindirection.h"
#include "Luafunctions.h"

LuaCallIndirection::LuaCallIndirection() {
	luaText = "";
}

void LuaCallIndirection::call() {
	if (luaText != "") {
		LuaFunctions::executeLuaScript(luaText);
	}
}

std::string LuaCallIndirection::getLuaSaveText() const {
	std::ostringstream oss;
	std::string objectName = "curEventHandler";
	oss << "local " << objectName << " = DawnInterface.createEventHandler();" << std::endl;
	oss << objectName << ":setExecuteText( [[" << luaText << "]] );" << std::endl;

	return oss.str();
}

void LuaCallIndirection::setExecuteText(std::string text) {
	this->luaText = text;
}

std::string LuaCallIndirection::getExecuteText() const {
	return luaText;
}