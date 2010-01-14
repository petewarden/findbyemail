/*
 *  opticalflow.cpp
 *  isightviewer
 *
 *  Created by Pete Warden on 1/5/10.
 *  Copyright 2010 Moveable Code. All rights reserved.
 *
 */

#include "opticalflow.h"

#include "bitmapops.h"

const int blurRadius = 12;
const int gradientOffset = 8;

// Takes an 8 bit RGBA image and returns a one-channel bitmap containing an approximation
// of the luminance for each pixel as a float value.
static void luminanceMapFromFrame(Bitmap4b& frame, Bitmap1f* result)
{
    const int width = frame._width;
    const int height = frame._height;
    
    result->resizeData(width, height);

    int y;
    for (y=0; y<height; y+=1)
    {
        int x;
        for (x=0; x<width; x+=1)
        {
            Vect4b* framePixel = (Vect4b*)(frame.pixelAt(x, y));
            float* resultPixel = (float*)(result->pixelAt(x, y));
            
            *resultPixel = ((float)(framePixel->y)/255.0);
        }
    }
}

// Takes a difference image and calculates the x/y derivatives for each pixel as a
// two-channel float bitmap
static void gradientFromDifference(Bitmap1f& difference, Bitmap2f* gradient)
{
    const int width = difference._width;
    const int height = difference._height;
    
    Bitmap1f xGradient;
    Bitmap1f yGradient;
    xGradient.resizeData(width, height);
    yGradient.resizeData(width, height);
        
    subtractImages(difference, difference, &xGradient, Vect2i(gradientOffset, 0));
    subtractImages(difference, difference, &yGradient, Vect2i(0, gradientOffset));
    
    gradient->resizeData(width, height);

    for (int y=0; y<height; y+=1)
    {
        for (int x=0; x<width; x+=1)
        {
            float* xPixel = xGradient.pixelAt(x, y);
            float* yPixel = yGradient.pixelAt(x, y);
            float* outPixel = gradient->pixelAt(x, y);
            
            outPixel[0] = *xPixel;
            outPixel[1] = *yPixel;
        }
    }
    
}

OpticalFlow::OpticalFlow()
{
    // Do nothing
}

OpticalFlow::~OpticalFlow()
{
    // Do nothing
}

Bitmap2f& OpticalFlow::getFlowForFrame(Bitmap4b& inputFrame)
{
    Bitmap1f luminanceMap;
    luminanceMapFromFrame(inputFrame, &luminanceMap);
    
    Bitmap1f blurredLuminance;
    blurImage(luminanceMap, blurRadius, &blurredLuminance);
    
    Bitmap1f difference;
    subtractImages(blurredLuminance, _previousFrame, &difference);
 
    _previousFrame = blurredLuminance;

    Bitmap2f rawGradient;
    gradientFromDifference(difference, &rawGradient);
    
    _currentFlow = rawGradient;
    
    return _currentFlow;
}