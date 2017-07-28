package com.utility;

public class SE_AVI {
	static {
		try { System.loadLibrary("SE_AVI"); }
		catch (UnsatisfiedLinkError ule) { System.out.println("loadLibrary(SE_AVI)," + ule.getMessage()); }
	}
	public native static int SEAVI_GetSdkVer(byte[] chDesc, int nDescMaxSize);
	public native static int SEAVI_OpenAvi(int[] ppHandle, String filename, String fourcc, int height, int width, int framerate);
	public native static int SEAVI_CloseAvi(int[] ppHandle);
	public native static int SEAVI_WriteData(int pHandle, byte[] data, int len, int keyframe);
	
}
