#include <engine/Shader.h>
#include <engine/Texture.h>

#include "GLImageView.h"
#include "GLVertexBuffer.h"



GLImageView::GLImageView(void)
{
    // Initialize all members.
    m_nViewportHeight = 0;
    m_nViewportLeft = 0;
    m_nViewportWidth = 0;
    m_nViewportHeight = 0;
    m_pVertexBuffer = 0;
    m_pShader = 0;
    m_pTexture = 0;
    //m_pText = 0;
    m_bDeleteShader = false;
}

GLImageView::~GLImageView(void)
{
    if( m_bDeleteShader )
    {
        delete m_pShader;
        m_pShader = 0;
    }
    /*if( m_pText )
    {
        delete m_pText;
        m_pText = 0;
    }*/
}

void GLImageView::SetViewport( const int nLeft_i, const int nTop_i,
                  const int nWidth_i, const int nHeight_i )
{
    m_nViewportLeft = nLeft_i;
    m_nViewportTop = nTop_i;
    m_nViewportWidth = nWidth_i;
    m_nViewportHeight = nHeight_i;
}
void GLImageView::SetVertexBuffer( GLVertexBuffer* pVertexBuffer_i )
{
    m_pVertexBuffer = pVertexBuffer_i;
}
void GLImageView::SetShader(Shader* pShader_i, const bool bDeleteShader_i )
{
    m_pShader = pShader_i;
    m_bDeleteShader = bDeleteShader_i;
}

void GLImageView::SetTextureParam(Texture* pTexture_i, const int nTextureWidth_i,const int nTextureHeight_i ){
    m_pTexture = pTexture_i;
    m_nTextureWidth = nTextureWidth_i;
    m_nTextureHeight = nTextureHeight_i;
}

void GLImageView::Draw()
{
    // Set viewport for Zoomed image display.
    glViewport( m_nViewportLeft, m_nViewportTop,
                m_nViewportWidth, m_nViewportHeight );

    glEnable( GL_TEXTURE_2D );

    if( 0 != m_pShader )
    {
        // If GLSL shader is selected use the same for interpolated image display.
		m_pShader->use();
		m_pShader->loadInt("ImageTexture", 0);
		m_pShader->loadFloat("fWidth", m_nTextureWidth);
		m_pShader->loadFloat("fHeight", m_nTextureHeight);
		m_pTexture->bind();
        m_pVertexBuffer->DrawVertexBuffer( GL_QUADS );
		m_pTexture->unbind();
		m_pShader->unuse();
    }
    else
    {
        
        // OpenGL Interpolation.
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, m_pTexture->getTexture());

        m_pVertexBuffer->DrawVertexBuffer( GL_QUADS );
		glDisable(GL_TEXTURE_2D);
    }

    glDisable( GL_TEXTURE_2D );
    glColor3f( 0.0, 0.0, 1.0 );

    glBegin( GL_LINE_LOOP );

    static float fPixel = 1.0 / 600.0;
    glVertex2d( -1+ fPixel, -1 + fPixel);
    glVertex3f( 1, -1 + fPixel, 0.0 );

    glVertex3f( 1, 1 , 0.0);
    glVertex3f( -1, 1 , 0.0);
    glVertex3f( -1 + fPixel, -1, 0.0 );
    glEnd();
    
    glDisable( GL_TEXTURE_2D );
    glColor3f( 1.0, 1.0, 1.0 );

    /*if( m_pText )
    {
        m_pText->Draw();
    }*/
}
