#include "StdAfx.h"
#include "AudioSample.h"

#pragma comment(lib, "Winmm.lib")
CAudioSample::CAudioSample(void)
{
	m_hThreadSample=NULL;
	m_bSamplingAudio=0;
	m_threadIDSampleAudio=0L;
	m_bAudioInOpen	=0;
	m_hWI=NULL;
	m_nSampleRate=8000;
}

CAudioSample::~CAudioSample(void)
{
	StopSample();
}

INT32 CAudioSample::StartSample(int index, int nSampleRate, POnSampleCallback pOnSampleCallback, void* pUserData)
{
	m_nSampleRate=nSampleRate;
	if(m_hThreadSample==NULL){
		m_hThreadSample=CreateThread(NULL, 0,ThreadSampleAudio, this, 0, &m_threadIDSampleAudio);
		if(m_hThreadSample==NULL) return -1;
	}

	m_nIndex=index;
	m_pSampleCallback=pOnSampleCallback;
	m_pUserData=pUserData;

	return 0;
}

void CAudioSample::StopSample()
{
	if(m_threadIDSampleAudio>0) PostThreadMessage(m_threadIDSampleAudio, OM_THREAD_QUIT, 0,0);
	if(m_hThreadSample){
		m_bSamplingAudio=0;

		WaitForSingleObject(m_hThreadSample, INFINITE); 
		CloseHandle(m_hThreadSample); 
		m_hThreadSample=NULL;
		m_threadIDSampleAudio=0;
	}
}

ULONG WINAPI CAudioSample::ThreadSampleAudio(void* lpPara)
{
	CAudioSample *pThis=(CAudioSample *)lpPara;	
	if(!pThis->m_bSamplingAudio){
		pThis->InitWIEnv();
		pThis->DoSampleAudio();
	}

	TRACE(_T("---ThreadSampleAudio exited----\n"));
	return 0L;
}

//@para  >0 success
//		 <0 failure
//			-1: MMSYSERR_NOERROR != woOpenRet
//
INT32 CAudioSample::InitWIEnv()
{
	WAVEFORMATEX WaveFormatEx;
	memset(&WaveFormatEx, 0, sizeof(WaveFormatEx));
	WaveFormatEx.wFormatTag		= WAVE_FORMAT_PCM;
	WaveFormatEx.nChannels		= 1;
	WaveFormatEx.nSamplesPerSec = m_nSampleRate;
	WaveFormatEx.wBitsPerSample = 16;
	WaveFormatEx.nBlockAlign	= 2;
	WaveFormatEx.nAvgBytesPerSec= WaveFormatEx.nBlockAlign*WaveFormatEx.nSamplesPerSec;
	
	MMRESULT wiOpenRet=waveInOpen(&m_hWI, WAVE_MAPPER, &WaveFormatEx, m_threadIDSampleAudio, 0, CALLBACK_THREAD);
	if(MMSYSERR_NOERROR != wiOpenRet) {
		TRACE(_T("CAudio_adpcm::InitWIEnv(): waveInOpen=failure\n"));
		return -1;
	}
	TRACE(_T("CAudioSample::InitWIEnv(): waveInOpen=success\n"));

	return 1;
}

#define SAMPLE_SIZE	640
INT32 CAudioSample::DoSampleAudio()
{
	bool bBegin = false;
	char bufferin1[SAMPLE_SIZE], bufferin2[SAMPLE_SIZE];
	WAVEHDR *pwavehdrin1=NULL, *pwavehdrin2=NULL, *pwavehdrin=NULL;

	pwavehdrin1 = new WAVEHDR;
	pwavehdrin1->lpData  = bufferin1;
	pwavehdrin1->dwBufferLength = SAMPLE_SIZE;
	pwavehdrin1->dwBytesRecorded = 0;
	pwavehdrin1->dwUser  = 0;
	pwavehdrin1->dwLoops = 0;
	pwavehdrin1->dwFlags = WHDR_DONE;
	pwavehdrin1->lpNext  = NULL;
	pwavehdrin1->reserved= 0;
	waveInPrepareHeader(m_hWI, pwavehdrin1,sizeof(WAVEHDR));
	pwavehdrin2 = new WAVEHDR;
	pwavehdrin2->lpData = bufferin2;
	pwavehdrin2->dwBufferLength = SAMPLE_SIZE;
	pwavehdrin2->dwBytesRecorded= 0;
	pwavehdrin2->dwUser = 0;
	pwavehdrin2->dwLoops= 0;
	pwavehdrin2->dwFlags= WHDR_DONE;
	pwavehdrin2->lpNext = NULL;
	pwavehdrin2->reserved = 0;
	waveInPrepareHeader(m_hWI, pwavehdrin2, sizeof(WAVEHDR));

	TRACE(_T("---ThreadSampleAudio going......\n"));
	m_bSamplingAudio=1;
	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		if(!m_bSamplingAudio) goto EXIT_FUN; //goto--------------		
		switch(msg.message) 
		{
		case OM_BEGIN_SAMPLE:
			if(msg.wParam == AUDIO_OK && m_bAudioInOpen)
			{
				bBegin = true;
				waveInAddBuffer(m_hWI, pwavehdrin1, sizeof(WAVEHDR));
				waveInAddBuffer(m_hWI, pwavehdrin2, sizeof(WAVEHDR));
				waveInStart(m_hWI);
				TRACE(_T("audio OM_BEGIN_SAMPLE\n"));
			}
			break;

		case OM_END_SAMPLE:
			bBegin = false;
			if(m_bAudioInOpen) waveInReset(m_hWI);				
			break;

		case MM_WIM_OPEN:
			TRACE(_T("audio MM_WIM_OPEN\n"));
			m_bAudioInOpen=TRUE;
			if(m_threadIDSampleAudio>0) PostThreadMessage(m_threadIDSampleAudio, OM_BEGIN_SAMPLE, AUDIO_OK, 0L);
			break;

		case MM_WIM_DATA:
			pwavehdrin = (WAVEHDR *)msg.lParam;
			if(bBegin) {
				if(m_pSampleCallback!=NULL){
					m_pSampleCallback(m_nIndex, pwavehdrin->lpData, SAMPLE_SIZE, m_pUserData);
				}
				waveInAddBuffer(m_hWI, pwavehdrin, sizeof(WAVEHDR));
			}
			break;

		case MM_WIM_CLOSE:
			break;

		case OM_THREAD_QUIT:
			TRACE(_T("audio OM_THREAD_QUIT\n"));
			goto EXIT_FUN; //goto----------------------------------

		default:;
		}
	}

EXIT_FUN:
	waveInReset(m_hWI);
	waveInUnprepareHeader(m_hWI,pwavehdrin1,sizeof(WAVEHDR));
	waveInUnprepareHeader(m_hWI,pwavehdrin2,sizeof(WAVEHDR));
	waveInClose(m_hWI);
	delete pwavehdrin1;
	delete pwavehdrin2;

	m_bAudioInOpen	=FALSE;
	TRACE(_T("delete pwavehdr1,2\n"));
	return 0;
}
