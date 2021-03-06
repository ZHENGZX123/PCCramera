

//    WAVE_OUT.H - Necessary stuff for WIN_AUDIO

#ifndef WAVE_OUT_H
#define WAVE_OUT_H

#include <stdio.h>
#include <windows.h>
#ifdef __MINGW32__
#include <mmsystem.h>
#endif

#define VERSION_STRING "\n 0.7.0\n"

#define Cdecl               __cdecl
#define __attribute__(x)
#define sleep(__sec)        Sleep ((__sec) * 1000)
#define inline              __inline
#define restrict

//// constants /////////////////////////////////////////////////////

#define CD_SAMPLE_FREQ         44.1e3
#define SAMPLE_SIZE            16
#define SAMPLE_SIZE_STRING     ""
#define WINAUDIO_FD            ((FILE_T)-128)
#define FILE_T                 FILE*
#define INVALID_FILEDESC       NULL

//// Simple types //////////////////////////////////////////////////

typedef signed   int        Int;        // at least -32767...+32767, fast type
typedef unsigned int        Uint;       // at least 0...65535, fast type
typedef long double         Ldouble;    // most exact floating point format

//// procedures/functions //////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

Int        Set_WIN_Params             ( FILE_T dummyFile , Ldouble SampleFreq, Uint BitsPerSample, Uint Channels);
int        WIN_Play_Samples           ( const void* buff, size_t len );
int		   WIN_Audio_reset ( void );
int        WIN_Audio_close            ( void );

#ifdef __cplusplus
}
#endif

#endif // WAVE_OUT_H
