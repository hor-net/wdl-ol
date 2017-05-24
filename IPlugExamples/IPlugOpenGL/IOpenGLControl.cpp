#include "IOpenGLControl.h"
#include "lice_gl_ctx.h"

extern LICE_pixel LiceColor(const IColor* pColor);
extern float LiceWeight(const IChannelBlend* pBlend);
extern int LiceBlendMode(const IChannelBlend* pBlend);


IOpenGLControl::IOpenGLControl(IPlugBase* pPlug, IRECT pR)
  :	IControl(pPlug, pR, -1)
  , m_bmp(pR.W(), pR.H())
  , mRotateTri(0.f)
  , mRotateQuad(0.f)
{
  CreateFBO(pR.W(), pR.H());
}

IOpenGLControl::~IOpenGLControl()
{
  ReleaseFBO();
}

bool IOpenGLControl::IsDirty()
{
  return true;
}

bool IOpenGLControl::Draw(IGraphics* pGraphics)
{
  FramebufferToGPU();

  DrawGL();

  FramebufferFromGPU();

  _LICE::LICE_Blit(pGraphics->GetDrawBitmap(), &m_bmp, mRECT.L, mRECT.T, 0, 0, mRECT.W(), mRECT.H(), LiceWeight(&mBlend), LiceBlendMode(&mBlend));
  return true;
}

void IOpenGLControl::DrawGL()
{
   // Calculate The Aspect Ratio Of The Window
    glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
    glLoadIdentity();									// Reset The Projection Matrix
    gluPerspective(45.0f,(GLfloat)mRECT.W()/(GLfloat)mRECT.H(), 0.1f, 100.0f);

    glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
    glLoadIdentity();									// Reset The Modelview Matrix
    glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
    glClearColor(0.0f, 0.0f, 0.0f, 1.f);				// Black Background
    glClearDepth(1.0f);									// Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
    glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
    glLoadIdentity();									// Reset The Current Modelview Matrix
    glTranslatef(-1.5f,0.0f,-6.0f);						// Move Left 1.5 Units And Into The Screen 6.0
    glRotatef(mRotateTri,0.0f,1.0f,0.0f);						// Rotate The Triangle On The Y axis ( NEW )
    glBegin(GL_TRIANGLES);								// Start Drawing A Triangle
    glColor3f(1.0f,0.0f,0.0f);						// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Front)
    glColor3f(0.0f,1.0f,0.0f);						// Green
    glVertex3f(-1.0f,-1.0f, 1.0f);					// Left Of Triangle (Front)
    glColor3f(0.0f,0.0f,1.0f);						// Blue
    glVertex3f( 1.0f,-1.0f, 1.0f);					// Right Of Triangle (Front)
    glColor3f(1.0f,0.0f,0.0f);						// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Right)
    glColor3f(0.0f,0.0f,1.0f);						// Blue
    glVertex3f( 1.0f,-1.0f, 1.0f);					// Left Of Triangle (Right)
    glColor3f(0.0f,1.0f,0.0f);						// Green
    glVertex3f( 1.0f,-1.0f, -1.0f);					// Right Of Triangle (Right)
    glColor3f(1.0f,0.0f,0.0f);						// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Back)
    glColor3f(0.0f,1.0f,0.0f);						// Green
    glVertex3f( 1.0f,-1.0f, -1.0f);					// Left Of Triangle (Back)
    glColor3f(0.0f,0.0f,1.0f);						// Blue
    glVertex3f(-1.0f,-1.0f, -1.0f);					// Right Of Triangle (Back)
    glColor3f(1.0f,0.0f,0.0f);						// Red
    glVertex3f( 0.0f, 1.0f, 0.0f);					// Top Of Triangle (Left)
    glColor3f(0.0f,0.0f,1.0f);						// Blue
    glVertex3f(-1.0f,-1.0f,-1.0f);					// Left Of Triangle (Left)
    glColor3f(0.0f,1.0f,0.0f);						// Green
    glVertex3f(-1.0f,-1.0f, 1.0f);					// Right Of Triangle (Left)
    glEnd();											// Done Drawing The Pyramid

    glLoadIdentity();									// Reset The Current Modelview Matrix
    glTranslatef(1.5f,0.0f,-7.0f);						// Move Right 1.5 Units And Into The Screen 7.0
    glRotatef(mRotateQuad,1.0f,1.0f,1.0f);					// Rotate The Quad On The X axis ( NEW )
    glBegin(GL_QUADS);									// Draw A Quad
    glColor3f(0.0f,1.0f,0.0f);						// Set The Color To Green
    glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Top)
    glVertex3f(-1.0f, 1.0f, 1.0f);					// Bottom Left Of The Quad (Top)
    glVertex3f( 1.0f, 1.0f, 1.0f);					// Bottom Right Of The Quad (Top)
    glColor3f(1.0f,0.5f,0.0f);						// Set The Color To Orange
    glVertex3f( 1.0f,-1.0f, 1.0f);					// Top Right Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f, 1.0f);					// Top Left Of The Quad (Bottom)
    glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Bottom)
    glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Bottom)
    glColor3f(1.0f,0.0f,0.0f);						// Set The Color To Red
    glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Front)
    glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Front)
    glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Front)
    glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Front)
    glColor3f(1.0f,1.0f,0.0f);						// Set The Color To Yellow
    glVertex3f( 1.0f,-1.0f,-1.0f);					// Top Right Of The Quad (Back)
    glVertex3f(-1.0f,-1.0f,-1.0f);					// Top Left Of The Quad (Back)
    glVertex3f(-1.0f, 1.0f,-1.0f);					// Bottom Left Of The Quad (Back)
    glVertex3f( 1.0f, 1.0f,-1.0f);					// Bottom Right Of The Quad (Back)
    glColor3f(0.0f,0.0f,1.0f);						// Set The Color To Blue
    glVertex3f(-1.0f, 1.0f, 1.0f);					// Top Right Of The Quad (Left)
    glVertex3f(-1.0f, 1.0f,-1.0f);					// Top Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f,-1.0f);					// Bottom Left Of The Quad (Left)
    glVertex3f(-1.0f,-1.0f, 1.0f);					// Bottom Right Of The Quad (Left)
    glColor3f(1.0f,0.0f,1.0f);						// Set The Color To Violet
    glVertex3f( 1.0f, 1.0f,-1.0f);					// Top Right Of The Quad (Right)
    glVertex3f( 1.0f, 1.0f, 1.0f);					// Top Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f, 1.0f);					// Bottom Left Of The Quad (Right)
    glVertex3f( 1.0f,-1.0f,-1.0f);					// Bottom Right Of The Quad (Right)
    glEnd();											// Done Drawing The Quad

    mRotateTri+=1.5f;											// Increase The Rotation Variable For The Triangle ( NEW )
    mRotateQuad-=1.65f;										// Decrease The Rotation Variable For The Quad ( NEW )

  //glClearColor(0.0f, 0.0f, 1.0f, 1.f);				// Black Background
  //glClearDepth(1.0f);									// Depth Buffer Setup
  //glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
  //glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
  //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
  //glEnable(GL_CULL_FACE);
  //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
  //glLoadIdentity();									// Reset The Current Modelview Matrix
}

