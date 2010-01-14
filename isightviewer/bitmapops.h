#ifndef INCLUDE_BITMAPOPS_H
#define INCLUDE_BITMAPOPS_H
/*
 *  bitmap.h
 *  isightviewer
 *
 *  Created by Pete Warden on 1/8/10.
 *  Copyright 2010 Moveable Code. All rights reserved.
 *
 */

#include "bitmap.h"
#include <stdint.h>

// blurImage() implements the 'box car' algorithm to repeatedly apply a box
// filter to the input image in x and y passes, to end up with an approximate
// gaussian filter. The main advantage of the box car algorithm is that it
// only requires two pixel reads per output pixel, no matter how large the
// radius is, making it approximately O(n) on the number of pixels processed.
//
// This version only accepts an integer radius, and uses float intermediates.
// Image edges are treated as if the edge pixels extended infinitely outside
// the borders, so black doesn't creep in at the edges.
//
template <class T, int channels> void blurImage(Bitmap<T, channels>& input, int radius, Bitmap<T, channels>* output)
{
    const int width = input._width;
    const int height = input._height;

    Bitmap<T, channels> buffers[2];
    buffers[0].resizeData(width, height);
    buffers[1].resizeData(width, height);
    
    output->resizeData(width, height);
    
    const int passCount = 3;
    
    const int doubleRadius = (radius*2);
    
    for (int pass=0; pass<passCount; pass+=1)
    {
        const bool isFirstPass = (pass==0);
        const bool isLastPass = (pass==(passCount-1));
    
        Bitmap<T, channels>* inBuffer;
        if (isFirstPass)
            inBuffer = &input;
        else
            inBuffer = &buffers[0];

        Bitmap<T, channels>* outBuffer;
        outBuffer = &buffers[1];
    
        for (int y=0; y<height; y+=1)
        {
            T* firstPixel = inBuffer->pixelAt(0, y);
        
            float accumulated[channels];
            for (int c=0; c<channels; c+=1)
                accumulated[c] = (firstPixel[c]*doubleRadius);

            for (int x=-radius; x<width; x+=1)
            {
                int leftX = (x-radius);
                int rightX = (x+radius);
                
                T* leftPixel = inBuffer->pixelAt(leftX, y);
                T* rightPixel = inBuffer->pixelAt(rightX, y);
    
                for (int c=0; c<channels; c+=1)
                {
                    accumulated[c] -= leftPixel[c];
                    accumulated[c] += rightPixel[c];
                }
                
                if (x>=0)
                {
                    T* outPixel = outBuffer->pixelAt(x, y);
                    for (int c=0; c<channels; c+=1)
                        outPixel[c] = (T)(accumulated[c]/doubleRadius);
                }
            }
        }

        inBuffer = &buffers[1];

        if (isLastPass)
            outBuffer = output;
        else
            outBuffer = &buffers[0];

        for (int x=0; x<width; x+=1)
        {
            T* firstPixel = inBuffer->pixelAt(x, 0);
        
            float accumulated[channels];
            for (int c=0; c<channels; c+=1)
                accumulated[c] = (firstPixel[c]*doubleRadius);

            for (int y=-radius; y<height; y+=1)
            {
                int topY = (y-radius);
                int bottomY = (y+radius);
                
                T* topPixel = inBuffer->pixelAt(x, topY);
                T* bottomPixel = inBuffer->pixelAt(x, bottomY);
    
                for (int c=0; c<channels; c+=1)
                {
                    accumulated[c] -= topPixel[c];
                    accumulated[c] += bottomPixel[c];
                }
                
                if (y>=0)
                {
                    T* outPixel = outBuffer->pixelAt(x, y);
                    for (int c=0; c<channels; c+=1)
                        outPixel[c] = (T)(accumulated[c]/doubleRadius);
                }
            }
        }
    
    }

}

// subtractImages() returns an output image with every pixel containing the result of the 
// corresponding pixel in b subtracted from the matching pixel in a.
//
// There's an optional offset parameter. If this is set, then the b image will be shifted
// by that number of pixels before the subtraction is performed.
// The size of the first 'a' image is used for the output size.
// If the result of the subtraction is outside the range of the type, it
// will not be gated, so you'll end up with overflow for chars especially 
//
template <class T, int channels> void subtractImages(Bitmap<T, channels>& a, Bitmap<T, channels>& b, Bitmap<T, channels>* output, Vect2i offset=Vect2i(0,0))
{
    const int width = a._width;
    const int height = a._height;

    output->resizeData(width, height);
    
    for (int y=0; y<height; y+=1)
    {
        for (int x=0; x<width; x+=1)
        {
            T* aPixel = a.pixelAt(x, y);
            T* bPixel = b.pixelAt((x+offset.x), (y+offset.y));
            T* outPixel = output->pixelAt(x, y);
        
            for (int c=0; c<channels; c+=1)
                outPixel[c] = (T)(aPixel[c]-bPixel[c]);
        }
        
    }
}

// Converts the image into an 8 bit RGBA form, useful for displaying as a texture
template <class T, int channels> void convertToARGB8(Bitmap<T, channels>& input, Bitmap4b* output, Vect4b defaultsVector=Vect4b(255,127,127,127))
{
    const int width = input._width;
    const int height = input._height;

    output->resizeData(width, height);

    uint8_t* defaults = (uint8_t*)(&defaultsVector);

    for (int y=0; y<height; y+=1)
    {
        for (int x=0; x<width; x+=1)
        {
            T* inPixel = input.pixelAt(x, y);
            uint8_t* outPixel = output->pixelAt(x, y);
        
            outPixel[0] = defaults[0];

            int c;
            for (c=1; c<(channels+1); c+=1)
                outPixel[c] = (uint8_t)((inPixel[c-1]*127.0)+127.0);

            for (; c<4; c+=1)
                outPixel[c] = defaults[c];
        }
        
    }
}

#endif // INCLUDE_BITMAPOPS_H