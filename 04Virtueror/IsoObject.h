#pragma once

#include "IsoLayer.h"

#include "engine/Vector.h"
#include "engine/Quad.h"
#include "engine/Shader.h"
#include "engine/Spritesheet.h"
#include "engine/Texture.h"

#include "Constants.h"

class IsoObject {

public:
	Quad *m_quad;
	Shader *m_shaderArray;
	Shader *m_shader;
	Matrix4f m_transform = Matrix4f::IDENTITY;
	Spritesheet *m_spriteSheet;
	Texture *m_texture;

	IsoObject(int rows, int cols);
	virtual ~IsoObject();

	IsoLayer * GetLayer() const;
	void SetLayer(IsoLayer * layer);

	int GetRow() const;
	void SetRow(int r);
	int GetCol() const;
	void SetCol(int c);

	int GetRows() const;
	int GetCols() const;

	void SetPosition(int x, int y);
	void SetPosition();

	int GetX() const;
	void SetX(int x);
	int GetY() const;
	void SetY(int y);

	int GetWidth() const;
	int GetHeight() const;

	//void SetTexture(sgl::graphic::Texture * tex);
	void SetAlpha(unsigned char alpha);
	void SetColor(unsigned int color);

	bool IsVisible() const;
	void SetVisible(bool vis);

	virtual void Render(Matrix4f& transform);

	Vector2f m_size = Vector2f(288.0f, 163.0f);
	Vector2f position = Vector2f(0.0f, 0.0f);

private:
	virtual void OnPositionChanged();

private:
	//sgl::graphic::Image * mImg = nullptr;

	IsoLayer * mLayer = nullptr;

	int mRows = 0;
	int mCols = 0;

	int mRow = 0;
	int mCol = 0;

	bool mVisible = true;
};

inline IsoLayer * IsoObject::GetLayer() const { return mLayer; }
inline void IsoObject::SetLayer(IsoLayer * layer) { mLayer = layer; }

inline int IsoObject::GetRow() const { return mRow; }
inline void IsoObject::SetRow(int r) { mRow = r; }
inline int IsoObject::GetCol() const { return mCol; }
inline void IsoObject::SetCol(int c) { mCol = c; }

inline int IsoObject::GetRows() const { return mRows; }
inline int IsoObject::GetCols() const { return mCols; }

inline bool IsoObject::IsVisible() const { return mVisible; }
inline void IsoObject::SetVisible(bool vis) { mVisible = vis; }