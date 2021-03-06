
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

/* Set TABS = 4 */
/********************************************************************

 function: To provide playback of 16 bit PCM wave data in Win32
           environments from decoded compressed files.

 ********************************************************************/

#if defined WIN32DLL || defined _WIN32

#include <string.h>
#include <errno.h>
#include "wave_out.h"

#define MAXWAVESIZE     4294967040LU
#define MAX_WAVEBLOCKS  32

// This is modified for USE_WIN_AUDIO - ONLY 2002-02-27


static CRITICAL_SECTION  g_cs;
static HWAVEOUT          g_dev                    = NULL;
static int               g_ScheduledBlocks        = 0;
static int               g_PlayedWaveHeadersCount = 0;          // free index
static WAVEHDR*          g_pPlayedWaveHeaders [MAX_WAVEBLOCKS];

static int
Box ( const char* msg )
{
	MessageBox ( NULL, msg, " "VERSION_STRING": Error Message . . .", MB_OK | MB_ICONEXCLAMATION );
	return -1;
}

//  This function registers already played WAVE chunks. Freeing is done by free_memory()
static void CALLBACK
wave_callback ( HWAVE hWave, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2 )
{
	if(uMsg == WOM_DONE) {
		EnterCriticalSection(&g_cs);
			if(g_PlayedWaveHeadersCount>=MAX_WAVEBLOCKS) {
				char a[256]={0};				
				sprintf(a, "===wave_out, g_PlayedWaveHeadersCount>=MAX_WAVEBLOCKS===\n");
				OutputDebugString(a);

			}else{
				g_pPlayedWaveHeaders[g_PlayedWaveHeadersCount] = (WAVEHDR*) dwParam1;
				g_PlayedWaveHeadersCount++;
			}
		LeaveCriticalSection(&g_cs);
	}
}


Int
Set_WIN_Params ( FILE_T   dummyFile ,
                 Ldouble  SampleFreq,
                 Uint     BitsPerSample,
                 Uint     Channels )
{
	WAVEFORMATEX  outFormat;
	UINT          deviceID = WAVE_MAPPER;

	(void) dummyFile;

	if(waveOutGetNumDevs () == 0) return Box ( "No audio device present." );

	outFormat.wFormatTag      = WAVE_FORMAT_PCM;
	outFormat.wBitsPerSample  = BitsPerSample;
	outFormat.nChannels       = Channels;
	outFormat.nSamplesPerSec  = (unsigned long)(SampleFreq + 0.5);
	outFormat.nBlockAlign     = (outFormat.wBitsPerSample + 7) / 8 * outFormat.nChannels;
	outFormat.nAvgBytesPerSec = outFormat.nSamplesPerSec * outFormat.nBlockAlign;

	switch(waveOutOpen(&g_dev, deviceID, &outFormat, (DWORD)wave_callback, 0, CALLBACK_FUNCTION))
	{
		case MMSYSERR_ALLOCATED:   return Box ( "Device is already open." );
		case MMSYSERR_BADDEVICEID: return Box ( "The specified device is out of range." );
		case MMSYSERR_NODRIVER:    return Box ( "There is no audio driver in this system." );
		case MMSYSERR_NOMEM:       return Box ( "Unable to allocate sound memory." );
		case WAVERR_BADFORMAT:     return Box ( "This audio format is not supported." );
		case WAVERR_SYNC:          return Box ( "The device is synchronous." );
		default:                   return Box ( "Unknown media error." );
		case MMSYSERR_NOERROR:     break;
	}

	waveOutReset ( g_dev );
	InitializeCriticalSection ( &g_cs );
	SetPriorityClass(GetCurrentProcess (), HIGH_PRIORITY_CLASS);
	return 0;
}



static void
free_memory ( void )
{
	WAVEHDR*  wh=NULL;
	HGLOBAL   hg=NULL;
  EnterCriticalSection( &g_cs );
	if(g_PlayedWaveHeadersCount<=0) {
			char a[256]={0};
			sprintf(a, "===wave_out, free_memory()===\n");
			OutputDebugString(a);
		}else{
			wh = g_pPlayedWaveHeaders[--g_PlayedWaveHeadersCount];
			g_ScheduledBlocks--;                               // decrease the number of USED blocks
		}
  LeaveCriticalSection ( &g_cs );

	waveOutUnprepareHeader ( g_dev, wh, sizeof (WAVEHDR) );

	hg = GlobalHandle ( wh -> lpData );       // Deallocate the buffer memory
	GlobalUnlock (hg);
	GlobalFree   (hg);

	hg = GlobalHandle ( wh );                 // Deallocate the header memory
	GlobalUnlock (hg);
	GlobalFree   (hg);
}

int
WIN_Play_Samples ( const void* data, size_t len )
{
	HGLOBAL    hg;
	HGLOBAL    hg2;
	LPWAVEHDR  wh;
	void*      allocptr;

	do {
		while(g_PlayedWaveHeadersCount > 0 ) free_memory();		// free used blocks ...
		if(g_ScheduledBlocks < MAX_WAVEBLOCKS) break;			// wait for a free block ...
		Sleep(10);
	}while(1);

	if ( (hg2 = GlobalAlloc ( GMEM_MOVEABLE, len )) == NULL )   // allocate some memory for a copy of the buffer
		return Box ( "GlobalAlloc failed." );

	allocptr = GlobalLock (hg2);
	CopyMemory ( allocptr, data, len );                         // Here we can call any modification output functions we want....

	if ( (hg = GlobalAlloc (GMEM_MOVEABLE | GMEM_ZEROINIT, sizeof (WAVEHDR))) == NULL ) // now make a header and WRITE IT!
		return -1;

	wh                   = GlobalLock (hg);
	wh -> dwBufferLength = len;
	wh -> lpData         = allocptr;

	if ( waveOutPrepareHeader ( g_dev, wh, sizeof (WAVEHDR)) != MMSYSERR_NOERROR ) {
		GlobalUnlock (hg);
		GlobalFree   (hg);
		return -1;
	}

	if ( waveOutWrite ( g_dev, wh, sizeof (WAVEHDR)) != MMSYSERR_NOERROR ) {
		GlobalUnlock (hg);
		GlobalFree   (hg);
		return -1;
	}

	EnterCriticalSection ( &g_cs );
	g_ScheduledBlocks++;
	LeaveCriticalSection ( &g_cs );

	return len;
}

int
WIN_Audio_reset ( void )
{
	if ( g_dev != NULL ) {
		while ( g_ScheduledBlocks > 0 ) {
			Sleep (g_ScheduledBlocks);
			while ( g_PlayedWaveHeadersCount > 0) free_memory(); // free used blocks ...				
		}
		waveOutReset (g_dev);      // reset the device
	}
	g_ScheduledBlocks = 0;
	return 0;
}


int
WIN_Audio_close ( void )
{
	if ( g_dev != NULL ) {
		while ( g_ScheduledBlocks > 0 ) {
			Sleep (g_ScheduledBlocks);
			while ( g_PlayedWaveHeadersCount > 0) free_memory(); // free used blocks ...				
		}

		waveOutReset (g_dev);      // reset the device
		waveOutClose (g_dev);      // close the device
		g_dev = NULL;
	}

	DeleteCriticalSection ( &g_cs );
	g_ScheduledBlocks = 0;
	return 0;
}

#endif

/* end of wave_out.c */
