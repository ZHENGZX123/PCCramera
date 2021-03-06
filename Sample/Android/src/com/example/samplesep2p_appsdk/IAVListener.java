package com.example.samplesep2p_appsdk;

import android.graphics.Bitmap;

public interface IAVListener {
	public void updateFrameInfo(Object obj, int nWidth, int nHeigh);
	public void updateBmpFrame(Object obj, byte[] rawVideoData, Bitmap bmp);
	public void updateMsg(Object obj, int nMsgType, byte[] pMsg, int nMsgSize, int pUserData);
}