#include "vect.h"

#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>

#ifdef __APPLE__
#include <Glut/glut.h>
#include <OpenGL/glext.h>
#else // Not OS X, so assume linux
#include <GL/glut.h>
#include <GL/glext.h>
#endif // __APPLE__

#include "livefeed.h"
#include "bitmap.h"
#include "bitmapops.h"
#include "opticalflow.h"

#define WINDOW_WIDTH (1024)
#define WINDOW_HEIGHT (768)

OpticalFlow g_opticalFlow;

double getSeconds()
{
	struct timeval timeValue;
	struct timezone timeZone;
	gettimeofday(&timeValue, &timeZone);
	return timeValue.tv_sec+(((double)timeValue.tv_usec)/1000000.0);
}

class BasicTexQuad
{
public:
	BasicTexQuad (Vect3d _pos, Vect3d _over, Vect3d _norm, double _width, double _height);	

	void SetTexture(Bitmap4b& bitmap);
	void DrawSelf ();
	
public:
	Vect3d pos;
	Vect3d over;
	Vect3d norm;
	double width;
	double height;

	char* bytes;
	GLuint texID;
	int pxw;
	int pxh;
};

void printLog(GLuint obj)
{
    int infologLength = 0;
    char infoLog[1024];
 
	if (glIsShader(obj))
		glGetShaderInfoLog(obj, 1024, &infologLength, infoLog);
	else
		glGetProgramInfoLog(obj, 1024, &infologLength, infoLog);
 
    if (infologLength > 0)
		printf("%s\n", infoLog);
}

#define logGLErrors()											\
do {                                                            \
    GLenum err = glGetError();                                  \
    if (err==GL_NO_ERROR)                                       \
		break;                                                  \
	                                                            \
	fprintf(stderr, "GL Error: %s at %s:%d", gluErrorString(err), __FILE__, __LINE__);       \
	exit(0);                                                    \
} while (false)

BasicTexQuad::BasicTexQuad (Vect3d _pos, Vect3d _over, Vect3d _norm,
                            double _width, double _height)
{ pos = _pos;
  over = _over;
  norm = _norm;
  width = _width;
  height = _height;

  bytes = NULL;
  texID = 0;
  pxw = 100;
  pxh = 100;
}

void BasicTexQuad::SetTexture(Bitmap4b& bitmap)
{
    width = bitmap._width;
    height = bitmap._height;

    if (texID!=0)
        glDeleteTextures(1, &texID);
        
	glGenTextures (1, &texID);

	glBindTexture (GL_TEXTURE_RECTANGLE_EXT, texID);
    glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA8, (bitmap._rowBytes/4), height,
        0, GL_BGRA_EXT, EFFECT_UNSIGNED_INT_ARGB_8_8_8_8, bitmap._pixelData);
}

void BasicTexQuad::DrawSelf ()
{ 
  glBindTexture(GL_TEXTURE_RECTANGLE_EXT, texID);
  glEnable(GL_TEXTURE_RECTANGLE_EXT);

  Vect3d up = over.cross (norm);
  Vect3d north = height * up;
  Vect3d east = width * over;
  Vect3d v = pos - 0.5 * (east + north);
  float left = 0.0;
  float right = width;
  float bottom = height;
  float top = 0;

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  glBegin(GL_QUADS);
  glNormal3d (norm.x, norm.y, norm.z);
  glTexCoord2f (left, bottom);     glVertex3f (v.x, v.y, v.z);  v += east;
  glTexCoord2f (right, bottom);     glVertex3f (v.x, v.y, v.z);  v += north;
  glTexCoord2f (right, top);        glVertex3f (v.x, v.y, v.z);  v -= east;
  glTexCoord2f (left, top);        glVertex3f (v.x, v.y, v.z);  v -= north;
  glEnd();
}

BasicTexQuad g_texQuad(
    Vect3d((WINDOW_WIDTH/2), (WINDOW_HEIGHT/2),0), 
    Vect3d(0,1,0), 
    Vect3d(0,0,1), 
    1, 
    1);
bool g_isTextureSetup = false;

void
reshape(int w, int h)
{
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1, 1);
  glScalef(1, -1, 1);
  glTranslatef(0, -h, 0);
}

void
drawFlowVectors(Bitmap2f& flow, Vect3d pos, Vect3d size)
{
    const float pointSpacing = 32.0f;
    const float lineScale = 256.0f;
    const float arrowWidth = 1.0f;
    const float arrowLength = 4.0f;
    const float minimumForDisplay = 0.0001f;
    
    const int flowWidth = flow._width;
    const int flowHeight = flow._height;

    glDisable(GL_TEXTURE_RECTANGLE_EXT);
    glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (float y=0.0f; y<size.y; y+= pointSpacing)
    {
        const int flowSampleY = (int)((y/size.y)*flowHeight);
        
        for (float x=0.0f; x<size.x; x+= pointSpacing)
        {
            const int flowSampleX = (int)((x/size.x)*flowWidth);
            
            Vect2f flowValues = *(Vect2f*)(flow.pixelAt(flowSampleX, flowSampleY));
        
            if (flowValues.lengthSquared()<minimumForDisplay)
                continue;
        
            Vect2f lineOrigin(pos.x+x, pos.y+y);
            Vect2f lineDirection = (flowValues*lineScale);
            Vect2f lineTip = (lineOrigin+lineDirection);
        
            Vect2f arrowBase = (lineTip-(flowValues.normalized()*arrowLength));
            Vect2f arrowOffset = (flowValues.cross().normalized()*arrowWidth);
            Vect2f arrowLeft = (arrowBase-arrowOffset);
            Vect2f arrowRight = (arrowBase+arrowOffset);

            glBegin(GL_LINE_STRIP);
            
            glVertex2f(lineOrigin.x, lineOrigin.y);
            glVertex2f(lineTip.x, lineTip.y);
            glVertex2f(arrowLeft.x, arrowLeft.y);
            glVertex2f(arrowRight.x, arrowRight.y);
            glVertex2f(lineTip.x, lineTip.y);
            
            glEnd();
        }
    }
}

void
display(void)
{
    // Pete - We need to call this periodically to give Quicktime a chance to grab the video data
    SGIdle(g_mungData->seqGrab);

    Bitmap2f& flow = g_opticalFlow.getFlowForFrame(g_feedImage);

    const bool showFlowTexture = false;
    if (showFlowTexture)
    {
        Bitmap4b flowTexture;
        convertToARGB8(flow, &flowTexture);

        g_texQuad.SetTexture(flowTexture);
    }
    else
    {
        g_texQuad.SetTexture(g_feedImage);    
    }
  
	glClear(GL_COLOR_BUFFER_BIT);
  
	const double angle = 0;
	g_texQuad.over = Vect3d(cos(angle), sin(angle), 0);

	g_texQuad.DrawSelf();
    
    Vect3d quadSize(g_texQuad.width, g_texQuad.height, 0);
    Vect3d halfQuadSize = (quadSize*0.5);
    Vect3d quadTopLeft = (g_texQuad.pos-halfQuadSize);
    
    drawFlowVectors(flow, quadTopLeft, quadSize);
  
	glutSwapBuffers();

	glutPostRedisplay();
}

extern "C" int
main(int argc, char **argv)
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
  glutCreateWindow("Video capture example");
  glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  
  lf_init();
  
  glutMainLoop();
  return 0; 
}
