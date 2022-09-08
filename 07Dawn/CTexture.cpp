#include "CTexture.h"
#include "ViewPort.h"

#include "Constants.h"

TextureAtlasCreator TextureAtlasCreator::s_instance;

std::auto_ptr<TextureCache> textureCache(new TextureCache());

sTexture TextureCache::getTextureFromCache(std::string filename){
	if (textures.count(filename) > 0) {
		return textures[filename];
	}

	Texture* tex  = new Texture();
	tex->loadFromFile(filename, true);

	textures[filename].width = tex->getWidth();
	textures[filename].height = tex->getHeight();
	textures[filename].x1 = 0.0f;
	textures[filename].x2 = 1.0f;
	textures[filename].y1 = 0.0f;
	textures[filename].y2 = 1.0f;

	// set the texture file name
	textures[filename].textureFile = filename;

	
	char* bytes = (char*)malloc(tex->getWidth() * tex->getHeight() * tex->getChannels());
	tex->readPixel(bytes);
	TextureAtlasCreator::get().addTexture(textures[filename], bytes, tex->getWidth(), tex->getHeight());
	free(bytes);

	return textures[filename];
}

TextureCache::TextureCache(){
}

TextureCache::~TextureCache(){
}

sTexture& CTexture::getTexture(int index){

	if (index >= m_texture.size()){
		std::cout << "Trying to get an unknown / unloaded texture: " << index << " in CTexture::getTexture()." << std::endl;
	}
	return m_texture[index];
}

std::vector<sTexture>& CTexture::getTexture(){
	return m_texture;
}

void CTexture::LoadIMG(std::string file, int texture_index, bool isOpenGLThreadInThreadedMode, int textureOffsetX, int textureOffsetY) {

	
	if (texture_index >= m_texture.size()){
		m_texture.resize(texture_index + 1);
	}

	m_texture[texture_index] = textureCache->getTextureFromCache(file);
	m_texture[texture_index].textureOffsetX = 0;
	m_texture[texture_index].textureOffsetY = 0;
	//m_texture[texture_index].texture = textureCache->getTextureFromCache(file).texture;
}

void CTexture::DrawTexture(int x, int y, int draw_id, float transparency, float red, float green, float blue, float x_scale, float y_scale) {
	//glColor4f(red, green, blue, transparency);

	if (!CTexture::isRectOnScreen(x, m_texture[draw_id].width * x_scale, y, m_texture[draw_id].height * y_scale)) {
		return;
	}

	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Batchrenderer::get().addQuad(Vector4f(x, y, m_texture[draw_id].width, m_texture[draw_id].height), Vector4f(m_texture[draw_id].x1, m_texture[draw_id].y1, m_texture[draw_id].x2, m_texture[draw_id].y2), m_texture[draw_id].frame);
}

bool CTexture::isRectOnScreen(int left, int width, int bottom, int height){
	ViewPort& viewPort = ViewPort::get();
	float _left = viewPort.getLeft();
	float _right = viewPort.getRight();
	float _bottom = viewPort.getBottom();
	float _top = viewPort.getTop();

	if (left >= (_right) ||   								// object right of screen
		bottom >= (_top) ||									// object above screen
		(left + width) <= _left ||                        	// object left of screen
		(bottom + height) <= _bottom)                      	// object below screen
	{
		return false;
	}
	return true;
}
