#pragma once
#include <vector>

/********************************

Class:		CVertexBufferObject

Purpose:	Wraps OpenGL vertex buffer
			object.

********************************/

class CVertexBufferObject
{
public:
	void CreateVBO(int a_iSize = 0);
	void DeleteVBO();

	void* MapBufferToMemory(int iUsageHint);
	void* MapSubBufferToMemory(int iUsageHint, unsigned int uiOffset, unsigned int uiLength);
	void UnmapBuffer();

	void BindVBO(int a_iBufferType = 34962);
	void UploadDataToGPU(int iUsageHint);
	
	void AddData(void* ptrData, unsigned int uiDataSize);

	void* GetDataPointer();
	unsigned int GetBufferID();

	int GetCurrentSize();

	CVertexBufferObject();

private:
	unsigned int uiBuffer;
	int iSize;
	int iCurrentSize;
	int iBufferType;
	std::vector<unsigned char> data;

	bool bDataUploaded;
};