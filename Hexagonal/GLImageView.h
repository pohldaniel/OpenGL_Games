#pragma once

#include <GL/glew.h>

class Shader;
class GLVertexBuffer;
class Texture;
class GLText;

/*
 This class prepares Zoomed image with the provided parameters.
 This class require different resources( Such as GLSLShader, GLTexture, GLVertexBuffert etc )
 to create a final image.
 This class is prepared to create Multiple views in ZoomInterpolation application.
 */
class GLImageView
{
public:
    GLImageView(void);
    ~GLImageView(void);

    void SetViewport( const int nLeft_i, const int nTop_i,
                      const int nWidth_i, const int nHeight_i );
    void SetVertexBuffer( GLVertexBuffer* pVertexBuffer_i );
    // Input flag indicate shader object need to delete.
    // All In View prepares different shader objects and put it in ImageView list.
    // When ImageView delete, GLSLShader objects deleted from this class.
    void SetShader(Shader* pShader_i , const bool bDeleteShader_i = false );
    //void SetText( GLText* pText_i );
    void SetTextureParam(Texture* pTexture_i, const int nTextureWidth_i,
                          const int nTextureHeight_i);
    void Draw();

private:

    int m_nViewportLeft;
    int m_nViewportTop;
    int m_nViewportWidth;
    int m_nViewportHeight;
    int m_nTextureWidth;
    int m_nTextureHeight;
	Texture* m_pTexture;
	Shader* m_pShader;
    GLVertexBuffer* m_pVertexBuffer;
    bool m_bDeleteShader;
};
