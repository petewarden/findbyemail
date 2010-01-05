
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

struct Vect
{
	double x;
	double y;
	double z;

	Vect() : x(0.0), y(0.0), z(0.0) { }
	Vect(double _x, double _y, double _z) : x(_x), y(_y), z(_z) { }
	Vect(const Vect& other) : x(other.x), y(other.y), z(other.z) { }

	const Vect& operator*=(double factor) { x*=factor; y*=factor; z*=factor; return *this; }
	const Vect& operator+=(const Vect& other) { x+=other.x; y+=other.y; z+=other.z; return *this; }
	const Vect& operator-=(const Vect& other) { x-=other.x; y-=other.y; z-=other.z; return *this; }

	Vect operator*(double factor) { return Vect(x*factor, y*factor, z*factor); }
	Vect operator+(const Vect& other) { return Vect(x+other.x, y+other.y, z+other.z); }
	Vect operator-(const Vect& other) { return Vect(x-other.x, y-other.y, z-other.z); }
	
	Vect Cross(const Vect& other) { return Vect((y*other.z)-(z*other.y), 
		(z*other.x)-(x*other.z), (x*other.y)-(y*other.x)); }
		
	double dot(const Vect& other) { return (x*other.x)+(y*other.y)+(z*other.z); }
};

inline Vect operator*(double factor, const Vect& other) { 
	return Vect(factor*other.x, factor*other.y, factor*other.z); }

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
	BasicTexQuad (Vect _pos, Vect _over, Vect _norm, double _width, double _height);	

	void SetupTexture ();
	void DrawSelf ();
	
public:
	Vect pos;
	Vect over;
	Vect norm;
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

BasicTexQuad::BasicTexQuad (Vect _pos, Vect _over, Vect _norm,
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
//  glEnable (GL_TEXTURE_2D);
//  if (!texID)
//    SetupTexture ();
//  else
//    glBindTexture (GL_TEXTURE_2D, texID);

glBindTexture(GL_TEXTURE_RECTANGLE_EXT, g_mungData->textureID);
glEnable(GL_TEXTURE_RECTANGLE_EXT);

  Vect up = over.Cross (norm);
  Vect north = height * up;
  Vect east = width * over;
  Vect v = pos - 0.5 * (east + north);
  float left = 0.0;
  float right = FEED_HEIGHT;//1.0;
  float bottom = 0.0;
  float top = FEED_HEIGHT;//1.0;

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

BasicTexQuad g_texQuad(Vect(100,100,0), Vect(0,1,0), Vect(0,0,1), 100, 100);
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
	g_texQuad.over = Vect(cos(angle), sin(angle), 0);

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
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  
  lf_init();
  
  glutMainLoop();
  return 0; 
}
