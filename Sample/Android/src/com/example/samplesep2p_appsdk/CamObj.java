package com.example.samplesep2p_appsdk;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Date;
import java.util.LinkedList;

import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioRecord;
import android.media.AudioTrack;
import android.media.MediaRecorder;
import android.os.Build.VERSION;
import android.os.Build;
import android.os.Environment;

import com.example.samplesep2p_appsdk.CallbackService.IMsg;
import com.example.samplesep2p_appsdk.CallbackService.IStream;
import com.p2p.AV_PARAMETER;
import com.p2p.INIT_STR;
import com.p2p.MSG_CONNECT_STATUS;
import com.p2p.MSG_GET_DEVICE_VERSION_RESP;
import com.p2p.MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ;
import com.p2p.MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ;
import com.p2p.MSG_GET_USER_INFO_RESP;
import com.p2p.MSG_PTZ_CONTROL_REQ;
import com.p2p.MSG_SET_ALARM_INFO_REQ;
import com.p2p.MSG_SET_CAMERA_PARAM_REQ;
import com.p2p.MSG_SET_CURRENT_WIFI_REQ;
import com.p2p.MSG_SET_DATETIME_REQ;
import com.p2p.MSG_SET_EMAIL_INFO_REQ;
import com.p2p.MSG_SET_FTP_INFO_REQ;
import com.p2p.MSG_SET_SDCARD_REC_PARAM_REQ;
import com.p2p.MSG_SET_USER_INFO_REQ;
import com.p2p.MSG_START_PLAY_REC_FILE_REQ;
import com.p2p.MSG_STOP_PLAY_REC_FILE_REQ;
import com.p2p.SEP2P_AppSDK;
import com.p2p.SEP2P_Define;
import com.p2p.STREAM_HEAD;
import com.utility.Convert;
import com.utility.SE_MP4;
import com.utility.SE_AudioCodec;
import com.utility.SE_VideoCodec;

@SuppressLint("SimpleDateFormat")
public class CamObj implements IMsg, IStream {
	private long nCamDBid = -1;
	private String strCamName = null;
	private String strCamDid = null;
	private String strCamUser = null;
	private String strCamPwd = null;
	private int nCamStatus = -1;
	private Bitmap objBmp = null;
	private int nSum = 0;
	private int nSumPic = 0;
	private int nMode = 0;
	private boolean bSelected = false;
	private String sP2pAPIVer = "";
	private String sFWP2pAppBuildTime = "";
	private String sFWP2pAppVer = "";
	private String sFWDdnsAppVer = "";
	private static boolean m_bInitAudio = false;
	private AudioTrack m_AudioTrack = null;
	private ThreadSpeak m_threadSpeak = null;
	private int m_nCurAudioCodecID = 0;
	private String m_strProductSeries = ""; 
	private int m_nProductSeries = 0; 
	private String m_strDevAdminName = ""; 
	private ThreadPlayAudio m_threadPlayAudio = null;
	private ThreadPlayVideo m_threadPlayVideo = null; 
	private FIFO m_fifoAudio = new FIFO(); 
	private FIFO m_fifoVideo = new FIFO(); 
	
	private FIFO m_fifoAudioPlayback = new FIFO();
	private FIFO m_fifoVideoPlayback = new FIFO();

	private byte[] ADPCMpRaw = new byte[3200];
	private int[] ppHandle = new int[1], in_outLen1 = new int[1]; // G726,G711A,AAC
	//private int[] ppG711AHandle = new int[1], in_outLen3 = new int[1]; // g711a
	private int m_bmWidth = -1, m_bmHeight = -1;

	private int[] ppHandleg726ToSpeak = new int[1];
	private byte[] outG726BufToSpeak = new byte[320];
	private int[] outG726BufLenToSpeak = new int[1];

	private int[] ppHandlePCMToSpeak = new int[1];
	private byte[] outPCMBufToSpeak = new byte[1024];
	private int[] outPCMBufLenToSpeak = new int[1];

	private int[] ppHandleG711ToSpeak = new int[1];
	private byte[] outG711BufToSpeak = new byte[320];
	private int[] outG711BufLenToSpeak = new int[1];

	int[] m_ppHandleMP4 = new int[1];
	boolean m_bRecMP4FirstIFrame=false;
	boolean m_bRecording=false;
	
	public AudioRecord recorder = null;
	private LinkedList<IAVListener> m_listener = new LinkedList<IAVListener>();

	private long m_nCurDevTimeStamp_video = 0L; //20150924
	
	private boolean isPictSave = false;
	public static boolean isTakepic = false;
	public Bitmap mDefaultBmp = Bitmap.createBitmap(1280, 720, Config.RGB_565);//

