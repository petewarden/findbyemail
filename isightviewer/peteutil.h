#ifndef INCLUDE_PETEUTIL_H
#define INCLUDE_PETEUTIL_H

/*
 *  peteutil.h
 *  isightviewer
 *
 *  Created by Pete Warden on 1/5/10.
 *  Copyright 2010 Moveable Code. All rights reserved.
 *
 */

template <class T> inline T gate(T input, T min, T max)
{
    if (input<min)
        return min;
    else if (input>max)
        return max;
    else
        return input;
}

#endif // INCLUDE_PETEUTIL_H