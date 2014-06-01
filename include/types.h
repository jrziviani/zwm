#ifndef _TYPES_H
#define _TYPES_H

//
// INCLUDES
//
#include "log.h"


//
// TYPEDEFS
//
typedef unsigned long ulong;
typedef ulong keysym; 
typedef ulong whandler;


// STRUCTS
struct size
{
    int width;
    int height;
};

struct position
{
    int x;
    int y;
};

#endif
