package com.example.samplesep2p_appsdk;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.KeyEvent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import com.example.samplesep2p_appsdk.CallbackService.IEvent;
import com.example.samplesep2p_appsdk.CallbackService.ILANSearch;
import com.p2p.AlarmBean;
import com.p2p.MSG_CONNECT_STATUS;
import com.p2p.MSG_GET_ALARM_INFO_RESP;
import com.p2p.MSG_GET_CURRENT_WIFI_RESP;
import com.p2p.MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP;
import com.p2p.MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP;
import com.p2p.MSG_START_PLAY_REC_FILE_RESP;
import com.p2p.REC_FILE_INFO;
import com.p2p.SEARCH_RESP;
import com.p2p.SEP2P_Define;
import com.p2p.SEP2P_RESULT_WIFI_INFO;
import com.p2p.STREAM_HEAD;
import com.p2p.WifiBean;
import com.p2p.WifiScanBean;
import com.utility.Convert;

@SuppressLint({ "ShowToast", "HandlerLeak" })
public class MainActivity extends Activity implements ILANSearch, IAVListener, IEvent {
	public static final int MAX_NUM_CAM_OBJ	=2;
	
	private final static int WHAT_SET_ALARM_FAIL	=0;
	private final static int WHAT_SET_ALARM_SUCCESS	=1;
	private final static int WHAT_WIFIPARAMS = 1;
	private final static int WHAT_SCANPARAMS = 2;
	private final static int WHAT_ALERMPARAMS= 3;
	private final static int WHAT_OVER 		 = 4;// set wifi over
	private final static int WHAT_TIMEOUT 	 = 5;	
	private final static int WHAT_SEARCH_ON_LAN=6;
	
	private EditText didEdit, userEdit, pwdEdit;
	private String STR_DID="", STR_USER, STR_PWD;
	private TouchedView[] m_arrViewLive=new TouchedView[MAX_NUM_CAM_OBJ];
	private Button search_btn, play_btn, connect_btn, audio_btn, talk_btn, snap_btn, record_btn;
	private Button btn_up, btn_down, btn_left, btn_right, btn_other_test;
	// private Button btn_get_recfiles, btn_playback;
	private Button btn_playback;

	private boolean isconnect = false;
	private boolean isvideo = false;
	private boolean isaudio = false;
	private boolean isrecord = false;
	private boolean istalk = false;
	private boolean m_bSearchingLAN = false;

	public static CamObj[] m_arrObjCam = new CamObj[MAX_NUM_CAM_OBJ];//array or list
	public AlarmBean alermBean = null;
	
	private String strMsg = "";
	private String m_strFilePlayback = null;
	private boolean bStartPlayback = false;
	WifiBean wifiBean = null;
	
