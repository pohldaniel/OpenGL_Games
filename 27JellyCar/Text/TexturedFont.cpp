#include "TexturedFont.h"
#include "RenderManager.h"
#include "VertexTypes.h"
#include "FileSystem/FileManager.h"

#include <functional>

#include "utf8-utils.h"
#include "distance-field.h"


#ifdef ANDROMEDA_SWITCH
#define STB_IMAGE_WRITE_IMPLEMENTATION
#endif
//#include "stb_image.h"
//#include "stb_image_write.h"

//#define STB_TRUETYPE_IMPLEMENTATION
//#include "stb_truetype.h"


#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif



        unsigned char TexturedFont::ComputeY(int r, int g, int b)
        {
            return (unsigned char)(((r * 77) + (g * 150) + (29 * b)) >> 8);
        }

        unsigned char* TexturedFont::Convert(unsigned char* data, int img_n, int req_comp, int x, int y)
        {
            int i, j;
            unsigned char* good;

            if (req_comp == img_n) return data;

            good = new unsigned char[req_comp * x * y];//(unsigned char *) malloc(req_comp * x * y);

            for (j = 0; j < (int)y; ++j)
            {
                unsigned char* src = data + j * x * img_n;
                unsigned char* dest = good + j * x * req_comp;

#define COMBO(a,b)  ((a)*8+(b))
#define CASE(a,b)   case COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
                // convert source image with img_n components to one with req_comp components;
                // avoid switch per pixel, so use switch per scanline and massive macros
                switch (COMBO(img_n, req_comp))
                {
                    CASE(1, 2) dest[0] = src[0], dest[1] = 255; break;
                    CASE(1, 3) dest[0] = dest[1] = dest[2] = src[0]; break;
                    CASE(1, 4)
                    {
                        dest[0] = dest[1] = dest[2] = 255;
                        dest[3] = src[0];
                    }
                    break;
                    CASE(2, 1) dest[0] = src[0]; break;
                    CASE(2, 3) dest[0] = dest[1] = dest[2] = src[0]; break;
                    CASE(2, 4) dest[0] = dest[1] = dest[2] = src[0], dest[3] = src[1]; break;
                    CASE(3, 4) dest[0] = src[0], dest[1] = src[1], dest[2] = src[2], dest[3] = 255; break;
                    CASE(3, 1) dest[0] = ComputeY(src[0], src[1], src[2]); break;
                    CASE(3, 2) dest[0] = ComputeY(src[0], src[1], src[2]), dest[1] = 255; break;
                    CASE(4, 1) dest[0] = ComputeY(src[0], src[1], src[2]); break;
                    CASE(4, 2) dest[0] = ComputeY(src[0], src[1], src[2]), dest[1] = src[3]; break;
                    CASE(4, 3) dest[0] = src[0], dest[1] = src[1], dest[2] = src[2]; break;

                }
#undef CASE
            }

            //free(data);
            //data = NULL;

            return good;
        }


		TexturedFont::TexturedFont(TextureAtlas* atlas, float size, std::string filename)
		{
			_atlas = atlas;
			_shader = 0;
			_vertexArray = 0;
			_fontAlign = FontCenter;

			_lastLenght = 0;
			_lastHash = 0;
			_maxLenght = 0;

			_text = "";
			_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			_position = glm::vec2(0.0f,0.0f);

			BaseFile* file = FileManager::Instance()->GetFile(filename);

			if (file == 0)
				return;

			file->Open(Read, Binary);

			int dataSize = 0;
			_buffer = file->GetData(dataSize);

			file->Close();
			delete file;


			//load actual data from memory
			LoadFromMemory(size, _buffer, dataSize);
		}

        //loading distance filed font
        TexturedFont::TexturedFont(float size, std::string filename)
        {
            _atlas = nullptr;
            _shader = 0;
            _vertexArray = 0;
            _fontAlign = FontCenter;

            _lastLenght = 0;
            _lastHash = 0;
            _maxLenght = 0;

            _text = "";
            _color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
            _position = glm::vec2(0.0f, 0.0f);

            LoadDistanceFiledFont(size, filename);
        }

		TexturedFont::~TexturedFont()
		{
			//delete font structure
			texture_font_delete(_font);

			//delete font buffer
			delete[] _buffer;

			//delete vertex buffer
			delete _vertexArray;
		}

		void TexturedFont::LoadFromMemory(float size, void* memory_font, size_t memory_size)
		{
			_font = texture_font_new_from_memory(_atlas->GetAtlas(), size, memory_font, memory_size);

            //test
            //unsigned char* temp_bitmap = new unsigned char[1024 * 1024];

            //stbtt_bakedchar cdata[15]; // ASCII 32..126 is 95 glyphs

            //stbtt_BakeFontBitmap(_buffer, 0, 34.0, temp_bitmap, 1024, 1024, 48, 15, cdata); // no guarantee this fits!

            //////convert data to rgba
            //unsigned char* newData = Convert(temp_bitmap, 1, 4, 1024, 1024);

            //////save image
            //stbi_write_png("eeeee.png", 1024, 1024, 4, newData, 1024 * 4);

            ////dalete data
            //delete[] newData;

            //delete[] memory_font;
		}

        void TexturedFont::LoadDistanceFiledFont(float size, std::string fileName)
        {
            //1. check if files exists
            std::string fontName = fileName + "_" + std::to_string((int)size) + ".bfont";
            std::string imageName = fileName + "_" + std::to_string((int)size) + ".png";

            bool fontExists = FileManager::Instance()->FileExists(fontName);
            bool imageExists = FileManager::Instance()->FileExists(imageName);

            if (fontExists && imageExists)
            {
                //2. load stuff
                
                //load generated font texture
                Texture2* texture = TextureManager::Instance()->LoadFromFile(imageName);

                //create fake font atlas
                _atlas = new TextureAtlas(texture->GetWidth(), texture->GetHeight());

                //load normal font file
                BaseFile* file = FileManager::Instance()->GetFile(fileName);

                if (file == 0)
                    return;

                file->Open(Read, Binary);

                int dataSize = 0;
                _buffer = file->GetData(dataSize);

                file->Close();
                delete file;

                //load font from memory...
                _font = texture_font_new_from_memory(_atlas->GetAtlas(), size, _buffer, dataSize);

                //load glymps
                //open file to write
                BaseFile* saveFile = FileManager::Instance()->GetFile(fontName);
                saveFile->Open(Read, Binary);

                //save sache size and cache text
                uint32_t nameLength = 0;
                std::string cache = "";
                saveFile->Read(&nameLength, sizeof(uint32_t), 1);
                saveFile->Read(&cache[0], sizeof(char), nameLength);

                //save ghlymps count
                uint32_t glymphCount = 0;
                saveFile->Read(&glymphCount, sizeof(uint32_t), 1);

                //save all ghlyps
                for (size_t i = 0; i < glymphCount; i++)
                {
                    texture_glyph_t* glyph = texture_glyph_new();
                
                    saveFile->Read(&glyph->offset_x, sizeof(int), 1);
                    saveFile->Read(&glyph->offset_y, sizeof(int), 1);

                    saveFile->Read(&glyph->width, sizeof(size_t), 1);
                    saveFile->Read(&glyph->height, sizeof(size_t), 1);
                    saveFile->Read(&glyph->codepoint, sizeof(uint32_t), 1);

                    saveFile->Read(&glyph->s0, sizeof(float), 1);
                    saveFile->Read(&glyph->t0, sizeof(float), 1);
                    saveFile->Read(&glyph->s1, sizeof(float), 1);
                    saveFile->Read(&glyph->t1, sizeof(float), 1);

                    saveFile->Read(&glyph->advance_x, sizeof(float), 1);
                    saveFile->Read(&glyph->advance_y, sizeof(float), 1);

                    vector_push_back(_font->glyphs, &glyph);
                }

                saveFile->Close();
                delete saveFile;

                //set atlas texture
                _atlas->SetTexture(texture);
            }
            else
            {
                return;
            }
        }

        bool TexturedFont::CreateDistanceFieldFont(float size, float bigSize,int atlasSize, std::string cache, std::string filename)
        {
            //1. load font file to memory
            BaseFile* file = FileManager::Instance()->GetFile(filename);

            if (file == 0)
                return false;

            file->Open(Read, Binary);

            int dataSize = 0;
            unsigned char* fileData = file->GetData(dataSize);

            file->Close();
            delete file;

            //2. create texture atlas
            TextureAtlas* atlas = new TextureAtlas(atlasSize, atlasSize);

            //3. load font from memory
            texture_font_t* font = texture_font_new_from_memory(atlas->GetAtlas(), size, fileData, dataSize);

            //4. init FT library
            FT_Library library;
            FT_Init_FreeType(&library);

            std::vector< texture_glyph_t*> glyphs;


            //5. create distance fileds for all chars in cache string
            for (char& c : cache)
            {
                texture_glyph_t* glyph = CreateDistanceGlymph(library, atlas, filename.c_str(), &c, bigSize, size, 0.1);

                vector_push_back(font->glyphs, &glyph);
                glyphs.push_back(glyph);
            }

            FT_Done_FreeType(library);

            //6. serialize all glymph info

            std::string newFontName = filename + "_" + std::to_string((int)size) + ".bfont";

            //create file to write
            BaseFile* saveFile = FileManager::Instance()->GetFile(newFontName);

            if (saveFile == 0)
            {
                return false;
            }

            //open file to write
            saveFile->Open(Write, Binary);

            //save sache size and cache text
            uint32_t nameLength = (uint32_t)cache.length();
            saveFile->Write(&nameLength, sizeof(uint32_t), 1);
            saveFile->Write(&cache[0], sizeof(char), strlen(cache.c_str()));

            //save ghlymps count
            uint32_t glymphCount = glyphs.size();
            saveFile->Write(&glymphCount, sizeof(uint32_t), 1);

            //save all ghlyps
            for (texture_glyph_t* glyph : glyphs)
            {
                saveFile->Write(&glyph->offset_x, sizeof(int), 1);
                saveFile->Write(&glyph->offset_y, sizeof(int), 1);

                saveFile->Write(&glyph->width, sizeof(size_t), 1);
                saveFile->Write(&glyph->height, sizeof(size_t), 1);
                saveFile->Write(&glyph->codepoint, sizeof(uint32_t), 1);

                saveFile->Write(&glyph->s0, sizeof(float), 1);
                saveFile->Write(&glyph->t0, sizeof(float), 1);
                saveFile->Write(&glyph->s1, sizeof(float), 1);
                saveFile->Write(&glyph->t1, sizeof(float), 1);

                saveFile->Write(&glyph->advance_x, sizeof(float), 1);
                saveFile->Write(&glyph->advance_y, sizeof(float), 1);

                
            }

            saveFile->Close();
            delete saveFile;

            //7. save atlas to file
            newFontName = filename + "_" + std::to_string((int)size) + ".png";
            atlas->SaveTextur(newFontName);
            delete atlas;

            delete[] fileData;


            return true;
        }

        texture_glyph_t* TexturedFont::CreateDistanceGlymph(FT_Library& library, TextureAtlas* atlas, const char* filename, const char* codepoint, const float   highres_size, const float   lowres_size, const float   padding)
        {
            size_t i, j;

            FT_Face face;

            FT_New_Face(library, filename, 0, &face);
            FT_Select_Charmap(face, FT_ENCODING_UNICODE);
            FT_UInt glyph_index = FT_Get_Char_Index(face, utf8_to_utf32(codepoint));

            // Render glyph at high resolution (highres_size points)
            FT_Set_Char_Size(face, highres_size * 64, 0, 64, 64);
            FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);
            FT_GlyphSlot slot = face->glyph;
            FT_Bitmap bitmap = slot->bitmap;

            // Allocate high resolution buffer
            size_t highres_width = bitmap.width + 2 * padding * highres_size;
            size_t highres_height = bitmap.rows + 2 * padding * highres_size;
            double* highres_data = (double*)malloc(highres_width * highres_height * sizeof(double));
            memset(highres_data, 0, highres_width * highres_height * sizeof(double));

            //set right padding
            int paddingx = (highres_width - bitmap.width) / 2;
            int paddingy = (highres_height - bitmap.rows) / 2;

            // Copy high resolution bitmap with padding and normalize values
            for (j = 0; j < bitmap.rows; ++j)
            {
                for (i = 0; i < bitmap.width; ++i)
                {
                    int x = i + paddingx;
                    int y = j + paddingx;
                    highres_data[y * highres_width + x] = bitmap.buffer[j * bitmap.width + i] / 255.0;
                }
            }

            // Compute distance map
            highres_data = make_distance_mapd(highres_data, highres_width, highres_height);

            // Allocate low resolution buffer
            size_t lowres_width = round(highres_width * lowres_size / highres_size);
            size_t lowres_height = round(highres_height * lowres_width / (float)highres_width);
            double* lowres_data = (double*)malloc(lowres_width * lowres_height * sizeof(double));
            memset(lowres_data, 0, lowres_width * lowres_height * sizeof(double));

            // Scale down highres buffer into lowres buffer
            resize(highres_data, highres_width, highres_height, lowres_data, lowres_width, lowres_height);

            // Convert the (double *) lowres buffer into a (unsigned char *) buffer and
            // rescale values between 0 and 255.
            unsigned char* data = (unsigned char*)malloc(lowres_width * lowres_height * sizeof(unsigned char));
            memset(data, 0, lowres_width * lowres_height * sizeof(unsigned char));

            for (j = 0; j < lowres_height; ++j)
            {
                for (i = 0; i < lowres_width; ++i)
                {
                    double v = lowres_data[j * lowres_width + i];
                    data[j * lowres_width + i] = (int)(255 * (1 - v));
                }
            }

            // Compute new glyph information from highres value
            float ratio = lowres_size / highres_size;
            size_t pitch = lowres_width * sizeof(unsigned char);

            int smallPaddingx = paddingx * ratio;
            int smallPaddingy = paddingy * ratio;

            // Create glyph
            texture_glyph_t* glyph = texture_glyph_new();

            glyph->offset_x = (slot->bitmap_left + paddingx) * ratio;
            glyph->offset_y = (slot->bitmap_top + paddingy) * ratio;

            glyph->width = lowres_width;
            glyph->height = lowres_height;
            glyph->codepoint = utf8_to_utf32(codepoint);

            ivec4 region = texture_atlas_get_region(atlas->GetAtlas(), glyph->width, glyph->height);

            texture_atlas_set_region(atlas->GetAtlas(), region.x, region.y, glyph->width, glyph->height, data, pitch);
            glyph->s0 = ((float)region.x / (float)atlas->GetAtlas()->width);
            glyph->t0 = ((float)region.y / (float)atlas->GetAtlas()->height);
            glyph->s1 = (float)(region.x + glyph->width) / (float)atlas->GetAtlas()->width;
            glyph->t1 = (float)(region.y + glyph->height) / (float)atlas->GetAtlas()->height;

            FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT);

            glyph->advance_x = ratio * face->glyph->advance.x / 64.0;
            glyph->advance_y = ratio * face->glyph->advance.y / 64.0;

            free(highres_data);
            free(lowres_data);
            free(data);
            //free(data2);

            return glyph;
        }

		void TexturedFont::CreateVertexObject()
		{
			//create
			_vertexArray = RenderManager::Instance()->CreateVertexArrayObject(TextureColor, DynamicDraw);

			_vertexArray->CreateVertices(4);
			_vertexArray->CreateIndices(6);

			TextureColorVertex* _simpleData = static_cast<TextureColorVertex*>(_vertexArray->GetVertices());
			unsigned short* _indices = static_cast<unsigned short*>(_vertexArray->GetIndices());

			_simpleData[0].x = 0.0f;	_simpleData[0].y = 0.0f;	_simpleData[0].z = 0.0f;
			_simpleData[1].x = 0.0f;	_simpleData[1].y = 0.0f;	_simpleData[1].z = 0.0f;
			_simpleData[2].x = 0.0f;	_simpleData[2].y = 0.0f;	_simpleData[2].z = 0.0f;
			_simpleData[3].x = 0.0f;	_simpleData[3].y = 0.0f;	_simpleData[3].z = 0.0f;

			_simpleData[0].r = 1.0f;	_simpleData[0].g = 1.0f;	_simpleData[0].b = 1.0f;
			_simpleData[1].r = 1.0f;	_simpleData[1].g = 1.0f;	_simpleData[1].b = 1.0f;
			_simpleData[2].r = 1.0f;	_simpleData[2].g = 1.0f;	_simpleData[2].b = 1.0f;
			_simpleData[3].r = 1.0f;	_simpleData[3].g = 1.0f;	_simpleData[3].b = 1.0f;

			_simpleData[0].u = 0.0f;	_simpleData[0].v = 0.0f;
			_simpleData[1].u = 0.0f;	_simpleData[1].v = 0.0f;
			_simpleData[2].u = 0.0f;	_simpleData[2].v = 0.0f;
			_simpleData[3].u = 0.0f;	_simpleData[3].v = 0.0f;

			//set data
			_indices[0] = 0;
			_indices[1] = 1;
			_indices[2] = 2;
			_indices[3] = 0;
			_indices[4] = 2;
			_indices[5] = 3;

			//generate buffer object
			_vertexArray->Generate();
		}



		int TexturedFont::CacheGlyphs(std::string text)
		{
			return texture_font_load_glyphs(_font, text.c_str());
		}

		void TexturedFont::SetShader(Shader2* shader)
		{
			_shader = shader;
		}

		void TexturedFont::SetAlign(TextureFontAlign fontAlign)
		{
			_fontAlign = fontAlign;
		}

		void TexturedFont::SetPosition(glm::vec2 position)
		{
			_position = position;
		}

		void TexturedFont::SetSolor(glm::vec4 color)
		{
			_color = color;
		}

		void TexturedFont::SetText(std::string text)
		{
			_text = text;
		}

		void TexturedFont::AddText(std::string text, int x, int y, TextureFontAlign align)
		{
			TextPart part;
			part.Text = text;
			part.Position = glm::vec2(x, y);
			part.Color = glm::vec3(1.0f, 1.0f, 1.0f);
			part.FontAlign = align;

			_textParts.push_back(part);
		}

		void TexturedFont::AddText(std::string text, int x, int y, glm::vec3 color, TextureFontAlign align)
		{
			TextPart part;
			part.Text = text;
			part.Position = glm::vec2(x, y);
			part.Color = color;
			part.FontAlign = align;

			_textParts.push_back(part);
		}

		void TexturedFont::AddText(std::string text, glm::vec2 position, glm::vec3 color, TextureFontAlign align)
		{
			TextPart part;
			part.Text = text;
			part.Position = position;
			part.Color = color;
			part.FontAlign = align;

			_textParts.push_back(part);
		}

		float TexturedFont::GetTextLenght(std::string text)
		{
			float lenght = 0;

			const char * chartext = text.c_str();
			int charCount = text.length();

			for (int i = 0; i < charCount; i++)
			{
				texture_glyph_t *glyph = texture_font_get_glyph(_font, chartext + i);
				if (glyph != NULL)
				{
					float kerning = 0.0f;
					if (i > 0)
					{
						kerning = texture_glyph_get_kerning(glyph, chartext + i - 1);
					}

					lenght += kerning;
					lenght += glyph->advance_x;
				}
			}

			return lenght;
		}

		void TexturedFont::PrepareBuffer()
		{
			//get text lenght
			int textLenght = 0;
			int textHash = 0;
			float colorHash = 0.0f;
			float positionHash = 0.0f;
			_draw = true;

			std::string wholeText;

			for (unsigned int i = 0;i < _textParts.size();i++)
			{
				textLenght += _textParts[i].Text.length();
				wholeText += _textParts[i].Text;
				colorHash += _textParts[i].Color.x + _textParts[i].Color.y + _textParts[i].Color.z;
				positionHash += _textParts[i].Position.x + _textParts[i].Position.y;
			}

			if (textLenght == 0)
			{
				_draw = false;

				_lastHash = 0;
				_lastLenght = 0;
				_lastColour = 0;
				_lastPosition = 0;
				return;
			}

			std::hash<std::string> str_hash;
			textHash = str_hash(wholeText);

			//check size
			bool checkSize = _lastLenght != textLenght;
			bool checkHash = _lastHash != textHash;
			bool checkColor = _lastColour != colorHash;
			bool checkPosition = _lastPosition != positionHash;
			bool bufferNull = _vertexArray == 0;
			bool update = false;

			if(checkHash || checkSize || bufferNull || checkColor || checkPosition)
			{
				TextureColorVertex* _simpleData = NULL;
				unsigned short* _indices;

				int verts = textLenght * 4;
				int indices = textLenght * 6;

				//different hash or size but buffer exists
				if ((checkHash || checkSize || checkColor || checkPosition) && !bufferNull)
				{
					if (textLenght > _maxLenght)
					{
						//delete and create bigger buffer
						delete _vertexArray;
						_vertexArray = 0;

						bufferNull = true;
					}
					else
					{
						//update existing buffer
						//_simpleData = new TextureColorVertex[verts];
                        _simpleData = static_cast<TextureColorVertex*>(_vertexArray->GetVertices());
						update = true;
					}
				}

				//if array is null
				if(bufferNull)
				{
					_vertexArray = RenderManager::Instance()->CreateVertexArrayObject(TextureColor, DynamicDraw);

					//verts and indices count
					_vertexArray->CreateVertices(verts);
					_vertexArray->CreateIndices(indices);

					if (_simpleData != NULL)
					{
						delete[] _simpleData;
					}

					_simpleData = static_cast<TextureColorVertex*>(_vertexArray->GetVertices());
					_indices = static_cast<unsigned short*>(_vertexArray->GetIndices());
				}

				int verCount = 0;
				int indiConut = 0;

				for (unsigned int i = 0; i < _textParts.size(); i++)
				{
					const char * text = _textParts[i].Text.c_str();
					int charCount = _textParts[i].Text.length();
					float textLenghtPixels = GetTextLenght(_textParts[i].Text);

					glm::vec2 position;
					glm::vec3 color = _textParts[i].Color;

					//get text start position based on align
					if (_textParts[i].FontAlign == FontLeft)
					{
						position = _textParts[i].Position;
					}

					if (_textParts[i].FontAlign == FontCenter)
					{
						position = glm::vec2(_textParts[i].Position.x - (textLenghtPixels / 2.0f), _textParts[i].Position.y);
					}

					if (_textParts[i].FontAlign == FontRight)
					{
						position = glm::vec2(_textParts[i].Position.x - textLenghtPixels, _textParts[i].Position.y);
					}

					//add vertices for this text
					for (int c = 0; c < charCount; c++)
					{
						texture_glyph_t *glyph = texture_font_get_glyph(_font, text + c);
						if (glyph != NULL)
						{
							float kerning = 0.0f;
							if (c > 0)
							{
								kerning = texture_glyph_get_kerning(glyph, text + c - 1);
							}


                            int offset_x = 0;

                            if (c == 0)
                            {
                                position.x -= glyph->offset_x;
                            }

							position.x += kerning;

							float x0 = (position.x + glyph->offset_x);
							float y0 = (position.y - glyph->offset_y);
							float x1 = (x0 + glyph->width);
							float y1 = (y0 + glyph->height);

							float s0 = glyph->s0;
							float t0 = glyph->t0;
							float s1 = glyph->s1;
							float t1 = glyph->t1;

							_simpleData[verCount + 0].x = x0;	_simpleData[verCount + 0].y = y0;	_simpleData[verCount + 0].z = 0.0f;
							_simpleData[verCount + 1].x = x0;	_simpleData[verCount + 1].y = y1;	_simpleData[verCount + 1].z = 0.0f;
							_simpleData[verCount + 2].x = x1;	_simpleData[verCount + 2].y = y1;	_simpleData[verCount + 2].z = 0.0f;
							_simpleData[verCount + 3].x = x1;	_simpleData[verCount + 3].y = y0;	_simpleData[verCount + 3].z = 0.0f;

							_simpleData[verCount + 0].u = s0;	_simpleData[verCount + 0].v = t0;
							_simpleData[verCount + 1].u = s0;	_simpleData[verCount + 1].v = t1;
							_simpleData[verCount + 2].u = s1;	_simpleData[verCount + 2].v = t1;
							_simpleData[verCount + 3].u = s1;	_simpleData[verCount + 3].v = t0;

							_simpleData[verCount + 0].r = color.x;	_simpleData[verCount + 0].g = color.y;	_simpleData[verCount + 0].b = color.z;
							_simpleData[verCount + 1].r = color.x;	_simpleData[verCount + 1].g = color.y;	_simpleData[verCount + 1].b = color.z;
							_simpleData[verCount + 2].r = color.x;	_simpleData[verCount + 2].g = color.y;	_simpleData[verCount + 2].b = color.z;
							_simpleData[verCount + 3].r = color.x;	_simpleData[verCount + 3].g = color.y;	_simpleData[verCount + 3].b = color.z;

							position.x += glyph->advance_x;

							if (!update)
							{
								_indices[indiConut + 0] = verCount + 0;
								_indices[indiConut + 1] = verCount + 1;
								_indices[indiConut + 2] = verCount + 2;
								_indices[indiConut + 3] = verCount + 0;
								_indices[indiConut + 4] = verCount + 2;
								_indices[indiConut + 5] = verCount + 3;

								indiConut += 6;
							}

							verCount += 4;
						}
					}
				}

				if (update)
				{
					_vertexArray->UpdateVertices(_simpleData, verts, false);
					_vertexArray->SetIndicesNumber(indices);
				}
				else
				{
					_vertexArray->Generate(false);
				}
			}

			_lastHash = textHash;
			_lastLenght = textLenght;
			_lastColour = colorHash;
			_lastPosition = positionHash;

			if(textLenght > _maxLenght)
			{
				_maxLenght = textLenght;
			}			
		}

		void TexturedFont::Draw(glm::mat4 &projection)
		{
			PrepareBuffer();

			if (_draw == true && _vertexArray != 0 && _atlas != 0 && _shader != 0)
			{
				//bind texture
				RenderManager::Instance()->UseTexture(_atlas->GetTexture());

				//shader
				_shader->Bind();

				_shader->SetUniform(VertexShader, "translation", projection);
				//_shader->SetUniform(FragmentShader, "colors", _color);

				_vertexArray->Draw();
			}

			_textParts.clear();
		}

		void TexturedFont::Draw(glm::mat4 &projection, std::string text, int x, int y, TextureFontAlign align)
		{
			SetText(text);
			SetPosition(glm::vec2(x, y));
			SetAlign(align);
			Draw(projection);
		}

        float TexturedFont::MitchellNetravali(float x)
        {
            const float B = 1 / 3.0, C = 1 / 3.0; // Recommended
            // const float B =   1.0, C =   0.0; // Cubic B-spline (smoother results)
            // const float B =   0.0, C = 1/2.0; // Catmull-Rom spline (sharper results)
            x = fabs(x);
            if (x < 1)
                return ((12 - 9 * B - 6 * C) * x * x * x
                    + (-18 + 12 * B + 6 * C) * x * x
                    + (6 - 2 * B)) / 6;
            else if (x < 2)
                return ((-B - 6 * C) * x * x * x
                    + (6 * B + 30 * C) * x * x
                    + (-12 * B - 48 * C) * x
                    + (8 * B + 24 * C)) / 6;
            else
                return 0;
        }

        float TexturedFont::interpolate(float x, float y0, float y1, float y2, float y3)
        {
            float c0 = MitchellNetravali(x - 1);
            float c1 = MitchellNetravali(x);
            float c2 = MitchellNetravali(x + 1);
            float c3 = MitchellNetravali(x + 2);
            float r = c0 * y0 + c1 * y1 + c2 * y2 + c3 * y3;
            return min(max(r, 0.0), 1.0);
        }

        int TexturedFont::resize(double* src_data, size_t src_width, size_t src_height, double* dst_data, size_t dst_width, size_t dst_height)
        {
            if ((src_width == dst_width) && (src_height == dst_height))
            {
                memcpy(dst_data, src_data, src_width * src_height * sizeof(double));
                return 0;
            }
            size_t i, j;
            float xscale = src_width / (float)dst_width;
            float yscale = src_height / (float)dst_height;
            for (j = 0; j < dst_height; ++j)
            {
                for (i = 0; i < dst_width; ++i)
                {
                    int src_i = (int)floor(i * xscale);
                    int src_j = (int)floor(j * yscale);
                    int i0 = min(max(0, src_i - 1), src_width - 1);
                    int i1 = min(max(0, src_i), src_width - 1);
                    int i2 = min(max(0, src_i + 1), src_width - 1);
                    int i3 = min(max(0, src_i + 2), src_width - 1);
                    int j0 = min(max(0, src_j - 1), src_height - 1);
                    int j1 = min(max(0, src_j), src_height - 1);
                    int j2 = min(max(0, src_j + 1), src_height - 1);
                    int j3 = min(max(0, src_j + 2), src_height - 1);
                    float t0 = interpolate(i / (float)dst_width,
                        src_data[j0 * src_width + i0],
                        src_data[j0 * src_width + i1],
                        src_data[j0 * src_width + i2],
                        src_data[j0 * src_width + i3]);
                    float t1 = interpolate(i / (float)dst_width,
                        src_data[j1 * src_width + i0],
                        src_data[j1 * src_width + i1],
                        src_data[j1 * src_width + i2],
                        src_data[j1 * src_width + i3]);
                    float t2 = interpolate(i / (float)dst_width,
                        src_data[j2 * src_width + i0],
                        src_data[j2 * src_width + i1],
                        src_data[j2 * src_width + i2],
                        src_data[j2 * src_width + i3]);
                    float t3 = interpolate(i / (float)dst_width,
                        src_data[j3 * src_width + i0],
                        src_data[j3 * src_width + i1],
                        src_data[j3 * src_width + i2],
                        src_data[j3 * src_width + i3]);
                    float y = interpolate(j / (float)dst_height, t0, t1, t2, t3);
                    dst_data[j * dst_width + i] = y;
                }
            }
            return 0;
        }        

