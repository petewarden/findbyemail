#ifndef INCLUDE_LIVEFEED_H
#define INCLUDE_LIVEFEED_H

#include <QuickTime/QuickTime.h>

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>

#if __BIG_ENDIAN__
#define EFFECT_UNSIGNED_INT_ARGB_8_8_8_8 GL_UNSIGNED_INT_8_8_8_8_REV
#else
#define EFFECT_UNSIGNED_INT_ARGB_8_8_8_8 GL_UNSIGNED_INT_8_8_8_8
#endif

#include "bitmap.h"

typedef struct {
    Rect 				boundsRect;	// bounds rect
    GWorldPtr 		 	pGWorld;	// offscreen
    SeqGrabComponent 	seqGrab;	// sequence grabber
    ImageSequence 	 	decomSeq;	// unique identifier for our decompression sequence
    ImageSequence 	 	drawSeq;	// unique identifier for our draw sequence
	int					rowBytes;
    long 			 	drawSize;
    TimeValue 		 	lastTime;
    TimeScale 		 	timeScale;
    long 			 	frameCount;
	GLuint				textureID;
} MungDataRecord, *MungDataPtr;

extern MungDataPtr g_mungData;

extern Bitmap4b g_feedImage;

void lf_init();

OSErr InitializeMungData(Rect inBounds,MungDataRecord** mungDataPtr);
void MakeSequenceGrabber(MungDataRecord* _mungData);
OSErr MakeSequenceGrabChannel(SeqGrabComponent seqGrab, SGChannel *sgchanVideo, Rect const *rect);
pascal OSErr MungGrabDataProc(SGChannel c, Ptr p, long len, long *offset, long chRefCon, TimeValue time, short writeType, long refCon);

#endif // INCLUDE_LIVEFEED_H