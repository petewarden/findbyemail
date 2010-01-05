
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
#include "vect.h"

#define WINDOW_WIDTH (1024)
#define WINDOW_HEIGHT (768)

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

	void SetupTexture ();
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

void BasicTexQuad::SetupTexture ()
{ 
	bytes = (char *)malloc (pxw * pxh * 4);
	glGenTextures (1, &texID);

	char* currentPixel = bytes;
	for (int y=0; y<pxh; y++)
	{
		for (int x=0; x<pxw; x++)
		{
			if (x&0x7)
			{
				currentPixel[0] = 0; 
				currentPixel[1] = 0; 
				currentPixel[2] = 0; 
				currentPixel[3] = 255;
			}
			else
			{
				currentPixel[0] = 255; 
				currentPixel[1] = 255; 
				currentPixel[2] = 255; 
				currentPixel[3] = 255;			
			}
			
			currentPixel +=4;
		}
	}

	glBindTexture (GL_TEXTURE_2D, texID);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA8, pxw, pxh, 0, 
			GL_RGBA, GL_UNSIGNED_BYTE, bytes);
			
}

void BasicTexQuad::DrawSelf ()
{ 
  glBindTexture(GL_TEXTURE_RECTANGLE_EXT, g_mungData->textureID);
  glEnable(GL_TEXTURE_RECTANGLE_EXT);

  Vect3d up = over.Cross (norm);
  Vect3d north = height * up;
  Vect3d east = width * over;
  Vect3d v = pos - 0.5 * (east + north);
  float left = 0.0;
  float right = FEED_WIDTH;;
  float bottom = FEED_HEIGHT;
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
    FEED_WIDTH, 
    FEED_HEIGHT);
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
display(void)
{
    // Pete - We need to call this periodically to give Quicktime a chance to grab the video data
    SGIdle(g_mungData->seqGrab);

	if (!g_isTextureSetup)
	{
		g_texQuad.SetupTexture();
		g_isTextureSetup = true;
	}
  
	glClear(GL_COLOR_BUFFER_BIT);
  
	const double angle = 0;
	g_texQuad.over = Vect3d(cos(angle), sin(angle), 0);

	g_texQuad.DrawSelf();
  
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