bool IOpenGLControl::CreateFBO(int w, int h)
{
  if (LICE_GL_IsValid())
  {   
    if (m_fbo) ReleaseFBO();
    glGenFramebuffersEXT(1, &m_fbo);  // create a new empty FBO
    if (m_fbo)
    {
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);  // bind this FBO so it is the rendering target
      glEnable(GL_TEXTURE_RECTANGLE_ARB); // enable texturing
      glGenTextures(1, &m_tex); // create a new texture to be the backing store
      if (m_tex)
      {
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_tex); // bind this texture so it is the texturing target
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // we won't be scaling it for this purpose
        glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);  // size the texture
        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_RECTANGLE_ARB, m_tex, 0);  // attach the texture as the backing store for the FBO
      }
      glDisable(GL_TEXTURE_RECTANGLE_ARB);  // done texturing
    }
  
    return BindFBO();  // this tests the FBO for validity
  } 
  return false;
}

bool IOpenGLControl::BindFBO()
{
  bool valid = false;
  if (m_fbo)
  {
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);  // bind this FBO so it is the rendering target
    valid = (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) == GL_FRAMEBUFFER_COMPLETE_EXT);
    if (valid)
    {
      int w = mRECT.W();
      int h = mRECT.H();
      glViewport(0, 0, w, h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(0.0, w, 0.0, h);
    }
    else
    {
      ReleaseFBO();
      LICE_GL_CloseCtx(); // if we fail once we're done with GL
    }
  }
  return valid; 
}

void IOpenGLControl::ReleaseFBO()
{
  if (m_fbo)
  {
    glDeleteFramebuffersEXT(1, &m_fbo);
    m_fbo = 0;
  }
  if (m_tex)
  {
    glDeleteTextures(1, &m_tex);
    m_tex = 0;
  }
}

void IOpenGLControl::FramebufferToGPU()
{
  if (BindFBO())
  {
    glDisable(GL_BLEND);
    glRasterPos2i(0, 0);
    glDrawPixels(mRECT.W(), mRECT.H(), GL_BGRA, GL_UNSIGNED_BYTE, m_bmp.getBits());   
  }
}

void IOpenGLControl::FramebufferFromGPU()
{
  if (BindFBO()) 
  {
    glFinish();
    glReadPixels(0, 0, mRECT.W(), mRECT.H(), GL_BGRA, GL_UNSIGNED_BYTE, m_bmp.getBits());
  }
}