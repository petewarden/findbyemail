#ifndef INCLUDE_LIVEFEED_H
#define INCLUDE_LIVEFEED_H

#include <QuickTime/QuickTime.h>

#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <OpenGL/glu.h>

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

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void lf_init();

OSErr InitializeMungData(Rect inBounds,MungDataRecord** mungDataPtr);
void MakeSequenceGrabber(MungDataRecord* _mungData);
OSErr MakeSequenceGrabChannel(SeqGrabComponent seqGrab, SGChannel *sgchanVideo, Rect const *rect);
pascal OSErr MungGrabDataProc(SGChannel c, Ptr p, long len, long *offset, long chRefCon, TimeValue time, short writeType, long refCon);

#ifdef __cplusplus
};
#endif // __cplusplus

#define FEED_WIDTH (320)
#define FEED_HEIGHT (240)

#endif // INCLUDE_LIVEFEED_H