	public CamObj() {}
	public int getSumPic() {
		return nSumPic;
	}
	public void setSumPic(int sum_pic) {
		this.nSumPic = sum_pic;
	}
	public boolean isSelected() {
		return bSelected;
	}
	public void setSelected(boolean selected) {
		this.bSelected = selected;
	}
	public int getMode() {
		return nMode;
	}
	public void setMode(int mode) {
		this.nMode = mode;
	}
	public boolean isAdminName() {
		if(strCamUser == null || isNullField(m_strDevAdminName)) return false;
		else {
		if(strCamUser.equals(m_strDevAdminName)) return true;
		else return false;
		}
	}
	public Bitmap getBmp() {
		return objBmp;
	}
	public void setBmp(Bitmap bmp) {
		this.objBmp = bmp;
	}
	public int getSum() {
		return nSum;
	}
	public void setSum(int sum) {
		this.nSum = sum;
	}
	public int getStatus() {
		return nCamStatus;
	}
	public void setStatus(int status) {
		this.nCamStatus = status;
	}
	public String getDid() {
		return strCamDid;
	}
	public void setDid(String did) {
		this.strCamDid = did;
	}
	public String getUser() {
		return strCamUser;
	}
	public void setUser(String user) {
		this.strCamUser = user;
	}
	public String getPwd() {
		return strCamPwd;
	}
	public void setPwd(String pwd) {
		this.strCamPwd = pwd;
	}
	public String getName() {
		return strCamName;
	}
	public void setName(String name) {
		this.strCamName = name;
	}
	public long getDBId() {
		return nCamDBid;
	}
	public void setDBId(long nCamDbId) {
		this.nCamDBid = nCamDbId;
	}
	public String getProductSeriesStr() {
		return m_strProductSeries;
	}
	public int getProductSeriesInt() {
		return m_nProductSeries;
	}
	public String getDevAdminName() {
		return m_strDevAdminName;
	}
	public String getP2pAPIVer() {
		return sP2pAPIVer;
	}
	public String getP2pAppBuildTime() {
		return sFWP2pAppBuildTime;
	}
	public String getP2pAppVer() {
		return sFWP2pAppVer;
	}
	public String getFWDdnsAppVer() {
		return sFWDdnsAppVer;
	}
	public static int getAPIVer() {
		byte[] bytDesc = new byte[250];
		Arrays.fill(bytDesc, (byte) 0);
		int nRet= SEP2P_AppSDK.SEP2P_GetSDKVersion(bytDesc, bytDesc.length);
		System.out.println(Convert.bytesToString(bytDesc));
		return nRet;
	}
	public static int initAPI(String strAndroidPackageName) {
		int nRet = -1;
		INIT_STR initStr = null;
		ArrayList<INIT_STR> arrInitStr = new ArrayList<INIT_STR>();
		initStr = new INIT_STR("VIEW","EFGBFFBJKFJKGGJBENHHFOEHGPJJDJNBGNFLEIDOEFJFPJPBHNBKGPPGHAOAMMLOAJNDPPCCLDNLFNGNMHNMNFBOMM");
		arrInitStr.add(initStr);
		SEP2P_AppSDK.SEP2P_Initialize(arrInitStr);
		SE_MP4.SEMP4_Init(strAndroidPackageName, Build.VERSION.SDK_INT);
		return nRet;
	}
	public static int deinitAPI() {
		int nRet = 0;
		nRet = SEP2P_AppSDK.SEP2P_DeInitialize();
		return nRet;
	}
	public int connectDev() {
		int nRet = 0;
		CallbackService.regIMsg(this);
		CallbackService.regIStream(this); 
		nRet = SEP2P_AppSDK.SEP2P_Connect(strCamDid, strCamUser, strCamPwd);
		return nRet;
	}
	public void disconnectDev() {
		SEP2P_AppSDK.SEP2P_Disconnect(strCamDid);
		CallbackService.unregIStream(this);
		CallbackService.unregIMsg(this);
	}
	public static void startSearchInLAN() {
		SEP2P_AppSDK.SEP2P_StartSearch();
	}
	public static void stopSearchInLAN() {
		SEP2P_AppSDK.SEP2P_StopSearch();
	}
	public int PTZCtrol(int PTZnHandle, int nCtrlParam) {
		if(!isAdminName()) return -1;
		byte[] ptz_up_data = null;
		ptz_up_data = MSG_PTZ_CONTROL_REQ.toBytes((byte)PTZnHandle, (byte)nCtrlParam, (byte)0);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_PTZ_CONTROL_REQ,ptz_up_data, ptz_up_data.length);
	}
	public int getCameraParam() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_CAMERA_PARAM_REQ,null, 0);
	}
	public int setCameraParam(int nResolution, int nBright, int nContrast, int nSaturation, int nMode, int nFlip, int nIRLed, int nBitMaskToSet,
			int bOSDTime, int bOSDName, int bOSDTempHumi, String strOSDName)
	{
		byte[] set_camera_param = null;
		set_camera_param = MSG_SET_CAMERA_PARAM_REQ.getBytes(nResolution, nBright, nContrast,nSaturation, 
								nMode, nFlip, (byte)nIRLed, nBitMaskToSet, bOSDTime, bOSDName, bOSDTempHumi, strOSDName);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_CAMERA_PARAM_REQ,set_camera_param, set_camera_param.length);
	}
	public int getUserInfo() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_USER_INFO_REQ,null, 0);
	}
	public int getVersionInfo() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_DEVICE_VERSION_REQ,null, 0);
	}
	public int getSDCardInfo() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_GET_SDCARD_REC_PARAM_REQ, null, 0);
	}
	public int getFTPInfo() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_FTP_INFO_REQ, null,0);
	}
	public int setFTPInfo(String FTPSvr, String User, String Pwd, String Dir, int nPort, int nMode,
			int uploadInterval) {
		byte[] set_ftp_info = null;
		set_ftp_info = MSG_SET_FTP_INFO_REQ.toBytes(FTPSvr, User, Pwd, Dir, nPort, nMode,uploadInterval);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_FTP_INFO_REQ,set_ftp_info, set_ftp_info.length);
	}
	public int setUserInfo(String visitor, String visitorPwd, String admin, String adminPwd) {
		byte[] set_user_info = null;
		set_user_info = MSG_SET_USER_INFO_REQ.toBytes(visitor, visitorPwd, admin, adminPwd);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_USER_INFO_REQ,set_user_info, set_user_info.length);
	}
	public int setSDcardInfo(int bRecordCoverInSDCard, int nRecordTimeLen, int bRecordTime) {
		byte[] sdcard_info = null;
		sdcard_info = MSG_SET_SDCARD_REC_PARAM_REQ.toBytes(bRecordCoverInSDCard, nRecordTimeLen,bRecordTime);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_SET_SDCARD_REC_PARAM_REQ, sdcard_info, sdcard_info.length);
	}
	public int formatSDcard() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_FORMAT_SDCARD_REQ,null, 0);
	}
	
	public int getDateTime() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_DATETIME_REQ, null,0);
	}
	public int setDateTime(int nSecToNow, int nSecTimeZone, int bEnableNTP, String chNTPServer, int nIndexTimeZoneTable){
		byte[] set_date_time = null;
		set_date_time = MSG_SET_DATETIME_REQ.toBytes(nSecToNow, nSecTimeZone, bEnableNTP,chNTPServer, nIndexTimeZoneTable);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_DATETIME_REQ,set_date_time, set_date_time.length);
	}
	
	public int setEmailInfo(String SMTPSvr, String User, String Pwd, String Sender,String Receiver1, String Receiver2, String Receiver3, String Receiver4, String Subject,int nport, int sslauth) {
		byte[] email_info = null;
		MSG_SET_EMAIL_INFO_REQ set_email_req = new MSG_SET_EMAIL_INFO_REQ();
		email_info = set_email_req.toBytes(SMTPSvr, User, Pwd, Sender, Receiver1, Receiver2,Receiver3, Receiver4, Subject, nport, sslauth, "");
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_EMAIL_INFO_REQ,email_info, email_info.length);
	}
	public int setAlarmInfo(byte[] bMDEnable, byte[] nMDSensitivity, int bInputAlarm,int nInputAlarmMode, int bIOLinkageWhenAlarm, int nPresetbitWhenAlarm,
			int bMailWhenAlarm, int bSnapshotToSDWhenAlarm, int bRecordToSDWhenAlarm,
			int bSnapshotToFTPWhenAlarm, int bRecordToFTPWhenAlarm, byte nAudioAlarmSensitivity, 
			int[] md_x, int[] md_y, int[] md_w, int[] md_h) 
	{
		byte[] bytAlarmData = null;
		MSG_SET_ALARM_INFO_REQ stAlarmInfoReq=new MSG_SET_ALARM_INFO_REQ();
		stAlarmInfoReq.set_md_alarm(0, bMDEnable[0]==0 ? false : true); //area 0 enable
		stAlarmInfoReq.set_md_alarm(1, bMDEnable[1]==0 ? false : true); //area 1 enable
		stAlarmInfoReq.set_md_alarm(2, bMDEnable[2]==0 ? false : true); //area 2 enable
		stAlarmInfoReq.set_md_alarm(3, bMDEnable[3]==0 ? false : true); //area 3 enable
		
		stAlarmInfoReq.set_md_sensitivity(0, nMDSensitivity[0]); //area 0 sensitivity
		stAlarmInfoReq.set_md_sensitivity(1, nMDSensitivity[1]); //area 1 sensitivity
		stAlarmInfoReq.set_md_sensitivity(2, nMDSensitivity[2]); //area 2 sensitivity
		stAlarmInfoReq.set_md_sensitivity(3, nMDSensitivity[3]); //area 3 sensitivity
		
		stAlarmInfoReq.set_io_in_alarm((bInputAlarm==0) ? false : true);
		stAlarmInfoReq.set_io_in_level(nInputAlarmMode==0 ? true : false);
		stAlarmInfoReq.set_io_out(bIOLinkageWhenAlarm==0 ? false : true);
		stAlarmInfoReq.set_presetbit_alarm(nPresetbitWhenAlarm==0?false:true, nPresetbitWhenAlarm);
		stAlarmInfoReq.set_pic_to_email(bMailWhenAlarm==0?false:true);
		stAlarmInfoReq.set_pic_to_sd(bSnapshotToSDWhenAlarm==0?false:true);
		stAlarmInfoReq.set_record_to_sd(bRecordToSDWhenAlarm==0?false:true);
		stAlarmInfoReq.set_pic_to_ftp(bSnapshotToFTPWhenAlarm==0?false:true);
		stAlarmInfoReq.set_record_to_ftp(bRecordToFTPWhenAlarm==0?false:true);
		stAlarmInfoReq.set_audio(nAudioAlarmSensitivity==0?false:true, nAudioAlarmSensitivity);
	
		stAlarmInfoReq.set_alarm_time_all_defence();
		//it alarm area to set whole area if x,y,width,heigh of area 0,1,2,3 are 0
		stAlarmInfoReq.set_md_xywh(0,0,0,0,0); //range of area 0 
		stAlarmInfoReq.set_md_xywh(1,0,0,0,0); //range of area 1 
		stAlarmInfoReq.set_md_xywh(2,0,0,0,0); //range of area 2 
		stAlarmInfoReq.set_md_xywh(3,0,0,0,0); //range of area 3 
		bytAlarmData=stAlarmInfoReq.getBytes();
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_ALARM_INFO_REQ, bytAlarmData, bytAlarmData.length);
	}
	public int setCurrentWifi(int bEnable, String chSSID, int nMode, int nAuthtype,
			int nWEPEncrypt, int nWEPKeyFormat, int nWEPDefaultKey, String chWEPKey1,
			String chWEPKey2, String chWEPKey3, String chWEPKey4, int nWEPKey1_bits,
			int nWEPKey2_bits, int nWEPKey3_bits, int nWEPKey4_bits, String chWPAPsk) 
	{
		byte[] current_info_data = null;
		current_info_data = MSG_SET_CURRENT_WIFI_REQ.toBytes(bEnable, chSSID, nMode, nAuthtype,nWEPEncrypt, nWEPKeyFormat, nWEPDefaultKey, chWEPKey1, chWEPKey2, chWEPKey3,chWEPKey4, nWEPKey1_bits, nWEPKey2_bits, nWEPKey3_bits, nWEPKey4_bits, chWPAPsk, (byte)1);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SET_CURRENT_WIFI_REQ,current_info_data, current_info_data.length);
	}
	
	public int getCurrentwifi() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_CURRENT_WIFI_REQ,null, 0);
	}
	public int getWifiList() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_WIFI_LIST_REQ,null, 0);
	}
	public int getAlarmInfo() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_ALARM_INFO_REQ,null, 0);
	}
	public int getEmailInfo() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_GET_EMAIL_INFO_REQ,null, 0);
	}
	public int setDefaultValue() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_SET_CAMERA_PARAM_DEFAULT_REQ, null, 0);
	}
	public void getSnapImg() {
		SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_SNAP_PICTURE_REQ, null, 0);
	}
	//----{{playback
	public int getRemote_rec_file_by_yyyymmdd(int nYearMonDay,int nRecType,int nBeignNoOfThisTime,int nEndNoOfThisTime){
		byte[] bytRecFileReq = MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ.toBytes(nYearMonDay, nRecType, nBeignNoOfThisTime, nEndNoOfThisTime);
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ, bytRecFileReq, bytRecFileReq.length);
	}

	public int getRemote_rec_day_by_yyyymm(int nYearMon, int nRecType) {
		byte[] bytRecDayReq = MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ.toBytes(nYearMon, nRecType);
		int nRet =SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ, bytRecDayReq, bytRecDayReq.length);
		return nRet;
	}
	
	public int startPlayback(int bOnlyPlayIFrame,int nBeginPos_sec,String chFilePath) {
		byte[] bytStartPlaybackReq = MSG_START_PLAY_REC_FILE_REQ.toBytes(bOnlyPlayIFrame, nBeginPos_sec, chFilePath);
		int nPlayback = SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_START_PLAY_REC_FILE_REQ, bytStartPlaybackReq, bytStartPlaybackReq.length);
		String str;
		str=String.format("SEP2P_MSG_START_PLAY_REC_FILE_REQ]=%d %s bOnlyPlayIFrame=%d, nBeginPos_sec=%d, chFilePath=%s", 
				nPlayback, strCamDid, bOnlyPlayIFrame, nBeginPos_sec, chFilePath);
		System.out.println(str);
		if(nPlayback >= 0 && m_threadPlayVideo == null) {
			m_threadPlayVideo = new ThreadPlayVideo(m_fifoVideoPlayback);
			m_threadPlayVideo.start();
		}
		
		try { Thread.sleep(500);} 
		catch (InterruptedException e) { e.printStackTrace();}
		if(nPlayback >= 0 && m_threadPlayAudio == null) {
			m_threadPlayAudio = new ThreadPlayAudio(m_fifoAudioPlayback);
			m_threadPlayAudio.start();
		}
		return nPlayback;
	}
	
	public int stopPlayback(String chFilePath) {
		byte[] bytStoPlaybackReq = MSG_STOP_PLAY_REC_FILE_REQ.toBytes(chFilePath);
		int nRet=SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_STOP_PLAY_REC_FILE_REQ, bytStoPlaybackReq, bytStoPlaybackReq.length);
		System.out.println("CamObj, stopPlayback, nRet="+nRet);
		if(m_threadPlayAudio != null) {
			m_threadPlayAudio.stopPlay();
			m_threadPlayAudio = null;
		}
		try { Thread.sleep(500);
		} catch (InterruptedException e) { e.printStackTrace(); }
		if(m_threadPlayVideo != null) {
			m_threadPlayVideo.stopPlay();
			m_threadPlayVideo = null;
		}
		return nRet;
	}
	//----}}playback
	
	public int startRecord(String strFilename) //for x series product, audio: AAC
	{
		if(strFilename==null) return -1;
		int nRet=SE_MP4.SEMP4_Open(m_ppHandleMP4, strFilename, 0, 0, SE_MP4.MP4_AV_CODECID_VIDEO_H264, 
									16000, 1, SE_MP4.MP4_AV_CODECID_AUDIO_AAC); //H264 + AAC
		System.out.println("SE_MP4.SEMP4_Open="+nRet+",m_ppHandleMP4[0]="+m_ppHandleMP4[0]+",strFilename="+strFilename);
		if(nRet>=0){
			m_bRecMP4FirstIFrame=false;
			m_bRecording=true;
			return 0;
		}else return -2;
	}
	
	public void stopRecord(){
		m_bRecording=false;
		if(m_ppHandleMP4[0]>=0){
			SE_MP4.SEMP4_Close(m_ppHandleMP4);
			m_ppHandleMP4[0]=-1;
		}
		m_bRecMP4FirstIFrame=false;
	}
	
	public boolean isStartVideo() {
		return (m_threadPlayVideo == null) ? true : false;
	}
	
	public int startVideo() {
		int nRet=-1;
		//1>request video stream of H264 and VIDEO_RESO_720P,VIDEO_RESO_VGA1,VIDEO_RESO_QVGA
		//byte[] byts=MSG_START_VIDEO.toBytes(STREAM_HEAD.AV_CODECID_VIDEO_H264, STREAM_HEAD.VIDEO_RESO_QVGA);
		//nRet=SEP2P_AppSDK.SEP2P_SendMsg(strCamDid,SEP2P_Define.SEP2P_MSG_START_VIDEO, byts, byts.length);
		
		//2>default
		nRet = SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_START_VIDEO, null,0);
		if(nRet >= 0 && m_threadPlayVideo == null) {
			m_threadPlayVideo = new ThreadPlayVideo(m_fifoVideo);
			m_threadPlayVideo.start();
		}
		return nRet;
	}

	public void stopVideo() {
		SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_STOP_VIDEO, null, 0);
		if(m_threadPlayVideo != null) {
			m_threadPlayVideo.stopPlay();
			m_threadPlayVideo = null;
		}
	}

	public void stopVideo(TouchedView touchedView) {
		SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_STOP_VIDEO, null, 0);
		if(m_threadPlayVideo != null) {
			m_threadPlayVideo.stopPlay(touchedView);
			m_threadPlayVideo = null;
		}
	}

	public int startAudio() {
		int nRet = SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_START_AUDIO, null,0);
		if(nRet >= 0 && m_threadPlayAudio == null) {
			m_threadPlayAudio = new ThreadPlayAudio(m_fifoAudio);
			m_threadPlayAudio.start();
		}
		return nRet;
	}
	public void stopAudio() {
		SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_STOP_AUDIO, null, 0);
		if(m_threadPlayAudio != null) {
			m_threadPlayAudio.stopPlay();
			m_threadPlayAudio = null;
		}
	}
	public int startTalk() {
		return SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_START_TALK, null, 0);
	}

	public void stopTalk() {
		SEP2P_AppSDK.SEP2P_SendMsg(strCamDid, SEP2P_Define.SEP2P_MSG_STOP_TALK, null, 0);
		if(m_threadSpeak != null) {
			m_threadSpeak.stopThread();
			m_threadSpeak = null;
		}
	}
	private boolean isNullField(String str) {
		if(str == null || str.length() == 0) return true;
		else return false;
	}
	public void regAVListener(IAVListener listener) {
		synchronized (m_listener) {
			if(listener != null && !m_listener.contains(listener)) m_listener.addLast(listener);
		}
	}
	public void unregAVListener(IAVListener listener) {
		synchronized (m_listener) {
			if(listener != null && !m_listener.isEmpty()) {
				for (int i = 0; i < m_listener.size(); i++) {
					if(m_listener.get(i) == listener) {
						m_listener.remove(i);
						break;
					}
				}
			}
		}
	}
	
	private void updateAVListenerMsg(int nMsgType, byte[] pMsg, int nMsgSize, int pUserData) {
		synchronized (m_listener) {
			IAVListener curListener = null;
			for (int i = 0; i < m_listener.size(); i++) {
				curListener = m_listener.get(i);
				curListener.updateMsg(this, nMsgType, pMsg, nMsgSize, pUserData);
			}
		}
	}
	
	private void updateAVListenerFrameInfo(int nWidth, int nHeigh) {
		synchronized (m_listener) {
			IAVListener curListener = null;
			for (int i = 0; i < m_listener.size(); i++) {
				curListener = m_listener.get(i);
				curListener.updateFrameInfo(this, nWidth, nHeigh);
			}
		}
	}
	
	private void updateAVListenerBmpFrame(byte[] pAVData, Bitmap bmp) {
		synchronized (m_listener) {
			IAVListener curListener = null;
			for (int i = 0; i < m_listener.size(); i++) {
				curListener = m_listener.get(i);
				if(isTakepic) {
					isTakepic = false;
					takePicture(bmp);
				}
				curListener.updateBmpFrame(this, pAVData, bmp);
			}
		}
	}

	class ThreadPlayVideo extends Thread {
		public static final int MAX_FRAMEBUF = 4147200; // 1080P
		long mTick1 = 0L;
		STREAM_HEAD stStreamHead = new STREAM_HEAD();
		boolean bFirstFrame_video = true;
		int[] nWidth = new int[1], nHeigh = new int[1];
		byte[] out_bmp565 = new byte[MAX_FRAMEBUF];
		int[] ppHandleH264 = new int[1], in_outLen = new int[1];
		
		public int bmpWidth = 0, bmpHeight = 0, bmpSizeInBytes = 0;
		Bitmap bmpLast = null;
		ByteBuffer bytBuffer = null;
		long m_nFirstTickLocal_video = 0L, m_nTick2_video = 0L, m_nFirstTimestampDevice_video = 0L;
		public boolean bPlaying = false;
		
        private FIFO fifoVideo=null;
		public ThreadPlayVideo(FIFO fifo) {
			super();
			fifoVideo = fifo;
		}
		public void run() {
			System.gc();
			super.run();
			byte[] videoData = null;
			ppHandleH264[0] = -1;
			SE_VideoCodec.SEVideo_Create(SE_VideoCodec.VIDEO_CODE_TYPE_H264, ppHandleH264);
			fifoVideo.removeAll();
			if(bytBuffer != null) bytBuffer.clear();
			bPlaying = true;
			m_nCurDevTimeStamp_video=0L; //20150924
			
			System.out.println("ThreadPlayVideo is going...");
			while (bPlaying) {
				if(fifoVideo.isEmpty()) {
					try {
						Thread.sleep(4);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
					continue;
				}
				if(mTick1 == 0L) mTick1 = System.currentTimeMillis();
				videoData = fifoVideo.removeHead();
				if(videoData != null) myDoVideoData(videoData);
			}
			if(ppHandleH264[0] > -1) {
				SE_VideoCodec.SEVideo_Destroy(ppHandleH264);
				ppHandleH264[0] = -1;
			}
			System.out.println("ThreadPlayVideo exit.");
		}
		public void stopPlay() {
			updateAVListenerBmpFrame(null, mDefaultBmp);
			bPlaying = false;
			if(this.isAlive()) {
				try {
					this.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			@SuppressWarnings("unused")
			byte[] pVideo = null;
			while (!fifoVideo.isEmpty()) {
				pVideo = fifoVideo.removeHead();
				pVideo = null;
			}
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		public void stopPlay(TouchedView touchedView) {
			updateAVListenerBmpFrame(null, mDefaultBmp);
			bPlaying = false;
			if(this.isAlive()) {
				try {
					this.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			@SuppressWarnings("unused")
			byte[] pVideo = null;
			while (!fifoVideo.isEmpty()) {
				pVideo = fifoVideo.removeHead();
				pVideo = null;
			}
			try {
				Thread.sleep(200);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			if(touchedView != null) touchedView.setBackgroundResource(R.drawable.ic_launcher);

		}

		private void myDoVideoData(byte[] pAVData) {
			int nRet = 0, nDataSize = pAVData.length - STREAM_HEAD.MY_LEN;
			stStreamHead.setData(pAVData);
			int nFrameSize = stStreamHead.getStreamDataLen();
			System.arraycopy(pAVData, STREAM_HEAD.MY_LEN, pAVData, 0, nDataSize);
			
			if(m_bRecording && m_ppHandleMP4[0]>=0){
				if(!m_bRecMP4FirstIFrame){
					if(stStreamHead.getParameter()== STREAM_HEAD.VIDEO_FRAME_FLAG_I) m_bRecMP4FirstIFrame=true;
				}
				if(m_bRecMP4FirstIFrame && m_ppHandleMP4[0]>=0){
					int bKey=(stStreamHead.getParameter()== STREAM_HEAD.VIDEO_FRAME_FLAG_I) ? 1 :0;
					int nRec=SE_MP4.SEMP4_AddVideoFrame(m_ppHandleMP4[0], pAVData, nFrameSize, stStreamHead.getTimestamp(), bKey);
					System.out.println("SE_MP4.SEMP4_AddVideoFrame, nRec="+nRec);
				}
			}
			switch (stStreamHead.getCodecID()) {
				case STREAM_HEAD.AV_CODECID_VIDEO_H264:
					if(ppHandleH264[0] > -1) {
						if(bFirstFrame_video && stStreamHead.getParameter() != STREAM_HEAD.VIDEO_FRAME_FLAG_I) break;
						bFirstFrame_video = false;
						while (nFrameSize > 0) {
							in_outLen[0] = MAX_FRAMEBUF;
							nRet = SE_VideoCodec.SEVideo_Decode2RGB565(ppHandleH264[0], pAVData, nFrameSize, out_bmp565, in_outLen, nWidth, nHeigh);
							if(nRet < 0) {
								nFrameSize = 0;
								break;
							}
							if(!bPlaying) break;
							if(nWidth[0] > 0 && nWidth[0] != bmpWidth) {
								bmpWidth = nWidth[0];
								bmpHeight = nHeigh[0];
								bmpSizeInBytes = bmpWidth * bmpHeight * 2;
								bmpLast = Bitmap.createBitmap(bmpWidth, bmpHeight, Config.RGB_565);
								m_bmWidth = bmpWidth;
								m_bmHeight = bmpHeight;
								System.out.println("ThreadPlayVideo] "+bmpWidth+"x"+bmpHeight);
								updateAVListenerFrameInfo(m_bmWidth, m_bmHeight);
							}
							long nCurDevTimeStamp = 0L, nDiffTimeStamp = 0L;
							nCurDevTimeStamp = stStreamHead.getTimestamp();
							m_nCurDevTimeStamp_video=nCurDevTimeStamp; //20150924
							m_nTick2_video = System.currentTimeMillis();
							if(m_nFirstTimestampDevice_video == 0L || m_nFirstTickLocal_video == 0L) {
								m_nFirstTimestampDevice_video = nCurDevTimeStamp;
								m_nFirstTickLocal_video = m_nTick2_video;
							}
							if(m_nTick2_video < m_nFirstTickLocal_video	|| nCurDevTimeStamp < m_nFirstTimestampDevice_video) {
								m_nFirstTimestampDevice_video = nCurDevTimeStamp;
								m_nFirstTickLocal_video = m_nTick2_video;
							}
							nDiffTimeStamp = (nCurDevTimeStamp - m_nFirstTimestampDevice_video)-(m_nTick2_video - m_nFirstTickLocal_video);
							if(nDiffTimeStamp > 10 && nDiffTimeStamp < 500) {
								try {
									Thread.sleep(nDiffTimeStamp);
								} catch (InterruptedException e) {
									e.printStackTrace();
								}
							}
							bytBuffer = ByteBuffer.wrap(out_bmp565, 0, bmpSizeInBytes);
							if(bPlaying) {
								bmpLast.copyPixelsFromBuffer(bytBuffer);
							} else {
								bmpLast = mDefaultBmp;
							}
							updateAVListenerBmpFrame(pAVData, bmpLast);
							nFrameSize = 0;
						}
					}
					break;

				case STREAM_HEAD.AV_CODECID_VIDEO_MJPEG:
					Bitmap bmp = null;
					try {
						if(bPlaying) bmp = BitmapFactory.decodeByteArray(pAVData, 0, nDataSize);
						else bmp = mDefaultBmp;
					}catch(Exception e) { e.printStackTrace(); }
					bmpLast = bmp;
					if(bmp != null) {
						bmpWidth = bmpLast.getWidth();
						bmpHeight = bmpLast.getHeight();
						m_bmWidth = bmpWidth;
						m_bmHeight = bmpHeight;
						updateAVListenerFrameInfo(m_bmWidth, m_bmHeight);
					}
					updateAVListenerBmpFrame(pAVData, bmpLast);
					break;

				default:
					break;
			}
		}
	}

	class ThreadPlayAudio extends Thread {
		public static final int MAX_AUDIOBUF = 3200;
		byte[] pRaw = new byte[MAX_AUDIOBUF];
		byte[] pG711Raw = new byte[MAX_AUDIOBUF];

		STREAM_HEAD stStreamHead = new STREAM_HEAD();
		boolean bPlaying = false;
		long m_nFirstTickLocal_audio = 0L, m_nTick2_audio = 0L, m_nFirstTimestampDevice_audio = 0L;
		int nNoPlayCount = 0;

		
		private FIFO fifoAudio;
		public ThreadPlayAudio(FIFO fifo) {
			super();
			fifoAudio = fifo;
		}
		
		@Override
		public void run() {
			byte[] audioData = null;
			boolean bFirst = true;
			int nAudioDataSize = 0;
			fifoAudio.removeAll();
			long nTimestamp = 0L;

			ppHandle[0] = -1;
			in_outLen1[0] = MAX_AUDIOBUF;

			bPlaying = true;
			System.out.println("ThreadPlayAudio is going...");
			while (bPlaying) {
				if(fifoAudio.isEmpty()) {
					try { Thread.sleep(6);} 
					catch (InterruptedException e) { e.printStackTrace(); }
					continue;
				}
				//----{{20150924
				audioData = fifoAudio.getFirstNode();
				if(audioData != null) {
					stStreamHead.setData(audioData);
					if(stStreamHead.getFlagPlayback()==1){
						if(stStreamHead.getTimestamp()>m_nCurDevTimeStamp_video){
							try { Thread.sleep(6); } 
							catch(InterruptedException e) { e.printStackTrace(); }
							//System.out.println("stStreamHead.getTimestamp()="+stStreamHead.getTimestamp()+
							//		           ",m_nCurDevTimeStamp_video="+m_nCurDevTimeStamp_video);
							continue; //juju20150924
						}
					}
					fifoAudio.removeHead();
					//----}}20150924
					
					if(bFirst) {
						bFirst = false;
						int nSampleRate=8000;
						if(m_nCurAudioCodecID==STREAM_HEAD.AV_CODECID_AUDIO_AAC) nSampleRate=16000;
						boolean bRet = initAudioDev(nSampleRate, STREAM_HEAD.AUDIO_CHANNEL_MONO,STREAM_HEAD.AUDIO_DATABITS_16);
						if(bRet)m_AudioTrack.play();
					}
					nAudioDataSize = audioData.length - STREAM_HEAD.MY_LEN;
					System.arraycopy(audioData, STREAM_HEAD.MY_LEN, audioData, 0, nAudioDataSize);
					nTimestamp = stStreamHead.getTimestamp();
					myDoAudioData(stStreamHead, nTimestamp, audioData, nAudioDataSize);
				}else fifoAudio.removeHead(); //20150924
			}// while-end
			deinitAudioDev();
			System.out.println("ThreadPlayAudio exit");
		}

		@SuppressWarnings("deprecation")
		public synchronized boolean initAudioDev(int sampleRateInHz, int channel, int dataBit) {
			if(!m_bInitAudio) {
				int channelConfig = 2;
				int audioFormat = 2;
				int mMinBufSize = 0;
				channelConfig = (channel == STREAM_HEAD.AUDIO_CHANNEL_STERO) ? AudioFormat.CHANNEL_CONFIGURATION_STEREO: AudioFormat.CHANNEL_CONFIGURATION_MONO;
				audioFormat = (dataBit == STREAM_HEAD.AUDIO_DATABITS_16) ? AudioFormat.ENCODING_PCM_16BIT: AudioFormat.ENCODING_PCM_8BIT;
				mMinBufSize = AudioTrack.getMinBufferSize(sampleRateInHz, channelConfig,audioFormat);
				if(mMinBufSize == AudioTrack.ERROR_BAD_VALUE || mMinBufSize == AudioTrack.ERROR) return false;
				try {
					m_AudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, sampleRateInHz,channelConfig, audioFormat, mMinBufSize, AudioTrack.MODE_STREAM);
				} catch (IllegalArgumentException iae) {
					iae.printStackTrace();
					return false;
				}
				m_AudioTrack.play();
				m_bInitAudio = true;
				return true;
			} else return false;
		}

		public synchronized void deinitAudioDev() {
			if(m_bInitAudio) {
				if(m_AudioTrack != null) {
					m_AudioTrack.stop();
					m_AudioTrack.release();
					m_AudioTrack = null;
				}
				m_bInitAudio = false;
			}
		}

		public void stopPlay() {
			bPlaying = false;
			if(this.isAlive()) {
				try {
					this.join();
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			@SuppressWarnings("unused")
			byte[] pAudio = null;
			while (!fifoAudio.isEmpty()) {
				pAudio = fifoAudio.removeHead();
				pAudio = null;
			}
		}

		private void myDoAudioData(STREAM_HEAD stAVDataHead, long nDevTimestamp, byte[] bytAudioData, int nAudioDataSize) {
			//System.out.println("ThreadPlayAudio.myDoAudioData] getCodecID="+stAVDataHead.getCodecID());
			switch (stAVDataHead.getCodecID()) {
				case STREAM_HEAD.AV_CODECID_AUDIO_G726: {// decode G726
					if(ppHandle[0]==-1)	SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_G726, ppHandle);
					SE_AudioCodec.SEAudio_Decode(ppHandle[0], bytAudioData, nAudioDataSize, pRaw,in_outLen1);
					m_nTick2_audio = System.currentTimeMillis();
					if(m_nFirstTimestampDevice_audio == 0L || m_nFirstTickLocal_audio == 0L) {
						m_nFirstTimestampDevice_audio = nDevTimestamp;
						m_nFirstTickLocal_audio = m_nTick2_audio;
					}
					if(m_nTick2_audio < m_nFirstTickLocal_audio
							|| nDevTimestamp < m_nFirstTimestampDevice_audio) {
						m_nFirstTimestampDevice_audio = nDevTimestamp;
						m_nFirstTickLocal_audio = m_nTick2_audio;
					}
					long nDiffTimeStamp = (nDevTimestamp - m_nFirstTimestampDevice_audio)
							- (m_nTick2_audio - m_nFirstTickLocal_audio);
					if(nDiffTimeStamp > 10 && nDiffTimeStamp < 100) {
						try {
							Thread.sleep(nDiffTimeStamp);
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
					}
					nNoPlayCount++;
					if(nNoPlayCount >= 100) nNoPlayCount = 0;
					else m_AudioTrack.write(pRaw, 0, in_outLen1[0]);
				}
					break;

				case STREAM_HEAD.AV_CODECID_AUDIO_ADPCM: {// decode adpcm
					if(ppHandle[0]==-1)	SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_ADPCM, ppHandle);
					SE_AudioCodec.SEAudio_Decode(ppHandle[0], bytAudioData, nAudioDataSize,ADPCMpRaw, in_outLen1);
					nNoPlayCount++;
					if(nNoPlayCount >= 100) nNoPlayCount = 0;
					m_AudioTrack.write(ADPCMpRaw, 0, in_outLen1[0]);
					}break;

				case STREAM_HEAD.AV_CODECID_AUDIO_AAC:{ //decode AAC
					if(m_bRecording && m_ppHandleMP4[0]>=0){
						int nRec=SE_MP4.SEMP4_AddAudioFrame(m_ppHandleMP4[0], bytAudioData, nAudioDataSize, stStreamHead.getTimestamp());
						System.out.println("SE_MP4.SEMP4_AddAudioFrame, nRec="+nRec);
					}
					if(ppHandle[0]==-1)	SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_AAC, ppHandle);
					SE_AudioCodec.SEAudio_Decode(ppHandle[0], bytAudioData, nAudioDataSize,pG711Raw, in_outLen1);
					nNoPlayCount++;
					if(nNoPlayCount >= 100) nNoPlayCount = 0;
					m_AudioTrack.write(pG711Raw, 0, in_outLen1[0]);
					}break;
					
				default:;
			}
		}
	}

	class ThreadSpeak extends Thread {
		private int[] ppHandleToSpeak = new int[1];
		byte[] outBufToSpeak = new byte[1024];
		int[] outBufLenToSpeak = new int[1];
		private int SAMPLE_RATE_IN_HZ = 8000;
		private int nMinBufSize = 0, nReadBytes = 0, nSizeAssembled = 0;
		byte[] inBuf = null, inBuf2 = new byte[1024]; // new byte[320];
		int nReadByteNum = 0, nRet = 0;
		byte flag = (byte) (STREAM_HEAD.AUDIO_SAMPLE_8K << 2 | STREAM_HEAD.AUDIO_DATABITS_16 << 1 | STREAM_HEAD.AUDIO_CHANNEL_MONO);
		volatile boolean bSpeaking = false;
		// ----{{juju8-----
		int nBufDataSize = 0;
		byte[] bytsTmp = null; // new byte[1024 * 3];

		int assembleData(byte[] bytBuf, byte[] bytSrcData, int nSrcDataSize, byte[] bytDstBuf,int nDstNeedSize) {
			try {
				int nRealSize = 0;
				if(nSrcDataSize > 0) {
					System.arraycopy(bytSrcData, 0, bytBuf, nBufDataSize, nSrcDataSize);
					nBufDataSize += nSrcDataSize;
					//System.out.println("adpcm nSizeAssembled] 1 nBufDataSize=" + nBufDataSize+ ",nDstNeedSize=" + nDstNeedSize);
				}
				if(nBufDataSize >= nDstNeedSize) {
					nRealSize = nDstNeedSize;
					System.arraycopy(bytBuf, 0, bytDstBuf, 0, nDstNeedSize);
					nBufDataSize = nBufDataSize - nDstNeedSize;
					//System.out.println("adpcm nSizeAssembled] 2 nBufDataSize=" + nBufDataSize);
					System.arraycopy(bytBuf, nDstNeedSize, bytBuf, 0, nBufDataSize);
				}
				//System.out.println("adpcm nSizeAssembled] 3 nSrcDataSize=" + nSrcDataSize+ ",nBufDataSize=" + nBufDataSize + ",nRealSize=" + nRealSize);
				return nRealSize;
			} catch (Exception e) {
				System.out.println("adpcm nSizeAssembled] excep " + e.getMessage());
				return -1;
			}
		}

		// ----}}juju8-----
		@Override
		public void run() {
			if(m_nCurAudioCodecID == STREAM_HEAD.AV_CODECID_AUDIO_AAC) SAMPLE_RATE_IN_HZ = 16000;
			nMinBufSize = AudioRecord.getMinBufferSize(SAMPLE_RATE_IN_HZ,
					AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT);
			recorder = new AudioRecord(MediaRecorder.AudioSource.MIC, SAMPLE_RATE_IN_HZ,
					AudioFormat.CHANNEL_IN_MONO, AudioFormat.ENCODING_PCM_16BIT, nMinBufSize);
			recorder.startRecording();
			bSpeaking = true;

			bytsTmp = new byte[nMinBufSize * 8];
			inBuf = new byte[nMinBufSize];
			ppHandleToSpeak[0] = -1;

			switch (m_nCurAudioCodecID) {
				case STREAM_HEAD.AV_CODECID_AUDIO_G726:
					SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_G726,
							ppHandleToSpeak);
					break;
				case STREAM_HEAD.AV_CODECID_AUDIO_G711A:
					SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_G711A,
							ppHandleToSpeak);
					break;
				case STREAM_HEAD.AV_CODECID_AUDIO_ADPCM:
					SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_ADPCM, ppHandleToSpeak);
					break;
				case STREAM_HEAD.AV_CODECID_AUDIO_AAC:
					SE_AudioCodec.SEAudio_Create(SE_AudioCodec.AUDIO_CODE_TYPE_AAC, ppHandleToSpeak);
					break;
			}

			while (bSpeaking) {
				// audio encode
				if(m_nCurAudioCodecID == STREAM_HEAD.AV_CODECID_AUDIO_G726) {
					Arrays.fill(inBuf, (byte) 0); // yanyan6
					nReadBytes = recorder.read(inBuf, 0, nMinBufSize);
					//System.out.println("wai g726 nReadBytes=" + nReadBytes + ",nMinBufSize="+ nMinBufSize);
					if(nReadBytes > 0) {// juju
						do {
							outBufLenToSpeak[0] = outBufToSpeak.length;
							nSizeAssembled = assembleData(bytsTmp, inBuf, nReadBytes, inBuf2, 320);
							//System.out.println("g726 nSizeAssembled=" + nSizeAssembled + ",nBufDataSize=" + nBufDataSize);
							nReadBytes = 0;
							if(nSizeAssembled > 0) { // juju8
								int nread_encode = SE_AudioCodec.SEAudio_Encode(ppHandleToSpeak[0],
										inBuf2, nSizeAssembled, outBufToSpeak, outBufLenToSpeak);
								// send audio data encoded
								if(nread_encode > 0) {
									// 写编码后的
									nRet = SEP2P_AppSDK.SEP2P_SendTalkData(strCamDid,
											outBufToSpeak, outBufLenToSpeak[0],
											System.currentTimeMillis());
								}
							} else break;
						} while (true);
					}
				}

				else if(m_nCurAudioCodecID == STREAM_HEAD.AV_CODECID_AUDIO_G711A) {
					Arrays.fill(inBuf, (byte) 0); // yanyan6
					nReadBytes = recorder.read(inBuf, 0, nMinBufSize);
					System.out.println();
					if(nReadBytes > 0) {// juju
						do {
							outBufLenToSpeak[0] = outBufToSpeak.length;
							nSizeAssembled = assembleData(bytsTmp, inBuf, nReadBytes, inBuf2, 320);
							//System.out.println("g711 nSizeAssembled=" + nSizeAssembled+ ",nBufDataSize=" + nBufDataSize);
							nReadBytes = 0;
							if(nSizeAssembled > 0) { // juju8
								int nread_encode = SE_AudioCodec.SEAudio_Encode(ppHandleToSpeak[0],
										inBuf2, nSizeAssembled, outBufToSpeak, outBufLenToSpeak);
								// send audio data encoded
								if(nread_encode > 0) nRet = SEP2P_AppSDK.SEP2P_SendTalkData(
										strCamDid, outBufToSpeak, outBufLenToSpeak[0],
										System.currentTimeMillis());
								// 1-20 add new code
								// try {
								// g711outputStream.write(outBufToSpeak,0,outBufLenToSpeak[0]);
								// } catch (IOException e) {
								// e.printStackTrace();
								// }
							} else break;
						} while (true);
					}
				}

				else if(m_nCurAudioCodecID == STREAM_HEAD.AV_CODECID_AUDIO_ADPCM) {
					Arrays.fill(inBuf, (byte) 0); // yanyan6
					//System.out.println("adpcm nMinBufSize" + nMinBufSize);
					nReadBytes = recorder.read(inBuf, 0, nMinBufSize);
					if(nReadBytes > 0) {// juju
						do {
							outBufLenToSpeak[0] = outBufToSpeak.length;
							nSizeAssembled = assembleData(bytsTmp, inBuf, nReadBytes, inBuf2, 1024);
							nReadBytes = 0;
							if(nSizeAssembled > 0) { // juju8
								int nread_encode = SE_AudioCodec.SEAudio_Encode(ppHandleToSpeak[0],
										inBuf2, nSizeAssembled, outBufToSpeak, outBufLenToSpeak);
								// send audio data encoded
								if(nread_encode > 0) {
									nRet = SEP2P_AppSDK.SEP2P_SendTalkData(strCamDid,
											outBufToSpeak, outBufLenToSpeak[0],
											System.currentTimeMillis());
								}
							} else break;
						} while (true);
					}
				}else if(m_nCurAudioCodecID == STREAM_HEAD.AV_CODECID_AUDIO_AAC) {
					Arrays.fill(inBuf, (byte) 0); // yanyan6
					nReadBytes = recorder.read(inBuf, 0, nMinBufSize);
					if(nReadBytes > 0) {// juju
						do {
							outBufLenToSpeak[0] = outBufToSpeak.length;
							nSizeAssembled = assembleData(bytsTmp, inBuf, nReadBytes, inBuf2, 1024);
							nReadBytes = 0;
							if(nSizeAssembled > 0) { // juju8
								int nread_encode = SE_AudioCodec.SEAudio_Encode(ppHandleToSpeak[0],inBuf2, nSizeAssembled, outBufToSpeak, outBufLenToSpeak);
								// send audio data encoded
								if(nread_encode > 0 && outBufLenToSpeak[0] > 0) 
									nRet = SEP2P_AppSDK.SEP2P_SendTalkData(strCamDid, outBufToSpeak, outBufLenToSpeak[0], System.currentTimeMillis());
							} else break;
						} while (true);
					}
				}
				if(!bSpeaking) break;
				try {
					Thread.sleep(20);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}
			if(recorder != null) {
				recorder.stop();
				recorder.release();
				recorder = null;
			}
			if(ppHandleToSpeak[0] > -1) {
				SE_AudioCodec.SEAudio_Destroy(ppHandleToSpeak);
				ppHandleToSpeak[0] = -1;
			}
		}

		public void stopThread() {
			try {
				Thread.sleep(800);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
			bSpeaking = false;
			try {
				if(this.isAlive()) {
					try {
						this.join();
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
			} catch (NullPointerException npe) {
				npe.printStackTrace();
			}
		}
	}

	public static int[] pprecordHandle = new int[1];
	private String getRecordDateTime() {
		Date d = new Date();
		SimpleDateFormat f = new SimpleDateFormat("yyyyMMdd_HHmmss");
		String strDate = f.format(d);
		System.out.println(strCamDid + "!" + "LOD_" + strDate);
		return strCamDid + "!" + "LOD_" + strDate;
	}

	private void takePicture(final Bitmap bmp) {
		if(isPictSave == false) {
			isPictSave = true;
			new Thread() {
				public void run() {
					savePicToSDcard(bmp);
				}
			}.start();
		} else {
			return;
		}
	}
	
	public String getRecordFilename(){
		try {
			File fileRoot=Environment.getExternalStorageDirectory();
			File div = new File(fileRoot, "IPcamer/video");
			if(!div.exists()) { div.mkdirs(); }
			return (fileRoot+"/IPcamer/video/"+getRecordDateTime()+".mp4");
		} catch (Exception e) { e.printStackTrace(); }
		return null;
	}

	private String getStrDate() {
		Date d = new Date();
		SimpleDateFormat f = new SimpleDateFormat("yyyy-MM-dd_HH_mm");
		String strDate = f.format(d);
		return strDate;
	}

	private synchronized void savePicToSDcard(final Bitmap bmp) {
		String strDate = getStrDate();
		FileOutputStream fos = null;
		try {
			File div = new File(Environment.getExternalStorageDirectory(), "IPcamer/pic");
			if(!div.exists()) {
				div.mkdirs();
			}
			File file = new File(div, strDate + "_" + "=" + strCamDid + "!" + strDate + "_" + ".jpg");
			fos = new FileOutputStream(file);
			if(bmp.compress(Bitmap.CompressFormat.JPEG, 100, fos)) {
				fos.flush();
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			isPictSave = false;
			if(fos != null) {
				try {
					fos.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
				fos = null;
			}
		}
	}

	@Override
	public void onMsg(String pDID, int nMsgType, byte[] pMsg, int nMsgSize, int pUserData) {
		if(isNullField(strCamDid)) return;
		if(!strCamDid.equals(pDID)) return;
		if(nMsgType == SEP2P_Define.SEP2P_MSG_CONNECT_STATUS) {
			MSG_CONNECT_STATUS stConnectStatus = new MSG_CONNECT_STATUS(pMsg);
			//refer to SEP2P_Define.CONNECT_STATUS
			System.out.println("CamObj::onMsg] connected, getConnectStatus="+stConnectStatus.getConnectStatus());
			if(stConnectStatus.getConnectStatus() == MSG_CONNECT_STATUS.CONNECT_STATUS_CONNECTED) {
				AV_PARAMETER stAVParameter = new AV_PARAMETER();
				SEP2P_AppSDK.SEP2P_GetAVParameterSupported(strCamDid, stAVParameter);
				m_nCurAudioCodecID = stAVParameter.getAudioCodecID();
				System.out.println("CamObj::onMsg] connected, getAudioCodecID()="+ m_nCurAudioCodecID);
				SEP2P_AppSDK.SEP2P_SendMsg(pDID, SEP2P_Define.SEP2P_MSG_GET_USER_INFO_REQ, null, 0);
				if(m_nProductSeries == 0) SEP2P_AppSDK.SEP2P_SendMsg(pDID,SEP2P_Define.SEP2P_MSG_GET_DEVICE_VERSION_REQ, null, 0);
			}
			nCamStatus = stConnectStatus.getConnectStatus();
		} else if(nMsgType == SEP2P_Define.SEP2P_MSG_GET_USER_INFO_RESP) {
			MSG_GET_USER_INFO_RESP user_info_resp = new MSG_GET_USER_INFO_RESP(pMsg);
			m_strDevAdminName = user_info_resp.getchAdmin();
			
		} else if(nMsgType == SEP2P_Define.SEP2P_MSG_GET_DEVICE_VERSION_RESP) {
			MSG_GET_DEVICE_VERSION_RESP get_device_resp = new MSG_GET_DEVICE_VERSION_RESP(pMsg);
			m_nProductSeries = get_device_resp.getProductSeriesInt2();
			m_strProductSeries = get_device_resp.getProductSeriesStr();
			sP2pAPIVer 		= get_device_resp.getAPIVer();
			sFWP2pAppBuildTime = get_device_resp.getP2PBuildTime();
			sFWP2pAppVer 	= get_device_resp.getFwddnsVer();
			sFWDdnsAppVer 	= get_device_resp.getFwddnsVer();
			System.out.println("imn_server_port="+get_device_resp.getImnServerPort());
			
		} else if(nMsgType == SEP2P_Define.SEP2P_MSG_START_TALK_RESP) {
			if(pMsg != null) { // can talk
				if(pMsg[0] == 0) {
					System.out.println("CamObj::onMsg] talk success...");
					if(m_threadSpeak == null) {
						m_threadSpeak = new ThreadSpeak();
						m_threadSpeak.start();
					}
				} else if(pMsg[0] == 1) {
					System.out.println("CamObj::onMsg] Other is talking...");
				}
			} else System.out.println("CamObj::onMsg] Talk failed");
		}
		else if(nMsgType == SEP2P_Define.SEP2P_MSG_START_PLAY_REC_FILE_RESP){
			System.out.println("play_rec_file_resp");
		}else if(nMsgType == SEP2P_Define.SEP2P_MSG_STOP_PLAY_REC_FILE_RESP){
			System.out.println("stop_rec_file_resp");
			
		}else if(nMsgType == SEP2P_Define.SEP2P_MSG_SET_CAMERA_PARAM_RESP){
			System.out.println("SEP2P_MSG_SET_CAMERA_PARAM_RESP");
		}
		updateAVListenerMsg(nMsgType, pMsg, nMsgSize, pUserData);
	}

	@Override
	public void onStream(String pDID, byte[] pData, int nDataSize, int pUserData) {
		int nCodecID = Convert.byteArrayToInt_Little(pData, 0);
		STREAM_HEAD stStreamHead=new STREAM_HEAD(pData);
		if(nCodecID < STREAM_HEAD.AV_CODECID_AUDIO_ADPCM) {
			// video data
			if(strCamDid.equals(pDID)) {
				if(stStreamHead.getFlagPlayback()== 0){ //live stream of the video
					m_fifoVideo.addLast(pData, nDataSize);
				}else {//playback stream of the video
					char cIPFrame='I';
					int nParam=stStreamHead.getParameter()&0xFF;
					cIPFrame=(nParam==STREAM_HEAD.VIDEO_FRAME_FLAG_I) ? 'I' : 'P';
					System.out.println("PlaybackVideo "+cIPFrame+": playbackID="+stStreamHead.getPlaybackID()+
										",size="+stStreamHead.getStreamDataLen()+",ts="+stStreamHead.getTimestamp());
					//m_fifoVideoPlayback.addLast(pData, nDataSize);
					//System.arraycopy(pData, STREAM_HEAD.MY_LEN, pVideoData, 0, nDataSize-STREAM_HEAD.MY_LEN);
				}
			}
		} else {
			// audio data
			if(strCamDid.equals(pDID))
				if(stStreamHead.getFlagPlayback()==0) {
					//int nParam=stStreamHead.getParameter()&0xFF; //(samplerate << 2) | (databits << 1) | (channel)
					m_fifoAudio.addLast(pData, nDataSize); //live stream of the audio
					
				}else{//playback stream of the audio
					int nParam=stStreamHead.getParameter()&0xFF; //(samplerate << 2) | (databits << 1) | (channel)
					int nSampeRate=nParam>>2;
					int nDatabits =(nParam>>1)&0x01;
					int nChannel  =nParam&0x01;
					String strSampleRate="8K";
					if(nSampeRate==STREAM_HEAD.AUDIO_SAMPLE_16K) strSampleRate="16K";
					else if(nSampeRate==STREAM_HEAD.AUDIO_SAMPLE_44K) strSampleRate="44K";
					System.out.println("PlaybackAudio parameter="+nParam+" [SampeRate="+strSampleRate+",Databit="+nDatabits+",nChannel="+nChannel+"]: playbackID="+
										stStreamHead.getPlaybackID()+",size="+stStreamHead.getStreamDataLen()+",ts="+stStreamHead.getTimestamp());
					//m_fifoAudioPlayback.addLast(pData, nDataSize);
					//System.arraycopy(pData, STREAM_HEAD.MY_LEN, pAudioData, 0, nDataSize-STREAM_HEAD.MY_LEN);
				}
		}
	}
}
