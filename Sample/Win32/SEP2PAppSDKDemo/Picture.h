// Picture.h: interface for the CPicture class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PICTURE_H__INCLUDED_)
#define AFX_PICTURE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define JPG_MAXSIZE	6220800 // 1920*1080*3

class CPicture  
{
public:
	CPicture();
	virtual ~CPicture();
	
	BOOL PushData(BYTE *pData, int nSize);
	BOOL LoadPictureData(BYTE *pBuffer, int nSize);
	BOOL PushData(BYTE *pData, BITMAPINFOHEADER *pBmiHead);
	BOOL Show(HDC hDC, CRect *pDrawRect);
	BOOL LoadPictureData(BYTE *pBuffer, BITMAPINFOHEADER *pBmiHead);
	void FreePictureData();
	
	LONG GetVWidth()	{ return m_Width; }
	LONG GetVHeight()	{ return m_Height;}

protected:
	ULONG m_nTickCount;
	HGLOBAL   m_hGlobal;	
	IPicture  *m_IPicture;
	LONG      m_Width, m_Height;

};

#endif // !defined(AFX_PICTURE_H__INCLUDED_)
