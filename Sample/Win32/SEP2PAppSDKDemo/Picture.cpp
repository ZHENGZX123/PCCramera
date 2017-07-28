// Picture.cpp: implementation of the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "SEP2PAppSDKDemo.h"
#include "Picture.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define HIMETRIC_INCH	2540

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPicture::CPicture()
{
	::CoInitialize(NULL);

	m_hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_NODISCARD, JPG_MAXSIZE);
	m_IPicture=NULL;
}

CPicture::~CPicture()
{
	if(m_IPicture != NULL) FreePictureData();

	if(m_hGlobal) {
		GlobalFree(m_hGlobal);
		m_hGlobal=NULL;
	}
	::CoUninitialize();
}

void CPicture::FreePictureData()
{
	if(m_IPicture != NULL)
	{
		m_IPicture->Release();
		m_IPicture	= NULL;
	}
}

BOOL CPicture::PushData(BYTE *pData, int nSize)
{
	BOOL bResult = FALSE;
	if(m_IPicture != NULL)  FreePictureData();
	if(LoadPictureData(pData, nSize)) bResult=TRUE;

	return bResult;
}
BOOL CPicture::LoadPictureData(BYTE *pBuffer, int nSize)
{
	BOOL bResult = FALSE;
	if(m_hGlobal == NULL) return FALSE;	

	void* pData = GlobalLock(m_hGlobal);
	memcpy(pData, pBuffer, nSize);
	GlobalUnlock(m_hGlobal);

	IStream  *pStream=NULL;
	if(CreateStreamOnHGlobal(m_hGlobal, FALSE, &pStream) == S_OK)
	{
		HRESULT hr;
		if((hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&m_IPicture)) != S_OK)
		{
			TRACE(_T("IPicture interface is not supported."));
			return(FALSE);			
		}else bResult=TRUE;

		pStream->Release();
		pStream = NULL;
	}else bResult=FALSE;

	return (bResult);
}

BOOL CPicture::PushData(BYTE *pData, BITMAPINFOHEADER *pBmiHead)
{
	BOOL bResult = FALSE;
	if(m_IPicture != NULL)  FreePictureData();
	if(LoadPictureData(pData, pBmiHead)) bResult=TRUE;
	
	return bResult;
}

BOOL CPicture::Show(HDC hDC, CRect *pDrawRect)
{
    if(hDC == NULL || m_IPicture == NULL) return FALSE;
    
    long Width  = 0;
    long Height = 0;
    m_IPicture->get_Width(&Width);
    m_IPicture->get_Height(&Height);
	
	ULONG nTick2=GetTickCount();
	if(m_nTickCount==0 || (nTick2-m_nTickCount)>=6000){
		m_nTickCount=nTick2;
		int nDevCaps=GetDeviceCaps(hDC, LOGPIXELSX);
		m_Width	= MulDiv(Width, nDevCaps, HIMETRIC_INCH);
		m_Height= MulDiv(Height,nDevCaps, HIMETRIC_INCH);
	}

    HRESULT hrP = NULL;
    CRect DrawRect(pDrawRect);
    hrP = m_IPicture->Render(hDC,
		DrawRect.left,                  // Left
		DrawRect.top,                   // Top
		DrawRect.right - DrawRect.left, // Right
		DrawRect.bottom- DrawRect.top,  // Bottom
		0,
		Height,
		Width,
		-Height,
		&DrawRect);
	
    if(SUCCEEDED(hrP)) return(TRUE);

	TRACE(_T("Can not allocate enough memory\n"));
    return(FALSE);
}

BOOL CPicture::LoadPictureData(BYTE *pBuffer, BITMAPINFOHEADER *pBmiHead)//24bit bmp
{
	BOOL bResult = FALSE;
	if(m_hGlobal == NULL) return FALSE;	

	int  nSize=pBmiHead->biSizeImage;
	void *pData = GlobalLock(m_hGlobal);
	
	BITMAPFILEHEADER bmpFile;
	memset(&bmpFile, 0, sizeof(BITMAPFILEHEADER));
	bmpFile.bfType='MB';
	bmpFile.bfOffBits=sizeof( BITMAPINFOHEADER ) + sizeof( BITMAPFILEHEADER );
	bmpFile.bfSize=sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+pBmiHead->biSizeImage;

	memcpy(pData, &bmpFile, sizeof(BITMAPFILEHEADER));
	memcpy((BYTE *)pData+sizeof(BITMAPFILEHEADER), pBmiHead, sizeof(BITMAPINFOHEADER));
	memcpy(((BYTE *)pData+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)), pBuffer, nSize);
	GlobalUnlock(m_hGlobal);

	IStream  *pStream=NULL;
	if(CreateStreamOnHGlobal(m_hGlobal, FALSE, &pStream) == S_OK)
	{
		HRESULT hr;
		//CTL_E_INVALIDPICTURE (0x800A01E1)
		//E_UNEXPECTED
		hr = OleLoadPicture(pStream, bmpFile.bfSize, FALSE, IID_IPicture, (LPVOID *)&m_IPicture);
		if(hr== E_NOINTERFACE || hr==CTL_E_INVALIDPICTURE)
		{
			TRACE(_T("IPicture interface is not supported."));
			return(FALSE);			
		}else bResult=TRUE;

		pStream->Release();
		pStream = NULL;
	}else bResult=FALSE;
	
	return (bResult);
}
