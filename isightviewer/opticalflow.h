#ifndef INCLUDE_OPTICAL_FLOW_H
#define INCLUDE_OPTICAL_FLOW_H
/*
 *  opticalflow.h
 *  isightviewer
 *
 *  Created by Pete Warden on 1/5/10.
 *  Copyright 2010 Moveable Code. All rights reserved.
 *
 */

#include "vect.h"
#include "bitmap.h"

struct FlowInputFrame
{
    Bitmap4b _frame;
    float _time;
};

struct OpticalFlow
{
    Bitmap2f _currentFlow;
    
    Bitmap1f _previousFrame;
    float _previousFrameTime;
    
    OpticalFlow();
    ~OpticalFlow();
    
    Bitmap2f& getFlowForFrame(Bitmap4b& inputFrame);
    
};


#endif // INCLUDE_OPTICAL_FLOW_H