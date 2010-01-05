#include "livefeed.h"

#if __BIG_ENDIAN__
#define EFFECT_UNSIGNED_INT_ARGB_8_8_8_8 GL_UNSIGNED_INT_8_8_8_8_REV
#else
#define EFFECT_UNSIGNED_INT_ARGB_8_8_8_8 GL_UNSIGNED_INT_8_8_8_8
#endif

#define BailErr(x) {err = x; if(err != noErr) fprintf(stderr,"Error '%d' :%s, %d\n",(int)(err),__FILE__,__LINE__);}

MungDataPtr g_mungData = NULL;

void lf_init()
{
    const int width = FEED_WIDTH;
    const int height = FEED_HEIGHT;
    OSErr err;

	if (g_mungData==NULL)
	{
        EnterMovies();

		GrafPtr safetyPort;
		safetyPort = CreateNewPort();
		SetPort( safetyPort );
        
		Rect portRect = { 0, 0, height, width };
		
		// initialize our data
		err = InitializeMungData(portRect, &g_mungData);
		BailErr(err);
		
		// create and initialize the sequence grabber
		MakeSequenceGrabber(g_mungData);
		BailErr(NULL == g_mungData->seqGrab);
		
		// create the channel
		SGChannel sgchanVideo;
		err = MakeSequenceGrabChannel(g_mungData->seqGrab, &sgchanVideo, &portRect);
		BailErr(err);
		
		// specify a data function
		err = SGSetDataProc(g_mungData->seqGrab, NewSGDataUPP(MungGrabDataProc), (long int)(g_mungData));
		BailErr(err);
		
		// lights...camera...
		err = SGPrepare(g_mungData->seqGrab, false, true);
		BailErr(err); 
		
		// ...action
		err = SGStartRecord(g_mungData->seqGrab);
		BailErr(err);

        fprintf(stderr, "Recording started\n");

		glGenTextures(1, &g_mungData->textureID);
		glBindTexture(GL_TEXTURE_RECTANGLE_EXT, g_mungData->textureID);
		glTexImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, GL_RGBA8, (g_mungData->rowBytes/4), height,
			0, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, NULL);
	}
}

// --------------------
// InitializeMungData
//
OSErr InitializeMungData(Rect inBounds,MungDataRecord** mungDataPtr)
{
    CGrafPtr theOldPort;
    GDHandle theOldDevice;
    
    OSErr err = noErr;
    
    // allocate memory for the data
    *mungDataPtr = (MungDataPtr)NewPtrClear(sizeof(MungDataRecord));
    MungDataRecord* _mungData = *mungDataPtr;
	
	if (MemError() || NULL == _mungData ) return -23;
    
    // create a GWorld
    err = QTNewGWorld(&(_mungData->pGWorld),	// returned GWorld
    					k32ARGBPixelFormat,		// pixel format
    					&inBounds,				// bounds
    					0,						// color table
    					NULL,					// GDHandle
    					0);						// flags
	BailErr(err);
    
    // lock the pixmap and make sure it's locked because
    // we can't decompress into an unlocked pixmap
    if(!LockPixels(GetGWorldPixMap(_mungData->pGWorld)))
	{
		BailErr(-23);
		goto bail;
	}
    
    GetGWorld(&theOldPort, &theOldDevice);    
    SetGWorld(_mungData->pGWorld, NULL);
    BackColor(blackColor);
    ForeColor(whiteColor);
    EraseRect(&inBounds);    
    SetGWorld(theOldPort, theOldDevice);

	_mungData->boundsRect = inBounds;

    {
		ImageDescriptionHandle desc = NULL;
		PixMapHandle hPixMap = GetGWorldPixMap(_mungData->pGWorld);
		Rect bounds;
		
		GetPixBounds(hPixMap, &bounds);

		err = MakeImageDescriptionForPixMap(hPixMap, &desc);
		BailErr(err);
		
		_mungData->rowBytes = GetPixRowBytes(hPixMap);
		
		_mungData->drawSize = (GetPixRowBytes(hPixMap) * (*desc)->height);

		if (desc)
			DisposeHandle((Handle)desc);
	}

bail:
	return err;
}

