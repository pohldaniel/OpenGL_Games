#ifndef DiskFile_H
#define DiskFile_H

#include "../BaseFile.h"


		class FileManager;

		class StdioFile : public BaseFile
		{
		private:

			FILE* _filBuf;

		protected:

			StdioFile();
			StdioFile(std::string fileName, std::string loadFileName);

		public:

			~StdioFile();

			bool Open(FileMode fileMode, FileType fileType) override;
			void Close() override;

			bool Exist() override;
			
			void Read(void* data, unsigned size, int count) override;
			void Write(void *data, unsigned int size, int count) override;

			unsigned char* GetData(int& dataSize) override;
			char* GetTextData(int &dataSize) override;

			friend class FileManager;
		};


#endif