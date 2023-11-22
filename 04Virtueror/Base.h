#pragma once

#include "Structure.h"

class Base : public Structure {

public:
	Base(int rows, int cols);

protected:
	void UpdateGraphics() override;

private:
	void SetImage();
};
