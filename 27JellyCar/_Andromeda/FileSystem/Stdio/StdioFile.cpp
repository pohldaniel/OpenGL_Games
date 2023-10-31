#include "StdioFile.h"


		StdioFile::StdioFile() : BaseFile()
		{
			_filBuf = nullptr;
		}

		StdioFile::StdioFile(std::string fileName, std::string loadFileName) : BaseFile(fileName, loadFileName)
		{
			_filBuf = nullptr;
		}

		StdioFile::~StdioFile()
		{
			StdioFile::Close();
		}

		bool StdioFile::Open(FileMode fileMode, FileType fileType)
		{
			if(fileMode == FileMode::Read)
			{
				if(fileType == FileType::Binary)
				{
					_filBuf = fopen(_loadFileName.c_str(), "rb");
				}else
				{
					_filBuf = fopen(_loadFileName.c_str(), "r");
				}				
			}
			else if (fileMode == FileMode::Write)
			{
				if (fileType == FileType::Binary)
				{
					_filBuf = fopen(_loadFileName.c_str(), "wb");
				}
				else
				{
					_filBuf = fopen(_loadFileName.c_str(), "w");
				}				
			}
			else
			{
				if (fileType == FileType::Binary)
				{
					_filBuf = fopen(_loadFileName.c_str(), "ab");
				}
				else
				{
					_filBuf = fopen(_loadFileName.c_str(), "a");
				}
			}

			if (_filBuf != 0)
			{
				return true;
			}

			return false;
		}

		void StdioFile::Close()
		{
			if (_filBuf != 0)
				fclose(_filBuf);
		}

		bool StdioFile::Exist()
		{
			if (_filBuf != 0)
				return true;

			return false;
		}

		void StdioFile::Read(void *data, unsigned int size, int count)
		{
			fread(data, size, count, _filBuf);
		}

		void StdioFile::Write(void *data, unsigned int size, int count)
		{
			fwrite(data, size, count, _filBuf);
		}

		unsigned char* StdioFile::GetData(int &dataSize)
		{
			if (_filBuf != 0)
			{
				fseek(_filBuf, 0, SEEK_END);
				dataSize = ftell(_filBuf);
				fseek(_filBuf, 0, SEEK_SET);

				unsigned char* buffer = new unsigned char[dataSize];

				fread(buffer, 1, dataSize, _filBuf);

				return buffer;
			}

			return 0;
		}

		char* StdioFile::GetTextData(int &dataSize)
		{
			if (_filBuf != 0)
			{
				fseek(_filBuf, 0, SEEK_END);
				dataSize = ftell(_filBuf);
				fseek(_filBuf, 0, SEEK_SET);

				char *buffer = new char[dataSize + 1];
				fread(buffer, dataSize, 1, _filBuf);

				buffer[dataSize] = '\0';

				return buffer;
			}

			return 0;
		}