pascal OSErr MungGrabDataProc(SGChannel c, Ptr p, long len, long *offset, long chRefCon, TimeValue time, short writeType, long refCon)
{
	MungDataRecord* _mungData = (MungDataRecord*)(refCon);

    CGrafPtr	theSavedPort;
    GDHandle    theSavedDevice;
    CodecFlags	ignore;
    
    ComponentResult err = noErr;

    GetGWorld(&theSavedPort, &theSavedDevice);    
    SetGWorld(_mungData->pGWorld, NULL);
	
	// reset frame and time counters after a stop/start
	if (_mungData->lastTime > time) {
		_mungData->lastTime = 0;
		_mungData->frameCount = 0;
	}
    
    _mungData->frameCount++;
        
    if (_mungData->timeScale == 0) {
    	// first time here so set the time scale
    	err = SGGetChannelTimeScale(c, &_mungData->timeScale);
    	BailErr(err);
    }
    
	if (_mungData->pGWorld) {
    	if (_mungData->decomSeq == 0) {
    		// Set up getting grabbed data into the GWorld
    		
    		Rect				   sourceRect = { 0, 0 };
			MatrixRecord		   scaleMatrix;
			ImageDescriptionHandle imageDesc = (ImageDescriptionHandle)NewHandle(0);
            
            // retrieve a channel’s current sample description, the channel returns a sample description that is
            // appropriate to the type of data being captured
            err = SGGetChannelSampleDescription(c, (Handle)imageDesc);
            BailErr(err);
                        
            // make a scaling matrix for the sequence
			sourceRect.right = (**imageDesc).width;
			sourceRect.bottom = (**imageDesc).height;
			RectMatrix(&scaleMatrix, &sourceRect, &_mungData->boundsRect);
			
            // begin the process of decompressing a sequence of frames
            // this is a set-up call and is only called once for the sequence - the ICM will interrogate different codecs
            // and construct a suitable decompression chain, as this is a time consuming process we don't want to do this
            // once per frame (eg. by using DecompressImage)
            // for more information see Ice Floe #8 http://developer.apple.com/quicktime/icefloe/dispatch008.html
            // the destination is specified as the GWorld
			err = DecompressSequenceBegin(&_mungData->decomSeq,	// pointer to field to receive unique ID for sequence
										  imageDesc,			// handle to image description structure
										  _mungData->pGWorld,   // port for the DESTINATION image
										  NULL,					// graphics device handle, if port is set, set to NULL
										  NULL,					// source rectangle defining the portion of the image to decompress 
                                          &scaleMatrix,			// transformation matrix
                                          srcCopy,				// transfer mode specifier
                                          (RgnHandle)NULL,		// clipping region in dest. coordinate system to use as a mask
                                          0,					// flags
                                          codecNormalQuality, 	// accuracy in decompression
                                          bestSpeedCodec);		// compressor identifier or special identifiers ie. bestSpeedCodec
            BailErr(err);
            
            DisposeHandle((Handle)imageDesc);         
            
        }
        
        // decompress a frame into the GWorld - can queue a frame for async decompression when passed in a completion proc
        err = DecompressSequenceFrameS(_mungData->decomSeq,	// sequence ID returned by DecompressSequenceBegin
        							   p,					// pointer to compressed image data
        							   len,					// size of the buffer
        							   0,					// in flags
        							   &ignore,				// out flags
        							   NULL);				// async completion proc

		if (err) {
            fprintf(stderr,"DecompressSequenceFrameS gave error %ld (%lx)",err,err);
            err = noErr;
		} else {	
//		   GetPixBaseAddr(GetGWorldPixMap(gMungData->pGWorld)),	// pointer image data
//		   gMungData->drawSize,									// size of the buffer
			
			char* videoData = GetPixBaseAddr(GetGWorldPixMap(_mungData->pGWorld));
//			const int videoDataSize = _mungData->drawSize;
//			fprintf(stderr,"0x%x:%dk\n",videoData,videoDataSize);

			glBindTexture(GL_TEXTURE_RECTANGLE_EXT, _mungData->textureID);
			glTexSubImage2D(GL_TEXTURE_RECTANGLE_EXT, 0, 0, 0, 
				(_mungData->rowBytes/4), _mungData->boundsRect.bottom, 
				GL_BGRA_EXT, EFFECT_UNSIGNED_INT_ARGB_8_8_8_8, videoData);

		}
	}
            
    _mungData->lastTime = time;

    SetGWorld(theSavedPort, theSavedDevice);
	
	return err;
}

// --------------------
// MakeSequenceGrabber
//
void MakeSequenceGrabber(MungDataRecord* _mungData)
{
	OSErr			 err = noErr;

    // open the default sequence grabber
    _mungData->seqGrab = OpenDefaultComponent(SeqGrabComponentType, 0);
    if (_mungData->seqGrab != NULL) { 
    	// initialize the default sequence grabber component
    	err = SGInitialize(_mungData->seqGrab);

    	if (err == noErr)
        	// set its graphics world to the specified window
        	err = SGSetGWorld(_mungData->seqGrab, _mungData->pGWorld, NULL );
    	
    	if (err == noErr)
    		// specify the destination data reference for a record operation
    		// tell it we're not making a movie
    		// if the flag seqGrabDontMakeMovie is used, the sequence grabber still calls
    		// your data function, but does not write any data to the movie file
    		// writeType will always be set to seqGrabWriteAppend
    		err = SGSetDataRef(_mungData->seqGrab,
    						   0,
    						   0,
    						   seqGrabDontMakeMovie);
    }

    if (err && (_mungData->seqGrab != NULL)) { // clean up on failure
        fprintf(stderr, "Error %d in MakeSequenceGrabber", err);
    	CloseComponent(_mungData->seqGrab);
        _mungData->seqGrab = NULL;
    }
}

// --------------------
// MakeSequenceGrabChannel
//
OSErr MakeSequenceGrabChannel(SeqGrabComponent seqGrab, SGChannel *sgchanVideo, Rect const *rect)
{
    long  flags = 0;
    
    OSErr err = noErr;
    
    err = SGNewChannel(seqGrab, VideoMediaType, sgchanVideo);
	BailErr(err);
    if (err == noErr) {
	    err = SGSetChannelBounds(*sgchanVideo, rect);
		BailErr(err);
	    if (err == noErr)
	    	// set usage for new video channel to avoid playthrough
	   		// note we don't set seqGrabPlayDuringRecord
	    	err = SGSetChannelUsage(*sgchanVideo, flags | seqGrabRecord );
	    
	    if (err != noErr) {
	        // clean up on failure
	        SGDisposeChannel(seqGrab, *sgchanVideo);
	        *sgchanVideo = NULL;
            fprintf(stderr, "Error %d in MakeSequenceGrabChannel", err);
	    }
    }

	return err;
}
