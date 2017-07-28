// stdafx.cpp : source file that includes just the standard includes
// SEP2PAppSDKDemo.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"



BOOL CharFromWSTRU(IN UINT codePage, IN LPCWSTR wstr, IN UINT len, OUT char *pStr)
{
	if( len > 0 )
	{
		size_t mblen = WideCharToMultiByte(codePage, 0, wstr, len, NULL, 0, NULL, NULL);
		if( mblen > 0 )
		{
			char *buffer = (char *)CoTaskMemAlloc(mblen+1);
			ZeroMemory(buffer, mblen+1);
			if( WideCharToMultiByte(codePage, 0, wstr, len, buffer, mblen, NULL, NULL) )
			{
				//buffer[mblen] = '\0';
				memcpy(pStr, buffer, mblen);
				*(pStr+mblen)='\0';
				CoTaskMemFree(buffer);
				return TRUE;
			}
		}
	}else *pStr='\0';

	return FALSE;
};

BOOL BSTRFromCharU(IN UINT codePage, IN LPCSTR s, OUT CString &csWStr)
{
	csWStr=_T("");
	int wideLen = MultiByteToWideChar(codePage, 0, s, -1, NULL, 0);
	if( wideLen > 0 )
	{
		WCHAR* wideStr = (WCHAR*)CoTaskMemAlloc(wideLen*sizeof(WCHAR));
		if( NULL != wideStr)
		{
			BSTR bstr;

			ZeroMemory(wideStr, wideLen*sizeof(WCHAR));
			MultiByteToWideChar(codePage, 0, s, -1, wideStr, wideLen);
			bstr = SysAllocStringLen(wideStr, wideLen-1);
			CoTaskMemFree(wideStr);

			csWStr=bstr;
			SysFreeString(bstr);
			return TRUE;
		}
	}
	return FALSE;
}
