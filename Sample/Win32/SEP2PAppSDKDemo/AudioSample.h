#pragma once

#include <mmsystem.h>


#define AUDIO_OK				1

#define OM_THREAD_QUIT			WM_USER+100
#define OM_BEGIN_SAMPLE			WM_USER+101
#define OM_END_SAMPLE			WM_USER+102

typedef INT32 (* POnSampleCallback)(
									int index,
									CHAR*	pData,
									UINT32  nDataSize,
									void*	pUserData);
class CAudioSample
{
public:
	CAudioSample(void);
	virtual ~CAudioSample(void);

private:
	CHAR m_bSamplingAudio;
	int  m_nIndex;
	int  m_nSampleRate;

	HWAVEIN	 m_hWI;
	volatile CHAR m_bAudioInOpen;
	HANDLE   m_hThreadSample;
	DWORD    m_threadIDSampleAudio;
	UCHAR    *m_pBufAudioEncoded;
	
	POnSampleCallback m_pSampleCallback;
	void	 *m_pUserData;
	
	INT32 InitWIEnv();
	INT32 DoSampleAudio();

	static ULONG WINAPI ThreadSampleAudio(void* lpPara);

public:
	INT32 StartSample(int index, int nSampleRate,POnSampleCallback pOnSampleCallback, void* pUserData);
	void StopSample();

};
