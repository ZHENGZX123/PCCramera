package com.p2p;

import com.utility.Convert;

public class MSG_GET_WIFI_LIST_RESP {
	public static final int MY_LEN = 4;
	
	int nResultCount; 
    //SEP2P_RESULT_WIFI_INFO[] wifi =new SEP2P_RESULT_WIFI_INFO[50]; 
    
    byte[] byt_nResultCount = new byte[4];
    
    public MSG_GET_WIFI_LIST_RESP(byte[] data) {
		System.arraycopy(data, 0, byt_nResultCount, 0, byt_nResultCount.length);
	}
	public int getnResultCount() {
		if(byt_nResultCount == null) return 0;
		else return Convert.byteArrayToInt_Little(byt_nResultCount);
	}
	
}