	private String[] X_SERIES_TIMEZONES= {
			"1(GMT-1200)Etc/GMT-12",
			"2(GMT-1100)Pacific/Apia",
			"3(GMT-1000)Pacific/Honolulu",
			"4(GMT-0900)America/Anchorage",
			"5(GMT-0800)America/Los_Angeles",
			"6(GMT-0700)America/Denver",
			"7(GMT-0700)America/Tegucigalpa",
			"8(GMT-0700)America/Phoenix",
			"9(GMT-0600)America/Winnipeg",
			"10(GMT-0600)America/Mexico_City",
			"11(GMT-0600)America/Chicago",
			"12(GMT-0600)America/Costa_Rica",
			"13(GMT-0600)America /Regina",
			"14(GMT-0500)America/Indianapolis",
			"15(GMT-0500)America/New_York",
			"16(GMT-0500)America/Bogota",
			"17(GMT-0400)America/Santiago",
			"18(GMT-0400)America/Montreal",
			"19(GMT-0330)Canada/Newfoundland",
			"20(GMT-0300)America/Thule",
			"21(GMT-0300)America/Buenos_Aires",
			"22(GMT-0300)America/Sao_Paulo",
			"23(GMT-0300)America/Recife",
			"24(GMT-0200)Atlantic/South_Georgia",
			"25(GMT-0200)America/Montevideo",
			"26(GMT-0100)Atlantic/Cape_Verde",
			"27(GMT-0100)Atlantic/Azores",
			"28(GMT)Europe/Dublin",
			"29(GMT)Africa/Casablanca",
			"30(GMT+0100)Europe/Amsterdam",
			"31(GMT+0100)Europe/Belgrade",
			"32(GMT+0100)Europe/Brussels",
			"33(GMT+0100)Europe/Warsaw",
			"34(GMT+0100)Africa/Lagos",
			"35(GMT+0200)Europe/Athens",
			"36(GMT+0200)Europe/Bucharest",
			"37(GMT+0200)Africa/Harare",
			"38(GMT+0200)Europe/Helsinki",
			"39(GMT+0200)Asia/Amman",
			"40(GMT+0200)Africa/Windhoek",
			"41(GMT+0200)Asia/Beirut",
			"42(GMT+0300)Asia/Baghdad",
			"43(GMT+0300)Asia/Kuwait",
			"44(GMT+0300)Europe/Moscow",
			"45(GMT+0300)Africa/Nairobi",
			"46(GMT+0300)Asia/Minsk",
			"47(GMT+0400)Asia/Dubai",
			"48(GMT+0400)Asia/Baku",
			"49(GMT+0400)Asia/Yerevan",
			"50(GMT+0430)Asia/Kabul",
			"51(GMT+0500)Asia/Yekaterinburg",
			"52(GMT+0500)Asia/Karachi",
			"53(GMT+0530)Asia/Calcutta",
			"54(GMT+0545)Asia/Katmandu",
			"55(GMT+0600)Asia/Almaty",
			"56(GMT+0600)Asia/Dhaka",
			"57(GMT+0600)Asia/Colombo",
			"58(GMT+0630)Asia/Rangoon",
			"59(GMT+0700)Asia/Bangkok",
			"60(GMT+0700)Asia/Krasnoyarsk",
			"61(GMT+0800)Asia/Hong_Kong",
			"62(GMT+0800)Asia/Irkutsk",
			"63(GMT+0800)Asia/Kuala_Lumpur",
			"64(GMT+0800)Australia/Perth",
			"65(GMT+0800)Asia/Taipei",
			"66(GMT+0900)Asia/Tokyo",
			"67(GMT+0900)Asia/Seoul",
			"68(GMT+0900)Asia/Yakutsk",
			"69(GMT+0930)Australia/Adelaide",
			"70(GMT+1000)Australia/Brisbane",
			"71(GMT+1000)Australia/Sydney",
			"72(GMT+1000)Pacific/Guam",
			"73(GMT+1000)Australia/Hobart",
			"74(GMT+1000)Asia/Vladivostok",
			"75(GMT+1100)Asia/Magadan",
			"76(GMT+1200)Pacific/Auckland",
			"77(GMT+1200)Pacific/Fiji",
			"78(GMT+1200)Pacific/Majuro",
			"79(GMT+1300)Pacific/Tongatapu"
	};
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main);
		
		String strVer=String.format("%X", CamObj.getAPIVer());
		System.out.println("SEP2P_AppSDK 0x"+ strVer);
		CamObj.initAPI(this.getPackageName());
		init();
		for(CamObj cam: m_arrObjCam) cam=null;
	}

	private void init() {
		m_arrObjCam[0] = new CamObj();     //先只处理1个Camera
		m_arrObjCam[0].regAVListener(this);
		alermBean = new AlarmBean();

		Intent intent = new Intent();
		intent.setClass(MainActivity.this, CallbackService.class);
		startService(intent);
		CallbackService.setLANSearchInterface(this);
		CallbackService.setEventInterface(this); // add new code

		didEdit  = (EditText)findViewById(R.id.didedit);
		userEdit = (EditText)findViewById(R.id.useredit);
		pwdEdit  = (EditText)findViewById(R.id.pwdedit);
		connect_btn= (Button)findViewById(R.id.btn_connect);
		search_btn = (Button)findViewById(R.id.btn_search);
		play_btn   = (Button)findViewById(R.id.btn_play);
		btn_up     = (Button)findViewById(R.id.btn_up);
		btn_down   = (Button)findViewById(R.id.btn_down);
		btn_left   = (Button)findViewById(R.id.btn_left);
		btn_right  = (Button)findViewById(R.id.btn_right);
		audio_btn  = (Button)findViewById(R.id.btn_audio);
		record_btn = (Button)findViewById(R.id.btn_record);
		snap_btn   = (Button)findViewById(R.id.btn_snap);
		talk_btn   = (Button)findViewById(R.id.btn_talk);
		btn_other_test = (Button) findViewById(R.id.btn_other_test);
		// btn_get_recfiles= (Button) findViewById(R.id.btn_get_recfiles);
		btn_playback = (Button) findViewById(R.id.btn_playback);

		m_arrViewLive[0] = (TouchedView) findViewById(R.id.view_live1);
		m_arrViewLive[1] = (TouchedView) findViewById(R.id.view_live2);
	}

	public void isShowBtn(boolean isshow) {
		play_btn.setEnabled(isshow);
		btn_up.setEnabled(isshow);
		btn_down.setEnabled(isshow);
		btn_left.setEnabled(isshow);
		btn_right.setEnabled(isshow);
		audio_btn.setEnabled(isshow);
		record_btn.setEnabled(isshow);
		snap_btn.setEnabled(isshow);
		talk_btn.setEnabled(isshow);
		btn_other_test.setEnabled(isshow);
		// btn_get_recfiles.setEnabled(isshow);
		btn_playback.setEnabled(isshow);
	}

	@SuppressLint("ShowToast")
	@SuppressWarnings("static-access")
	public void btnClick(View v) {
		switch (v.getId()) {
			case R.id.btn_connect:
				if(isconnect) {
					isconnect = false;
					m_arrObjCam[0].disconnectDev();
					connect_btn.setText(getResources().getString(R.string.str_con));
					isShowBtn(false);
				} else {
					STR_DID = didEdit.getText().toString();
					STR_USER = userEdit.getText().toString();
					STR_PWD = pwdEdit.getText().toString();
					m_arrObjCam[0].setDid(STR_DID);
					m_arrObjCam[0].setUser(STR_USER);
					m_arrObjCam[0].setPwd(STR_PWD);
					m_arrObjCam[0].setName("abcd");
					int nRet=m_arrObjCam[0].connectDev();
					System.out.println("m_arrObjCam[0].connectDev()="+nRet);
				}
				break;

			case R.id.btn_search:
				if(m_bSearchingLAN) {
					m_bSearchingLAN = false;
					CamObj.stopSearchInLAN();
					search_btn.setText(getResources().getString(R.string.str_start_search));
				} else {
					m_bSearchingLAN = true;
					CamObj.startSearchInLAN();
					search_btn.setText(getResources().getString(R.string.str_stop_search));
				}
				break;

			//StartVideo
			case R.id.btn_play:
				if(isvideo) {
					isvideo = false;
					m_arrObjCam[0].stopVideo();
					play_btn.setText(getResources().getString(R.string.str_play_video));
				} else {
					isvideo = true;
					m_arrViewLive[0].attachCamera(m_arrObjCam[0]);
					m_arrObjCam[0].startVideo();
					play_btn.setText(getResources().getString(R.string.str_stop_video));
				}
				break;

			//PTZ Control
			case R.id.btn_up:
				if(isvideo) m_arrObjCam[0].PTZCtrol(SEP2P_Define.PTZ_CTRL_UP, 0);
				break;

			case R.id.btn_down:
				if(isvideo) m_arrObjCam[0].PTZCtrol(SEP2P_Define.PTZ_CTRL_DOWN, 0);
				break;

			case R.id.btn_left:
				if(isvideo) m_arrObjCam[0].PTZCtrol(SEP2P_Define.PTZ_CTRL_LEFT, 0);
				break;

			case R.id.btn_right:
				if(isvideo) m_arrObjCam[0].PTZCtrol(SEP2P_Define.PTZ_CTRL_RIGHT, 0);
				break;

			// StartAudio
			case R.id.btn_audio: {
				if(isaudio) {
					isaudio = false;
					m_arrObjCam[0].stopAudio();
					audio_btn.setText(getResources().getString(R.string.str_play_audio));
				} else {
					isaudio = true;
					m_arrObjCam[0].startAudio();
					audio_btn.setText(getResources().getString(R.string.str_stop_audio));
				}
				}break;

			//StartTalk
			case R.id.btn_talk:
				if(istalk) {
					istalk = false;
					m_arrObjCam[0].stopTalk();
					talk_btn.setText(getResources().getString(R.string.str_play_talk));
				} else {
					istalk = true;
					m_arrObjCam[0].startTalk();
					talk_btn.setText(getResources().getString(R.string.str_stop_talk));
				}
				break;

			//Snap
			case R.id.btn_snap:
				if(existSdcard()) {
					m_arrObjCam[0].isTakepic = true;
					Toast.makeText(getApplicationContext(),getResources().getString(R.string.ptz_takepic_ok), 3000).show();
				} else {
					Toast.makeText(getApplicationContext(),getResources().getString(R.string.ptz_takepic_save_fail), 3000).show();
				}
				break;

			//Start Record
			case R.id.btn_record:
				if(isrecord){ //stop record
					isrecord = false;
					m_arrObjCam[0].stopRecord();
					Toast.makeText(this, getResources().getString(R.string.stopvideoing), 3000).show();
					record_btn.setText(getResources().getString(R.string.str_play_record));
					
				}else{ //start record
					isrecord = true;
					if(existSdcard()) {
						String strFilename=m_arrObjCam[0].getRecordFilename();
						int nRet=m_arrObjCam[0].startRecord(strFilename);
						if(nRet>=0){
							record_btn.setText(getResources().getString(R.string.str_stop_record));
							Toast.makeText(this, getResources().getString(R.string.videoing), 3000).show();
						}
					} else {
						Toast.makeText(this, getResources().getString(R.string.check_sd_card), 3000).show();
					}
				}break;

			case R.id.btn_other_test:{
				int nRet=m_arrObjCam[0].getVersionInfo();
				System.out.println("getVersionInfo(..)=" + nRet);
				// 1>obtain alarm information
				// if (m_objCam != null)
				// m_objCam.getAlarmInfo();

				// 2>set alarm info
				// m_objCam.setAlarmInfo(1,7,0,0,0,0,0,0,0,0,0,(byte)40);
				// setAlerm();
				
				//3>obtain wifi list
				//m_arrObjCam[0].getWifiList();
				
				//4>get current wifi
				//m_arrObjCam[0].getCurrentwifi();
				
				//5>set current wifi
				/*
				m_arrObjCam[0].setCurrentWifi(wifiBean.getEnable(),
						wifiBean.getSsid(), wifiBean.getMode(),
						wifiBean.getAuthtype(), wifiBean.getEncryp(),
						wifiBean.getKeyformat(), wifiBean.getDefkey(),
						wifiBean.getKey1(), wifiBean.getKey2(), wifiBean.getKey3(),
						wifiBean.getKey4(), wifiBean.getKey1_bits(),
						wifiBean.getKey2_bits(), wifiBean.getKey3_bits(),
						wifiBean.getKey4_bits(), wifiBean.getWpa_psk());
				*/
				//6>get record file by the yyyymmdd
				/*
				 if(m_arrObjCam[0] != null) {
					 int n=m_arrObjCam[0].getRemote_rec_file_by_yyyymmdd(20141226, SEP2P_Define.RECORD_TYPE_PLAN, 0,0);
					 System.out.println("nnnnn---"+n);
				 }
				 */
				
				//7>set camera parameter
				m_arrObjCam[0].setCameraParam(STREAM_HEAD.VIDEO_RESO_QVGA, 0,0,0,0,0,0, SEP2P_Define.BIT_MASK_CAM_PARAM_RESOLUTION,
						1,0,0,"");
				
				/*
				//8>set the datetime of the device by the phone datetime 
				TimeZone objTZ = TimeZone.getDefault();
				String strTZGMTValue= objTZ.getDisplayName(false, TimeZone.SHORT);
				String strTZCityName= objTZ.getID();
				boolean bFoundCityName= false;
				int i=0,nPos=0;
				int nIndexTimeZoneTable=27; //GMT+00:00, begin from 0
				System.out.println("Current TimeZone:1 strTZGMTValue="+strTZGMTValue+",strTZCityName="+strTZCityName);
				//													  GMT+08:00                       Asia/Shanghai
				strTZGMTValue=strTZGMTValue.replace(":", "");
				nPos=strTZGMTValue.indexOf("+");
				if(nPos>=0)	strTZGMTValue=strTZGMTValue.substring(nPos);
				nPos=strTZGMTValue.indexOf("-");
				if(nPos>=0)	strTZGMTValue=strTZGMTValue.substring(nPos);
				System.out.println("Current TimeZone:2 strTZGMTValue="+strTZGMTValue+",strTZCityName="+strTZCityName);
				
				for(i=0; i<X_SERIES_TIMEZONES.length; i++){
					if(X_SERIES_TIMEZONES[i].indexOf(strTZCityName)!=-1) {
						System.out.println("Found city name of timezone,i="+i);
						bFoundCityName = true;
						nIndexTimeZoneTable = i;
						break;
					}
				}
				if(!bFoundCityName){
					for(i=0; i<X_SERIES_TIMEZONES.length; i++) {
						if(X_SERIES_TIMEZONES[i].indexOf(strTZGMTValue)!=-1){
							nIndexTimeZoneTable = i;
							break;
						}
					}
				}
				System.out.println("nIndexTimeZoneTable="+nIndexTimeZoneTable);
				if(m_arrObjCam[0] != null) {
					int nSecTimeZone = objTZ.getRawOffset() / 1000;
					Calendar calendar= Calendar.getInstance();
					int nSecToNow = (int) (calendar.getTimeInMillis() / 1000);
					int nRetDateTime=0;
					nRetDateTime=m_arrObjCam[0].setDateTime(nSecToNow, nSecTimeZone, 1, "time.nist.gov", nIndexTimeZoneTable);
					System.out.println("setDateTime(..)=" + nRetDateTime);
				}
				*/
				}break;

			case R.id.btn_playback:
				System.out.println(m_arrObjCam[0] + ",bStartPlayback=" + bStartPlayback	+ ",m_strFilePlayback=" + m_strFilePlayback);
				// if (m_objCam != null) {
				// if(bStartPlayback){
				// m_objCam.stopPlayback(m_strFilePlayback);
				// m_viewLive.deattachCamera();
				// }else{
				// if(m_strFilePlayback==null) break;
				// bStartPlayback=true;
				// m_viewLive.attachCamera(m_objCam);
				// m_objCam.startPlayback(0, 0, m_strFilePlayback);
				// }
				// }
				// if(m_objCam != null) {
				// int n = m_objCam.getRemote_rec_day_by_yyyymm(201501,1);
				// System.out.println("playback n--"+n);
				// }
				Intent intent = new Intent(MainActivity.this, PlayBackActivity.class);
				startActivity(intent);
				break;

			default:
				break;
		}
	}

	@SuppressWarnings("unused")
	private void setAlerm() {
		if(m_arrObjCam[0] != null) {
			m_arrObjCam[0].setAlarmInfo(alermBean.getMotion_armed(), alermBean.getMotion_sensitivity(),
					alermBean.getInput_armed(), alermBean.getIoin_level(),
					alermBean.getIolinkage(), alermBean.getAlermpresetsit(), alermBean.getMail(),
					alermBean.getSnapshot(), alermBean.getRecord(), alermBean.getFtpsnapshot(),
					alermBean.getFtprecord(), (byte)50, 
					alermBean.getmd_x(), alermBean.getmd_y(),alermBean.getmd_w(),alermBean.getmd_h());
		} else {
			Toast.makeText(MainActivity.this, "set failed", Toast.LENGTH_LONG);
		}
	}

	private boolean existSdcard() {
		if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
			return true;
		} else {
			return false;
		}
	}

	@SuppressLint("HandlerLeak")
	Handler mHandler = new Handler() {
		@SuppressLint("HandlerLeak")
		@Override
		public void handleMessage(Message msg) {
			switch (msg.what) {
				case WHAT_SET_ALARM_FAIL:
					isShowBtn(false);
					break;

				case 1:
					isShowBtn(true);
					break;
				
				case WHAT_ALERMPARAMS:{
					// motion_armed
					byte[] bMDEnable=alermBean.getMotion_armed();
					if(bMDEnable[0] == (byte)0) {
						strMsg = "0 area: close motion armed";
						Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					} else {
						strMsg = "0 area: start motion armed";
						Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					}

					// input_armed
					if(alermBean.getInput_armed() == 0) {
						strMsg = "close input armed";
						Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					} else {
						strMsg = "start input armed";
						Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					}

					// audio_armed
					if(alermBean.getnAudioAlarmSensitivity() == 0) {
						strMsg = "close audio armed";
						Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					} else {
						strMsg = "start audio armed";
						Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					}
				}break;
				
				case WHAT_SCANPARAMS:
					strMsg = "return wifi list" ;
					Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					break;
					
				case WHAT_OVER:
					strMsg="over";
					Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					break;
					
				case WHAT_TIMEOUT:
					strMsg="timeout";
					Toast.makeText(MainActivity.this, strMsg, Toast.LENGTH_LONG).show();
					break;
					
				case WHAT_SEARCH_ON_LAN:{
					String strDID=(String)msg.obj;
					if(strDID!=null && STR_DID.isEmpty()){
						didEdit.setText(strDID);
						STR_DID=strDID;
					}
					}break;
				default:;
			}
			super.handleMessage(msg);
		}
	};

	@Override
	public void onLANSearch(byte[] pData, int nDataSize) {
		SEARCH_RESP objSearchResp = new SEARCH_RESP(pData);
		System.out.println("Searched: DID="+objSearchResp.getDID()+", ip="+objSearchResp.getIpAddr());
		
		Message msg=new Message();
		msg.what=WHAT_SEARCH_ON_LAN;
		msg.obj=objSearchResp.getDID();
		mHandler.sendMessage(msg);
	}

	@SuppressWarnings("static-access")
	protected void doDestroy() {
		m_arrObjCam[0].stopSearchInLAN();
		m_arrObjCam[0].stopRecord();
		// if(m_strFilePlayback!= null)
		// m_objCam.stopPlayback(m_strFilePlayback);
		m_arrObjCam[0].stopTalk();
		m_arrObjCam[0].stopAudio();
		m_arrObjCam[0].stopVideo();
		m_arrObjCam[0].disconnectDev();
		
		CamObj.deinitAPI();
		Intent intent = new Intent();
		intent.setClass(this, CallbackService.class);
		stopService(intent);
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if(keyCode == KeyEvent.KEYCODE_BACK) {
			doDestroy();
		}
		return super.onKeyDown(keyCode, event);
	}

	@Override
	protected void onDestroy() {
		doDestroy();
		super.onDestroy();
	}

	@Override
	public void updateFrameInfo(Object obj, int nWidth, int nHeigh) {
	}

	@Override
	public void updateBmpFrame(Object obj, byte[] rawVideoData, Bitmap bmp) {
	}

	@Override
	public void updateMsg(Object obj, int nMsgType, byte[] pMsg, int nMsgSize, int pUserData) {
		Message msg = MsgHandler.obtainMessage();
		msg.what = nMsgType;
		msg.obj = pMsg;
		MsgHandler.sendMessage(msg);
	}

	Handler MsgHandler = new Handler() {
		@SuppressLint("HandlerLeak")
		public void handleMessage(Message msg) {
			switch (msg.what) {
				case SEP2P_Define.SEP2P_MSG_CONNECT_STATUS:
					int msgParam = m_arrObjCam[0].getStatus();
					@SuppressWarnings("unused")
					String did = m_arrObjCam[0].getDid();
					if(msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_INVALID_ID	
							|| msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_WRONG_USER_PWD){
						m_arrObjCam[0].disconnectDev();
					}else if(msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_CONNECT_TIMEOUT
							|| msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_CONNECT_FAILED
							|| msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_DEVICE_NOT_ONLINE
							|| msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_EXCEED_MAX_USER) {
						m_arrObjCam[0].disconnectDev();
						
					}else if(msgParam == MSG_CONNECT_STATUS.CONNECT_STATUS_CONNECTED){
						connect_btn.setText(getResources().getString(R.string.str_discon));
						isconnect = true;
						isShowBtn(isconnect);
					}
					break;

				case SEP2P_Define.SEP2P_MSG_GET_ALARM_INFO_RESP: {
					if(msg.obj == null) break;
					byte[] byts = (byte[]) msg.obj;
					MSG_GET_ALARM_INFO_RESP alarm_info_resp = new MSG_GET_ALARM_INFO_RESP();
					alarm_info_resp.setData(byts);
					alermBean.setDid(STR_DID);

					System.out.println("alarm_info_resp MDEnable="
							+ alarm_info_resp.get_md_alarm(0)+","
							+ alarm_info_resp.get_md_alarm(1)+","
							+ alarm_info_resp.get_md_alarm(2)+","
							+ alarm_info_resp.get_md_alarm(3)+","
							+ "getnMDSensitivity="
							+ alarm_info_resp.get_md_sensitivity(0)+","
							+ alarm_info_resp.get_md_sensitivity(1)+","
							+ alarm_info_resp.get_md_sensitivity(2)+","
							+ alarm_info_resp.get_md_sensitivity(3));
					System.out.println("bInputAlarm=" + alarm_info_resp.is_io_in_alarm()
							+ "IOLinkageWhenAlarm=" + alarm_info_resp.is_io_out());
					System.out.println("bMailWhenAlarm: " + alarm_info_resp.is_pic_to_email());
					System.out.println("alermBean.getnAudioAlarmSensitivity: "
							+ alarm_info_resp.get_audio_sensitivity());
					
					alermBean.setMotion_armed0(alarm_info_resp.get_md_alarm(0)?(byte)1:(byte)0);
					alermBean.setMotion_armed1(alarm_info_resp.get_md_alarm(1)?(byte)1:(byte)0);
					alermBean.setMotion_armed2(alarm_info_resp.get_md_alarm(2)?(byte)1:(byte)0);
					alermBean.setMotion_armed3(alarm_info_resp.get_md_alarm(3)?(byte)1:(byte)0);
					
					alermBean.setMotion_sensitivity0((byte)alarm_info_resp.get_md_sensitivity(0));
					alermBean.setMotion_sensitivity1((byte)alarm_info_resp.get_md_sensitivity(1));
					alermBean.setMotion_sensitivity2((byte)alarm_info_resp.get_md_sensitivity(2));
					alermBean.setMotion_sensitivity3((byte)alarm_info_resp.get_md_sensitivity(3));
					
					alermBean.setmd_x(alarm_info_resp.get_md_x(0), alarm_info_resp.get_md_x(1),alarm_info_resp.get_md_x(2),alarm_info_resp.get_md_x(3));
					alermBean.setmd_y(alarm_info_resp.get_md_y(0), alarm_info_resp.get_md_y(1),alarm_info_resp.get_md_y(2),alarm_info_resp.get_md_y(3));
					alermBean.setmd_w(alarm_info_resp.get_md_width(0), alarm_info_resp.get_md_width(1),alarm_info_resp.get_md_width(2),alarm_info_resp.get_md_width(3));
					alermBean.setmd_h(alarm_info_resp.get_md_height(0),alarm_info_resp.get_md_height(1),alarm_info_resp.get_md_height(2),alarm_info_resp.get_md_height(3));
					
					alermBean.setInput_armed(alarm_info_resp.is_io_in_alarm()?1:0);
					alermBean.setIoin_level(alarm_info_resp.is_io_in_level()?1:0);
					alermBean.setIolinkage(alarm_info_resp.is_io_out()?1:0);
					if(alarm_info_resp.is_presetbit_alarm()) alermBean.setAlermpresetsit(alarm_info_resp.get_presetbit());
					else alermBean.setAlermpresetsit(0);
					alermBean.setMail(alarm_info_resp.is_pic_to_email()?1:0);
					alermBean.setSnapshot(alarm_info_resp.is_pic_to_sd()?1:0);
					alermBean.setRecord(alarm_info_resp.is_record_to_sd()?1:0);
					alermBean.setFtpsnapshot(alarm_info_resp.is_pic_to_ftp()?1:0);
					alermBean.setFtprecord(alarm_info_resp.is_record_to_ftp()?1:0);
					alermBean.setnAudioAlarmSensitivity(alarm_info_resp.get_audio_sensitivity());
					alermBean.setUpload_interval(15);

					mHandler.sendEmptyMessage(WHAT_ALERMPARAMS);
					}break;

				case SEP2P_Define.SEP2P_MSG_SET_ALARM_INFO_RESP: {
					if(msg.obj == null) break;
					byte[] byts = (byte[]) msg.obj;
					if(byts != null) {
						if(byts[0] == 0) mHandler.sendEmptyMessage(WHAT_SET_ALARM_SUCCESS);
						else mHandler.sendEmptyMessage(WHAT_SET_ALARM_FAIL);
					}
					}break;
			
			  case SEP2P_Define.SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP:{
					  if(msg.obj==null) break; byte[] byts=(byte[])msg.obj;
					  MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP stResp=new
					  MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP(byts); stResp.getnResult();
					  stResp.getnFileTotalNum();
					  System.out.println("result="+stResp.getnResult()+",TotalNum="+stResp.getnFileTotalNum()+",["+
							  			 stResp.getnBeginNoOfThisTime()+","+stResp.getnEndNoOfThisTime()+"]"); 
					  REC_FILE_INFO stRecFileInfo=null; String str=null; 
					  int i=0, nFileNumOfThisTime=stResp.getnEndNoOfThisTime()-stResp.getnBeginNoOfThisTime()+1; 
					  for(i=0; i<nFileNumOfThisTime; i++){
						  stRecFileInfo = new REC_FILE_INFO(byts, MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP.MY_LEN+i * REC_FILE_INFO.MY_LEN);
						  str=String.format("%d, %d %dsec %dKB, %s", i, 
							  			stRecFileInfo.getchnRecType(), stRecFileInfo.getnTimeLen_sec(),
							  			stRecFileInfo.getnFileSize_KB(), stRecFileInfo.getchFilePath());
						  System.out.println(str); 
					  }
					  if(i==0)  m_strFilePlayback=stRecFileInfo.getchFilePath();//for test
			  		}break;
			
			  case SEP2P_Define.SEP2P_MSG_START_PLAY_REC_FILE_RESP:{
				  if(msg.obj==null) break; 
				  byte[] byts=(byte[])msg.obj; 
				  String str=null; 
				  MSG_START_PLAY_REC_FILE_RESP stResp=new MSG_START_PLAY_REC_FILE_RESP(byts);
				  str=String.format("result=%d aparam=%d file=%s", stResp.getnResult(), stResp.getnAudioParam(),
				  stResp.getchFilePath()); System.out.println(str); 
				  }break;
			
			  case SEP2P_Define.SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP:{
				  if(msg.obj==null) break; 
				  byte[] byts=(byte[])msg.obj;
				  MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP day_by_month = new MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP(byts);
				  System.out.println("MainActivity GetRecDay] "+day_by_month.getnChDay()); 
				  }break;
			 
				case SEP2P_Define.SEP2P_MSG_GET_WIFI_LIST_RESP:{
					if(msg.obj == null) break;
					byte[] byts = (byte[]) msg.obj;
					byte[] byt_nResultCount = new byte[4];
					byte[] byt_result_wifi_info = new byte[SEP2P_RESULT_WIFI_INFO.MY_LEN];
					System.arraycopy(byts, 0, byt_nResultCount, 0, byt_nResultCount.length);
					int total_count = Convert.byteArrayToInt_Little(byt_nResultCount);
					for (int i = 0; i < total_count; i++) {
						System.arraycopy(byts, 4 + SEP2P_RESULT_WIFI_INFO.MY_LEN * i, byt_result_wifi_info, 0, byt_result_wifi_info.length);
						SEP2P_RESULT_WIFI_INFO wifi_info = new SEP2P_RESULT_WIFI_INFO(byt_result_wifi_info);
						WifiScanBean bean = new WifiScanBean();
						bean.setDid(STR_DID);
						bean.setSsid(wifi_info.getSSID());
						bean.setSecurity(wifi_info.getAuthType());
						bean.setDbm0(Integer.parseInt(wifi_info.getdbmo()));
						bean.setMac(wifi_info.getMac());
						bean.setMode(wifi_info.getMode());
						bean.setDbm1(Integer.parseInt(wifi_info.getdbm1()));
						System.out.println("obtain wifilist did="+STR_DID+",ssid,["+wifi_info.getSSID()+"],authType,["+wifi_info.getAuthType()
								+"],mac,["+wifi_info.getMac()+"],mode,["+wifi_info.getMode()+"],dbmo,["+wifi_info.getdbmo()+"],dbm1,["+wifi_info.getdbm1()+"]");
					}
					mHandler.sendEmptyMessage(WHAT_SCANPARAMS);
				}
				break;
					
				case SEP2P_Define.SEP2P_MSG_GET_CURRENT_WIFI_RESP:{
					if(msg.obj == null) break;
					byte[] byts = (byte[]) msg.obj;
					wifiBean = new WifiBean();
				    MSG_GET_CURRENT_WIFI_RESP get_current_wifi = new MSG_GET_CURRENT_WIFI_RESP(byts);
					wifiBean.setDid(STR_DID);
					wifiBean.setEnable(1);// enable在设置时一定是1
					wifiBean.setSsid(get_current_wifi.getChSSID());
					wifiBean.setChannel(0);
					wifiBean.setMode(0);// 0
					wifiBean.setAuthtype(get_current_wifi.getnAuthtype());// security
																				// 无--不输密码
					wifiBean.setEncryp(0);// 0
					wifiBean.setKeyformat(0);// 0
					wifiBean.setDefkey(0);// 0
					wifiBean.setKey1(get_current_wifi.getChWEPKey1());// ""wep
					wifiBean.setKey2("");// ""
					wifiBean.setKey3("");// ""
					wifiBean.setKey4("");// ""
					wifiBean.setKey1_bits(0);// 0
					wifiBean.setKey2_bits(0);// 0
					wifiBean.setKey3_bits(0);// 0
					wifiBean.setKey4_bits(0);// 0
					wifiBean.setWpa_psk(get_current_wifi.getChWPAPsk());// 其他
					System.out.println("get currentwifi did="+STR_DID+",ssid,["+get_current_wifi.getChSSID()+"],nAuthtype["+get_current_wifi.getnAuthtype()+"],WepKey["+get_current_wifi.getChWEPKey1()+"],wpa psk["+get_current_wifi.getChWPAPsk()+"]");
					mHandler.sendEmptyMessage(WHAT_WIFIPARAMS);
					}
					break;
					
				case SEP2P_Define.SEP2P_MSG_SET_CURRENT_WIFI_RESP:{
					if(msg.obj == null) break;
					byte[] byts = (byte[]) msg.obj;
					if(byts != null) {
						if(byts[0] == 0)  mHandler.sendEmptyMessage(WHAT_OVER);
						else  mHandler.sendEmptyMessage(WHAT_TIMEOUT);
					}
					}break;
			}
		}
	};

	
	// receive alarm event
	@Override
	public void onEvent(String pDID, int nEventType, int pUserData) {
		if(!STR_DID.equals(pDID)) return;
		switch (nEventType) {
			case SEP2P_Define.EVENT_TYPE_MOTION_ALARM:
				strMsg = "EVENT_TYPE_MOTION_ALARM";
				System.out.println(strMsg);
				break;
			case SEP2P_Define.EVENT_TYPE_INPUT_ALARM:
				strMsg = "EVENT_TYPE_INPUT_ALARM";
				System.out.println(strMsg);
				break;
			case SEP2P_Define.EVENT_TYPE_AUDIO_ALARM:
				strMsg = "EVENT_TYPE_AUDIO_ALARM";
				System.out.println(strMsg);
				break;
			default:
				;
		}
	}
}
