#ifndef COMMON_H
#define COMMON_H

//  Standard C Libaries
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <malloc.h>
#include <memory>
#include <stdexcept>
#include <inttypes.h>

//  Unix System
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

//  Standard Template Library (STL)
#include <string>
#include <cstring>
#include <vector>
#include <deque>
#include <map>
#include <algorithm>

//  Mathematics
#include <math.h>

//  OpenGL
//#include <GL/glew.h>
//#include <GL/glx.h>    /* this includes the necessary X headers */
//#include <GL/gl.h>
//#include <GL/glext.h>

//  DevIL Img Lib
#include <IL/il.h>
#include <IL/ilu.h>

//  MRF Graph Cut
#include <MRF/mrf.h>
#include <MRF/energy.h>
#include <MRF/GCoptimization.h>

//  X System
/*
 *  X86 compilation: gcc -o -L/usr/X11/lib   main main.c -lGL -lX11
 *  X64 compilation: gcc -o -L/usr/X11/lib64 main main.c -lGL -lX11
 */
#include <X11/X.h>    /* X11 constant (e.g. TrueColor) */
#include <X11/keysym.h>
#include <X11/Xlib.h>

//  Optimization
#include <omp.h>
#include <xmmintrin.h>  //  SSE
#include <emmintrin.h>  //  SSE2s
//  General Headers
#include "Terms.h"
#include "Tools.h"
//#include "System/XWindow.h"

#endif
