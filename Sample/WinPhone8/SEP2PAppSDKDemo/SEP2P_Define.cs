using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;

namespace SEP2P_AppSDK.SEP2P_Define
{
    ///////////////////////////////////////////////////////////////////////////////////////
	//         AppSDK Defined Error Value
	//
	///////////////////////////////////////////////////////////////////////////////////////
    public static class SEP2P_Error{
		public const int ERR_SEP2P_SUCCESSFUL			    = 0;

		public const int ERR_SEP2P_NOT_INITIALIZED		    = -1;
		public const int ERR_SEP2P_ALREADY_INITIALIZED	    = -2;
		public const int ERR_SEP2P_TIME_OUT				    = -3;
		public const int ERR_SEP2P_INVALID_ID			    = -4;
		public const int ERR_SEP2P_INVALID_PARAMETER		= -5;
		public const int ERR_SEP2P_DEVICE_NOT_ONLINE		= -6;
		public const int ERR_SEP2P_FAIL_TO_RESOLVE_NAME	    = -7;
		public const int ERR_SEP2P_INVALID_PREFIX		    = -8;
		public const int ERR_SEP2P_ID_OUT_OF_DATE		    = -9;
		public const int ERR_SEP2P_NO_RELAY_SERVER_AVAILABLE= -10;
		public const int ERR_SEP2P_INVALID_SESSION_HANDLE	= -11;
		public const int ERR_SEP2P_SESSION_CLOSED_REMOTE	= -12;
		public const int ERR_SEP2P_SESSION_CLOSED_TIMEOUT	= -13;
		public const int ERR_SEP2P_SESSION_CLOSED_CALLED	= -14;
		public const int ERR_SEP2P_REMOTE_SITE_BUFFER_FULL	= -15;
		public const int ERR_SEP2P_USER_LISTEN_BREAK		= -16;
		public const int ERR_SEP2P_MAX_SESSION				= -17;
		public const int ERR_SEP2P_UDP_PORT_BIND_FAILED		= -18;
		public const int ERR_SEP2P_USER_CONNECT_BREAK		= -19;
		public const int ERR_SEP2P_SESSION_CLOSED_INSUFFICIENT_MEMORY = -20;
		public const int ERR_SEP2P_INVALID_APILICENSE		= -21;
		public const int ERR_SEP2P_FAIL_TO_CREATE_THREAD	= -22;

		public const int ERR_SEP2P_EXCEED_MAX_CONNECT_NUM	= -200;
		public const int ERR_SEP2P_ALREADY_CONNECTED		= -201;
		public const int ERR_SEP2P_INVALID_MSG_TYPE			= -202;
		public const int ERR_SEP2P_NO_CONNECT_THIS_DID		= -203;
		public const int ERR_SEP2P_NO_SUPPORT_THIS_CODECID	= -204;
		public const int ERR_SEP2P_NO_SUPPORT_THIS_RESO		= -205;
		public const int ERR_SEP2P_FIRST_START_VIDEO		= -206;
		public const int ERR_SEP2P_WRITTEN_SIZE_TOO_BIG		= -207;
		public const int ERR_SEP2P_STOPPED_TALK				= -208;
	};

    ///////////////////////////////////////////////////////////////////////////////////////
    //         AppSDK defined STREAM_HEAD and some constants
    //
    ///////////////////////////////////////////////////////////////////////////////////////
	public static class SEP2P_ENUM_AV_CODECID
	{
		public const int AV_CODECID_UNKNOWN     = 0x00;
		public const int AV_CODECID_VIDEO_MJPEG = 0x01;
		public const int AV_CODECID_VIDEO_H264  = 0x02;

		public const int AV_CODECID_AUDIO_ADPCM = 0x100;
		public const int AV_CODECID_AUDIO_G726  = 0x101;
		public const int AV_CODECID_AUDIO_G711A = 0x102;
		public const int AV_CODECID_AUDIO_AAC   = 0x103;
	};

	public static class SEP2P_ENUM_VIDEO_FRAME
	{
		public const int VIDEO_FRAME_FLAG_I = 0x00;	// Video I Frame
		public const int VIDEO_FRAME_FLAG_P = 0x01;	// Video P Frame
		public const int VIDEO_FRAME_FLAG_B = 0x02;	// Video B Frame
	};

	public static class SEP2P_ENUM_VIDEO_RESO
	{
		public const int VIDEO_RESO_QQVGA= 0x00; //160*120
		public const int VIDEO_RESO_QVGA = 0x01;	//320*240
		public const int VIDEO_RESO_VGA1 = 0x02;	//640*480
		public const int VIDEO_RESO_VGA2 = 0x03;	//640*360
		public const int VIDEO_RESO_720P = 0x04;	//1280*720
		public const int VIDEO_RESO_960P = 0x05;	//1280*960
		public const int VIDEO_RESO_1080P= 0x06;	//1920*1080 //added on 2014-10-30, for x series
		public const int VIDEO_RESO_1296P= 0x07;	//2304*1296 //added on 2015-06-08, for x series

		public const int VIDEO_RESO_UNKNOWN = 0xFF;
	};

	public static class SEP2P_ENUM_AUDIO_SAMPLERATE
	{
		public const int AUDIO_SAMPLE_8K  = 0x00;
		public const int AUDIO_SAMPLE_11K = 0x01;
		public const int AUDIO_SAMPLE_12K = 0x02;
		public const int AUDIO_SAMPLE_16K = 0x03;
		public const int AUDIO_SAMPLE_22K = 0x04;
		public const int AUDIO_SAMPLE_24K = 0x05;
		public const int AUDIO_SAMPLE_32K = 0x06;
		public const int AUDIO_SAMPLE_44K = 0x07;
		public const int AUDIO_SAMPLE_48K = 0x08;
	};

	public static class SEP2P_ENUM_AUDIO_DATABITS
	{
		public const int AUDIO_DATABITS_8 = 0;
		public const int AUDIO_DATABITS_16= 1;
	};

	public static class SEP2P_ENUM_AUDIO_CHANNEL
	{
		public const int AUDIO_CHANNEL_MONO	= 0;
		public const int AUDIO_CHANNEL_STERO= 1;
	};

    public class STREAM_HEAD
	{
        public const int MY_LEN=24;
        //{
        //    UINT32 nCodecID;		// refer to SEP2P_ENUM_AV_CODECID
        //    CHAR   nParameter;	// Video: refer to SEP2P_ENUM_VIDEO_FRAME.   Audio:(samplerate << 2) | (databits << 1) | (channel), samplerate refer to SEP2P_ENUM_AUDIO_SAMPLERATE; databits refer to SEP2P_ENUM_AUDIO_DATABITS; channel refer to SEP2P_ENUM_AUDIO_CHANNEL
        //    CHAR   nLivePlayback;	// Video: 0:live video or audio;  1:playback video or audio
		//	  UCHAR  nChannel;		// begin from 0
        //    CHAR   reserve1;

        //    UINT32 nStreamDataLen;	// Stream data size after following struct 'STREAM_HEAD'
        //    UINT32 nTimestamp;		// Timestamp of the frame, in milliseconds
        //    UCHAR  nNumConnected;		// amount that app connected this device for M,X series when nCodecID is AV_CODECID_VIDEO...
        //    UCHAR  nNumLiveView;		// amount that app is at liveview UI for M,X series when nCodecID is AV_CODECID_VIDEO...
        //    CHAR   reserve2[2];
        //    UINT32 nPlaybackID;		// reserve2[2,5] -> nPlaybackID, modified on 20141201
        //}
        public STREAM_HEAD(byte[] arrData){
            if (arrData == null || arrData.Length < MY_LEN){
                nCodecID = 0;
                nParameter = 0;
                nLivePlayback = 0;
				nChannel=0;
                nStreamDataLen=0;
                nTimestamp = 0;
                nNumConnected = 0;
                nNumLiveView = 0;
                nPlaybackID = 0;
            }else{
                nCodecID = (UInt32)BitConverter.ToInt32(arrData, 0);
                nParameter = arrData[4];
                nLivePlayback = arrData[5];
				nChannel =arrData[6];
                nStreamDataLen=(UInt32)BitConverter.ToInt32(arrData, 8);
                nTimestamp = (UInt32)BitConverter.ToInt32(arrData, 12);
                nNumConnected = arrData[16];
                nNumLiveView = arrData[17];
                nPlaybackID = (UInt32)BitConverter.ToInt32(arrData, 20);
            }
        }
        //Property---------------------------------------------------------
        public UInt32 nCodecID  { get;set;}
        public Byte   nParameter{ get;set;}
        public Byte   nLivePlayback    { get;set;}
        public Byte   nChannel		   { get;set;}

        public UInt32 nStreamDataLen   { get;set;}
        public UInt32 nTimestamp       { get;set;}
        public Byte   nNumConnected    { get;set;}
        public Byte   nNumLiveView     { get;set;}
        public UInt32 nPlaybackID      { get;set;}
	};

    public static class SEP2P_ENUM_EVENT_TYPE
	{
		public const int EVENT_TYPE_UNKNOWN		 = 0x00;
		public const int EVENT_TYPE_MOTION_ALARM = 0x01;
		public const int EVENT_TYPE_INPUT_ALARM	 = 0x02;
		public const int EVENT_TYPE_AUDIO_ALARM	 = 0x03;
		public const int EVENT_TYPE_MANUAL_ALARM = 0x04;

		public const int EVENT_TYPE_DOORBELL_ALARM	  = 0x08;
		public const int EVENT_TYPE_TEMPERATURE_ALARM = 0x10;
		public const int EVENT_TYPE_HUMIDITY_ALARM	  = 0x20;
	};

	public class EVENT_DATA
	{
        public const int MY_LEN=16;

		public byte  eDeviceType	{get;set;}
		public byte  nChannel		{get;set;}
		public byte  nTypeNotify	{get;set;}
		public byte[] reserve		{get;set;}//[13];
	};

	///////////////////////////////////////////////////////////////////////////////////////
	//         AppSDK Defined Message Type
	//
	///////////////////////////////////////////////////////////////////////////////////////
	public static class SEP2P_ENUM_MSGTYPE
	{
		public const int SEP2P_MSG_QUERY_CHANNEL_INFO_OF_NVR_REQ	= 0x0096;
		public const int SEP2P_MSG_QUERY_CHANNEL_INFO_OF_NVR_RESP	= 0x0097;

		public const int SEP2P_MSG_CONNECT_STATUS   = 0x0100;
		public const int SEP2P_MSG_CONNECT_MODE     = 0x0101;

		public const int SEP2P_MSG_START_VIDEO      = 0x0110;    // start video
		public const int SEP2P_MSG_STOP_VIDEO       = 0x0111;     // stop video
		public const int SEP2P_MSG_START_AUDIO      = 0x0112;     // start audio
		public const int SEP2P_MSG_STOP_AUDIO       = 0x0113;     // stop audio
		public const int SEP2P_MSG_START_TALK       = 0x0114;     // start talk
		public const int SEP2P_MSG_START_TALK_RESP  = 0x0115;     // start talk response
		public const int SEP2P_MSG_STOP_TALK        = 0x0117;     // stop talk

		public const int SEP2P_MSG_GET_CAMERA_PARAM_REQ     = 0x0120;
		public const int SEP2P_MSG_GET_CAMERA_PARAM_RESP    = 0x0121;
		public const int SEP2P_MSG_SET_CAMERA_PARAM_REQ     = 0x0122;
		public const int SEP2P_MSG_SET_CAMERA_PARAM_RESP    = 0x0123;
		public const int SEP2P_MSG_SET_CAMERA_PARAM_DEFAULT_REQ     = 0x0124;
		public const int SEP2P_MSG_SET_CAMERA_PARAM_DEFAULT_RESP    = 0x0125;
		public const int SEP2P_MSG_PTZ_CONTROL_REQ          = 0x0126;    // yuntai control request
		public const int SEP2P_MSG_PTZ_CONTROL_RESP         = 0x0127;    // yuntai control response
		public const int SEP2P_MSG_SNAP_PICTURE_REQ         = 0x0128;    //
		public const int SEP2P_MSG_SNAP_PICTURE_RESP        = 0x0129;    // return jpg snapshoted

		public const int SEP2P_MSG_GET_CURRENT_WIFI_REQ     = 0x0130;
		public const int SEP2P_MSG_GET_CURRENT_WIFI_RESP    = 0x0131;
		public const int SEP2P_MSG_SET_CURRENT_WIFI_REQ     = 0x0132;
		public const int SEP2P_MSG_SET_CURRENT_WIFI_RESP    = 0x0133;
		public const int SEP2P_MSG_GET_WIFI_LIST_REQ        = 0x0134;
		public const int SEP2P_MSG_GET_WIFI_LIST_RESP       = 0x0135;

		public const int SEP2P_MSG_GET_USER_INFO_REQ        = 0x0140;
		public const int SEP2P_MSG_GET_USER_INFO_RESP       = 0x0141;
		public const int SEP2P_MSG_SET_USER_INFO_REQ        = 0x0142;
		public const int SEP2P_MSG_SET_USER_INFO_RESP       = 0x0143;

		//----{{ X series P2P FW>=1.0.0.43------
		public const int SEP2P_MSG_GET_USER_INFO2_REQ	    = 0x0144;
		public const int SEP2P_MSG_GET_USER_INFO2_RESP		= 0x0145;
		public const int SEP2P_MSG_SET_USER_INFO2_REQ	    = 0x0146;
		public const int SEP2P_MSG_SET_USER_INFO2_RESP		= 0x0147;
		//----}} X series P2P FW>=1.0.0.43------

		public const int SEP2P_MSG_GET_DATETIME_REQ         = 0x0150;
		public const int SEP2P_MSG_GET_DATETIME_RESP        = 0x0151;
		public const int SEP2P_MSG_SET_DATETIME_REQ         = 0x0152;
		public const int SEP2P_MSG_SET_DATETIME_RESP        = 0x0153;

		public const int SEP2P_MSG_GET_FTP_INFO_REQ         = 0x0160;
		public const int SEP2P_MSG_GET_FTP_INFO_RESP        = 0x0161;
		public const int SEP2P_MSG_SET_FTP_INFO_REQ         = 0x0162;
		public const int SEP2P_MSG_SET_FTP_INFO_RESP        = 0x0163;

		public const int SEP2P_MSG_GET_EMAIL_INFO_REQ       = 0x0170;
		public const int SEP2P_MSG_GET_EMAIL_INFO_RESP      = 0x0171;
		public const int SEP2P_MSG_SET_EMAIL_INFO_REQ       = 0x0172;
		public const int SEP2P_MSG_SET_EMAIL_INFO_RESP      = 0x0173;

		public const int SEP2P_MSG_GET_ALARM_INFO_REQ       = 0x0180;
		public const int SEP2P_MSG_GET_ALARM_INFO_RESP      = 0x0181;
		public const int SEP2P_MSG_SET_ALARM_INFO_REQ       = 0x0182;
		public const int SEP2P_MSG_SET_ALARM_INFO_RESP      = 0x0183;

		public const int SEP2P_MSG_ALARM_IO_CTRL_REQ        = 0x0184;
		public const int SEP2P_MSG_ALARM_IO_CTRL_RESP       = 0x0185;

		public const int SEP2P_MSG_GET_UART_CTRL_REQ        = 0x0186;
		public const int SEP2P_MSG_GET_UART_CTRL_RESP       = 0x0187;
		public const int SEP2P_MSG_SET_UART_CTRL_REQ        = 0x0188;
		public const int SEP2P_MSG_SET_UART_CTRL_RESP       = 0x0189;

		public const int SEP2P_MSG_GET_SDCARD_REC_PARAM_REQ = 0x0190;
		public const int SEP2P_MSG_GET_SDCARD_REC_PARAM_RESP= 0x0191;
		public const int SEP2P_MSG_SET_SDCARD_REC_PARAM_REQ = 0x0192;
		public const int SEP2P_MSG_SET_SDCARD_REC_PARAM_RESP= 0x0193;

		public const int SEP2P_MSG_FORMAT_SDCARD_REQ        = 0x0194;
		//public const int SEP2P_MSG_FORMAT_SDCARD_RESP		= 0x0195;

		public const int SEP2P_MSG_REBOOT_DEVICE            = 0x0196;	//valid when firmwareVersion>=0.1.0.21 of M
		//

		public const int SEP2P_MSG_GET_CUSTOM_PARAM_REQ     = 0x0198;	//valid when firmwareVersion>=0.1.0.21 of M
		public const int SEP2P_MSG_GET_CUSTOM_PARAM_RESP    = 0x0199;	//valid when firmwareVersion>=0.1.0.21 of M
		public const int SEP2P_MSG_SET_CUSTOM_PARAM_REQ     = 0x019A;	//valid when firmwareVersion>=0.1.0.21 of M
		public const int SEP2P_MSG_SET_CUSTOM_PARAM_RESP    = 0x019B;	//valid when firmwareVersion>=0.1.0.21 of M

		public const int SEP2P_MSG_GET_DEVICE_VERSION_REQ   = 0x01A0;
		public const int SEP2P_MSG_GET_DEVICE_VERSION_RESP  = 0x01A1;

		public const int SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ  = 0x01A6;	//added on 2014-12-01
		public const int SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP = 0x01A7;	//added on 2014-12-01
		public const int SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ   = 0x01A8;	//added on 2014-12-01
		public const int SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP  = 0x01A9;	//added on 2014-12-01

		public const int SEP2P_MSG_START_PLAY_REC_FILE_REQ          = 0x01AA;	//added on 2014-12-01
		public const int SEP2P_MSG_START_PLAY_REC_FILE_RESP         = 0x01AB;	//added on 2014-12-01
		public const int SEP2P_MSG_STOP_PLAY_REC_FILE_REQ           = 0x01AC;	//added on 2014-12-01
		public const int SEP2P_MSG_STOP_PLAY_REC_FILE_RESP          = 0x01AD;	//added on 2014-12-01

		public const int SEP2P_MSG_GET_REC_DATE_RANGE_REQ           = 0x01AE;	//
		public const int SEP2P_MSG_GET_REC_DATE_RANGE_RESP          = 0x01AF;	//
		public const int SEP2P_MSG_START_PLAY_REC_FILE2_REQ         = 0x01B0;	//
		public const int SEP2P_MSG_START_PLAY_REC_FILE2_RESP        = 0x01B1;	//
		public const int SEP2P_MSG_STOP_PLAY_REC_FILE2_REQ          = 0x01B2;	//
		public const int SEP2P_MSG_STOP_PLAY_REC_FILE2_RESP         = 0x01B3;	//

		public const int SEP2P_MSG_GET_IPUSH_INFO_REQ				= 0x01BA;	//X series, supported on the firmware v1.0.0.14
		public const int SEP2P_MSG_GET_IPUSH_INFO_RESP				= 0x01BB;	//X series, supported on the firmware v1.0.0.14
		public const int SEP2P_MSG_SET_IPUSH_INFO_REQ				= 0x01BC;	//X series, supported on the firmware v1.0.0.14
		public const int SEP2P_MSG_SET_IPUSH_INFO_RESP				= 0x01BD;	//X series, supported on the firmware v1.0.0.14
		
		public const int SEP2P_MSG_RESTORE_FACTORY					= 0x01BE;	//X series, restore to default(factory)

		//user can extend message type from 0x1000 to 0xFFFF
	};

	public static class CONNECT_STATUS{
		public const int CONNECT_STATUS_CONNECTING          =0x00;
		public const int CONNECT_STATUS_INITIALING          =0x01;
		public const int CONNECT_STATUS_ONLINE              =0x02;
		public const int CONNECT_STATUS_CONNECT_FAILED      =0x03;
		public const int CONNECT_STATUS_DISCONNECT          =0x04;
		public const int CONNECT_STATUS_INVALID_ID          =0x05;
		public const int CONNECT_STATUS_DEVICE_NOT_ONLINE   =0x06;
		public const int CONNECT_STATUS_CONNECT_TIMEOUT     =0x07;
		public const int CONNECT_STATUS_WRONG_USER_PWD      =0x08;

		public const int CONNECT_STATUS_INVALID_REQ         =0x09;
		public const int CONNECT_STATUS_EXCEED_MAX_USER     =0x0A;	// exceed the max user
		public const int CONNECT_STATUS_CONNECTED           =0x0B;

		public const int CONNECT_STATUS_UNKNOWN = 0xFFFFFF;
	};

	public static class CONNECT_MODE{
		public const int CONNECT_MODE_P2P   =0x0;
		public const int CONNECT_MODE_RLY   =0x1;

		public const int CONNECT_MODE_UNKNOWN=0x7F;
	};

	public static class ENUM_PTZ_CONTROL_CMD
	{
		public const int PTZ_CTRL_STOP  = 0x00;
		public const int PTZ_CTRL_UP    = 0x01;
		public const int PTZ_CTRL_DOWN  = 0x02;
		public const int PTZ_CTRL_LEFT  = 0x03;
		public const int PTZ_CTRL_RIGHT = 0x04;

		public const int PTZ_CTRL_CRUISE_H = 0x30;
		public const int PTZ_CTRL_CRUISE_V = 0x31;

		public const int PTZ_CTRL_PRESET_BIT_SET    = 0x40;
		public const int PTZ_CTRL_PRESET_BIT_GOTO   = 0x41;
		//public const int PTZ_CTRL_PRESET_BIT_CLEAR= 0x42;
	};

	public static class ENUM_BIT_MASK_CAMERA_PARAM
	{
		public const int BIT_MASK_CAM_PARAM_RESOLUTION  = 0x01;
		public const int BIT_MASK_CAM_PARAM_BRIGHT      = 0x02;
		public const int BIT_MASK_CAM_PARAM_CONTRAST    = 0x04;
		public const int BIT_MASK_CAM_PARAM_HUE         = 0x08;

		public const int BIT_MASK_CAM_PARAM_SATURATION  = 0x10;
		public const int BIT_MASK_CAM_PARAM_OSD         = 0x20;
		public const int BIT_MASK_CAM_PARAM_MODE        = 0x40;
		public const int BIT_MASK_CAM_PARAM_FLIP        = 0x80;
		public const int BIT_MASK_CAM_PARAM_IRLED       = 0x0100;
		public const int BIT_MASK_CAM_PARAM_FPS_KBPS	= 0x0200;
		public const int BIT_MASK_CAM_PARAM_MIC_VOLUME  = 0x0400; //Microphone volume of IPC, 20161214
		public const int BIT_MASK_CAM_PARAM_SPK_VOLUME  = 0x0800; //speaker volume of IPC,20161214
	};

	///////////////////////////////////////////////////////////////////////////////////////
	//         AppSDK Defined Struct
	//
	///////////////////////////////////////////////////////////////////////////////////////
	
	//SEP2P_MSG_QUERY_CHANNEL_INFO_OF_NVR_REQ
	public class MSG_QUERY_CHANNEL_INFO_OF_NVR_REQ
	{
        public const int MY_LEN=16;

		public byte  nChannel	{get;set;}
		public byte[] reserve   {get;set;}//[15];
	};
	
	//SEP2P_MSG_QUERY_CHANNEL_INFO_OF_NVR_RESP
	public class CHANNEL_INFO_OF_NVR
	{
		public const int MY_LEN=16;
		public byte    bExistDevice; //1:exist; 0:not
		//it's value refer to SEP_ENUM_VIDEO_RESO; 
		//resolution index save at chResoIndex[0],chResoIndex[1]... from small to large order. the value 0xFF is over.
		public byte[] chResoIndex;//[7]; 
		public byte[]  reserve;	//[8]
	};
	public class MSG_QUERY_CHANNEL_INFO_OF_NVR_RESP
	{
		public const int MY_LEN=8;

		public byte    result;
		public byte[] reserve;	//[2]
		public byte  nChannel;
		public Int32 nChannelNum;
		//CHANNEL_INFO_OF_NVR[0]
		//CHANNEL_INFO_OF_NVR[1]
		//......
	};

	//SEP2P_MSG_CONNECT_STATUS
	public class MSG_CONNECT_STATUS
	{
        public const int MY_LEN=64;

		public Int32 eConnectStatus {get;set;}
		public Int32 eConnectMode   {get;set;} //refer to CONNECT_MODE
		public byte[] stRemoteAddr  {get;set;} //struct sockaddr_in, Remote site IP:Port
		public byte[] stMyLocalAddr {get;set;} //struct stMyLocalAddr, My Local site IP:Port
		public byte[] stMyWanAddr   {get;set;} //struct stMyWanAddr, My WAN site IP:Port
		public byte[] reserve       {get;set;} //[8];
	};

	//SEP2P_MSG_START_VIDEO
	public class MSG_START_VIDEO
	{
        public const int MY_LEN=16;

		public Int32 nVideoCodecID  {get;set;} //refer to SEP2P_ENUM_AV_CODECID
		public Int32 eVideoReso     {get;set;} //refer to SEP2P_ENUM_VIDEO_RESO
		public byte  nChannel		{get;set;}
		public byte[]  reserve      {get;set;} //[7];
	};

	//SEP2P_MSG_STOP_VIDEO
	public class MSG_STOP_VIDEO
	{
        public const int MY_LEN=16;
		
		public byte nChannel  {get;set;}
	    public byte[]  reserve{get;set;}//[15];
	};

	//SEP2P_MSG_START_AUDIO
	public class MSG_START_AUDIO
	{
        public const int MY_LEN=16;

		public Int32 nAudioCodecID  {get;set;} //refer to SEP2P_ENUM_AV_CODECID
		public byte nChannel		{get;set;}
		public byte[] reserve       {get;set;}//[11];
	};
    
 //SEP2P_MSG_STOP_AUDIO
 public class MSG_STOP_AUDIO
 {
    public const int MY_LEN=16;

	public byte nChannel  {get;set;}
    public byte[] reserve {get;set;}//[15];
 };


 //SEP2P_MSG_START_TALK
 public class MSG_START_TALK
 {
    public const int MY_LEN=16;
	
	public byte nChannel  {get;set;}
    public byte[] reserve {get;set;}//[15];
 };

 //SEP2P_MSG_START_TALK_RESP
 public class MSG_START_TALK_RESP
 {
     public const int MY_LEN=16;

     public byte  result    {get;set;}	//=0 OK; 1:talking; 2:unknown error
	 public byte nChannel   {get;set;}
     public byte[] reserve  {get;set;}//[14];
 };

 //SEP2P_MSG_STOP_TALK
 public class MSG_STOP_TALK
 {
    public const int MY_LEN=16;
	
	public byte nChannel   {get;set;}
    public byte[]  reserve {get;set;}//[15];
 };

 //SEP2P_MSG_GET_CAMERA_PARAM_REQ
 public class MSG_GET_CAMERA_PARAM_REQ
 {
    public const int MY_LEN=16;

    public byte[]  reserve{get;set;}//[16];
 };

 //SEP2P_MSG_GET_CAMERA_PARAM_RESP
 public class MSG_GET_CAMERA_PARAM_RESP
 {
     public const int MY_LEN=116;

     public Int32 nResolution   {get;set;}	//refer to SEP2P_ENUM_VIDEO_RESO. 0->160*120, 1->320*240, 2->640*480, 3->640*360, 4->1280*720, 5->1280*960, 6->1920*1080
     public Int32 nBright       {get;set;}	//[0,255]
     public Int32 nContrast     {get;set;}	//[0,7]
     public Int32 reserve1      {get;set;}
     public Int32 nSaturation   {get;set;}	//[0,255]
     public Int32 bOSD          {get;set;}	//0->disable, 1->enable
     public Int32 nMode         {get;set;}	//0->50Hz, 1->60Hz
     public Int32 nFlip         {get;set;}	//0->normal, 1->flip, 2->mirror, 3->flip & mirror
	 public Int32 nKBPS			{get;set;}	//[1,x],  mask: BIT_MASK_CAM_PARAM_FPS_KBPS, MP2P FW>=v0.1.0.35, XP2P FW>=v1.0.1.7
	 public byte nFPS			{get;set;}	//[1,60], mask: BIT_MASK_CAM_PARAM_FPS_KBPS, MP2P FW>=v0.1.0.35, XP2P FW>=v1.0.1.7
     public byte[]  reserve2    {get;set;}	//[3]
     public byte nIRLed			{get;set;}	   //0->close, 1->open, 2->auto //support getting its value when FirmwareVer>=0.1.0.18 for only M Series
     public byte bHasMIC_SPKVolume  {get;set;} //0-->none, 1-->has mic spk volume function when get
	 public byte nMICVolume			{get;set;} //MIC volume, range is [1,100]
	 public byte nSPKVolume			{get;set;} //speaker volume, range is [1,100]
	 public byte[]  reserve3    {get;set;} //[12]
	 public byte[]  chOSDName   {get;set;} //[60]
 };


 //SEP2P_MSG_SET_CAMERA_PARAM_REQ
 public class MSG_SET_CAMERA_PARAM_REQ
 {
     public const int MY_LEN=116;

     public Int32 nResolution   {get;set;}	//refer to SEP2P_ENUM_VIDEO_RESO. 0->160*120, 1->320*240, 2->640*480, 3->640*360, 4->1280*720, 5->1280*960, 6->1920*1080
     public Int32 nBright       {get;set;}	//[0,255]
     public Int32 nContrast     {get;set;}	//[0,7]
     public Int32 reserve1      {get;set;}
     public Int32 nSaturation   {get;set;}	//[0,255]
     public Int32 bOSD          {get;set;}	//0->disable, 1->enable
     public Int32 nMode         {get;set;}	//0->50Hz, 1->60Hz
     public Int32 nFlip         {get;set;}	//0->normal, 1->flip, 2->mirror, 3->flip & mirror
	 public Int32 nKBPS			{get;set;}	//[1,x],  mask: BIT_MASK_CAM_PARAM_FPS_KBPS, MP2P FW>=v0.1.0.35, XP2P FW>=v1.0.1.7
	 public byte nFPS			{get;set;}	//[1,60], mask: BIT_MASK_CAM_PARAM_FPS_KBPS, MP2P FW>=v0.1.0.35, XP2P FW>=v1.0.1.7
     public byte[]  reserve2    {get;set;}	//[3]
     public byte nIRLed			{get;set;}	   //0->close, 1->open, 2->auto //support getting its value when FirmwareVer>=0.1.0.18 for only M Series
     public byte bHasMIC_SPKVolume  {get;set;} //0-->none, 1-->has mic spk volume function when get
	 public byte nMICVolume			{get;set;} //MIC volume, range is [1,100]
	 public byte nSPKVolume			{get;set;} //speaker volume, range is [1,100]
	 public byte[]  reserve3    {get;set;} //[8]
	 
     public Int32 nBitMaskToSet {get;set;}  //refer to ENUM_BIT_MASK_CAMERA_PARAM
                         //Bit0:=1->set resolution; Bit1:=1->set bright; Bit2:=1->set contrast; Bit3:=1->set hue;
                         //Bit4:=1->set saturation; Bit5:=1->set OSD;	 Bit6:=1->set mode;		Bit7:=1->set flip; Bit8:=1->set infrared led
	 public byte[]  chOSDName   {get;set;} //[60]
 };

 //SEP2P_MSG_SET_CAMERA_PARAM_RESP
public class MSG_SET_CAMERA_PARAM_RESP
 {
     public const int MY_LEN=16;

     public byte   result       {get;set;}	//0: OK; or not fail
     public byte[] reserve      {get;set;}  //[11];
     public Int32  nBitMaskToSet{get;set;}
 };

 //SEP2P_MSG_SET_CAMERA_PARAM_DEFAULT_REQ
public class MSG_SET_CAMERA_PARAM_DEFAULT_REQ //Change brightness and contrast to default value, no flip and no mirror
 {
    public const int MY_LEN=16;

    public byte[] reserve{get;set;}//[16];
 };

 //SEP2P_MSG_SET_CAMERA_PARAM_DEFAULT_RESP
public class MSG_SET_CAMERA_PARAM_DEFAULT_RESP
 {
     public const int MY_LEN=16;

     public byte   result   {get;set;}	//0: OK; or not fail
     public byte[] reserve  {get;set;}//[15];
 };


 //SEP2P_MSG_PTZ_CONTROL_REQ
public class MSG_PTZ_CONTROL_REQ
 {
     public const int MY_LEN=16;

     public byte nCtrlCmd   {get;set;}	// PTZ control command, refer to ENUM_PTZ_CONTROL_CMD
     public byte nCtrlParam {get;set;}	// for PTZ_CTRL_PRESET_BIT_...: nCtrlParam is SET/GOTO/CLEAR preset bit no. range is [0,15]
										// for PTZ_CTRL_UP, PTZ_CTRL_DOWN, PTZ_CTRL_LEFT, PTZ_CTRL_RIGHT: 1=continue after one step; 0=one step

	 public byte nChannel   {get;set;}
     public byte reserve    {get;set;}//[13]
 };

 //SEP2P_MSG_PTZ_CONTROL_RESP
 public class MSG_PTZ_CONTROL_RESP
 {
     public const int MY_LEN=16;

     public byte   result {get;set;}	//0: OK; or not fail
	 public byte nChannel {get;set;}
     public byte[] reserve{get;set;}//[14];
 };

 //SEP2P_MSG_SNAP_PICTURE_REQ
 public class MSG_SNAP_PICTURE_REQ
 {
    public const int MY_LEN=16;

    public byte[] reserve{get;set;}//[16];
 };

 //SEP2P_MSG_SNAP_PICTURE_RESP
 //typedef INT32 (* POnRecvMsgCallback)(CHAR*   pDID, UINT32  nMsgType, CHAR* pMsg, UINT32  nMsgSize, VOID* pUserData);
 //nMsgType=SEP2P_MSG_SNAP_PICTURE_RESP
 //pMsg	  =jpg data snapshoted
 //nMsgSize=the size of jpg data snapshoted

 //SEP2P_MSG_GET_CURRENT_WIFI_REQ
 public class MSG_GET_CURRENT_WIFI_REQ
 {
    public const int MY_LEN=16;

    public byte[] reserve{get;set;}//[16];
 };

 //SEP2P_MSG_GET_CURRENT_WIFI_RESP
 public class MSG_GET_CURRENT_WIFI_RESP
 {
     public const int MY_LEN=812;

     public Int32  bEnable      {get;set;}   //0->disable Wi-Fi, 1->enable Wi-Fi
     public byte[] chSSID       {get;set;}//[128]; //WiFi SSID name
     public byte   nChannel     {get;set;}//no use
	 public byte   bReqTestWifiResult  {get;set;} //0->don't request testing wifi result; 1->request
	 public byte[] reserve      {get;set;} //[2]
     public Int32  nMode        {get;set;}	//0->infra; 1->adhoc
     public Int32  nAuthtype    {get;set;}  //0->WEP-NONE; 1->WEP; 2->WPA-PSK AES; 3->WPA-PSK TKIP; 4->WPA2-PSK AES; 5->WPA2-PSK TKIP
     public Int32  nWEPEncrypt  {get;set;}	//WEP: 0->open; 1->share key
     public Int32  nWEPKeyFormat{get;set;}  //WEP: 0->hexadecimal number; 1->ASCII Character
     public Int32  nWEPDefaultKey{get;set;} //WEP: [0,3]
     public byte[] chWEPKey1    {get;set;}//[128] //WEP: WEP key1
     public byte[] chWEPKey2    {get;set;}//[128] //WEP: WEP key2
     public byte[] chWEPKey3    {get;set;}//[128] //WEP: WEP key3
     public byte[] chWEPKey4    {get;set;}//[128] //WEP: WEP key4
     public Int32  nWEPKey1_bits{get;set;}  //WEP: 0->64bits, 1->128bits for WEP key1
     public Int32  nWEPKey2_bits{get;set;}  //WEP: 0->64bits, 1->128bits for WEP key2
     public Int32  nWEPKey3_bits{get;set;}  //WEP: 0->64bits, 1->128bits for WEP key3
     public Int32  nWEPKey4_bits{get;set;}  //WEP: 0->64bits, 1->128bits for WEP key4
     public byte[] chWPAPsk     {get;set;}//[128]  //WPA: share key
 };

 //SEP2P_MSG_SET_CURRENT_WIFI_REQ
 public class MSG_SET_CURRENT_WIFI_REQ:MSG_GET_CURRENT_WIFI_RESP{} //L: the device will reboot after completing settings
 
 //SEP2P_MSG_SET_CURRENT_WIFI_RESP
 public class MSG_SET_CURRENT_WIFI_RESP
 {
     public const int MY_LEN=16;

     public byte   result   {get;set;}	//0: OK; or not fail
     public byte[] reserve  {get;set;}//[15];
 };

 //SEP2P_MSG_GET_WIFI_LIST_REQ
 public class MSG_GET_WIFI_LIST_REQ
 {
    public const int MY_LEN=16;

    public byte[] reserve   {get;set;}//[16];
 };

 public class SEP2P_RESULT_WIFI_INFO
 {
     public const int MY_LEN=204;

     public byte[]   chSSID {get;set;}//[64]; //Wi-Fi SSID name
     public byte[]   chMAC  {get;set;}//[64];
     public Int32  nAuthtype{get;set;}//0->WEP-NONE, 1->WEP, 2->WPA-PSK AES, 3->WPA-PSK TKIP, 4->WPA2-PSK AES, 5->WPA2-PSK TKIP

     public byte[] dbm0     {get;set;}//[32];//'80' sign level.
     public byte[] dbm1     {get;set;}//[32];//'100'percent. it is always 100.
     public Int32  nMode;	//0->infra 1->adhoc
     public Int32  reserve;
 };

 //SEP2P_MSG_GET_WIFI_LIST_RESP
 public class MSG_GET_WIFI_LIST_RESP
 {
     public Int32 nResultCount      {get;set;}
     SEP2P_RESULT_WIFI_INFO[] wifi  {get;set;}//[50];
 };

 //SEP2P_MSG_GET_USER_INFO_REQ
 public class MSG_GET_USER_INFO_REQ
 {
    public const int MY_LEN=16;
    public byte[]  reserve {get;set;}//[16];
 };

 //SEP2P_MSG_GET_USER_INFO_RESP
 public class MSG_GET_USER_INFO_RESP
 {
     public const int MY_LEN=392;

     public byte[] chVisitor     {get;set;}//[64]; //visitor
     public byte[] chVisitorPwd  {get;set;}//[64];
     public byte[] chCurUser     {get;set;}//[64];
	 public byte[] chCurUserPwd  {get;set;}//[64];
     public byte[] chAdmin       {get;set;}//[64];	//admin
     public byte[] chAdminPwd    {get;set;}//[64];
	 public byte  nCurUserRoleID;	//only get it for X series, 2015-04-16
	 public byte[] reserve		 {get;set;}//[7]
 };

 //SEP2P_MSG_SET_USER_INFO_REQ
 public class MSG_SET_USER_INFO_REQ:MSG_GET_USER_INFO_RESP{};

 //SEP2P_MSG_SET_USER_INFO_RESP
 public class MSG_SET_USER_INFO_RESP
 {
     public const int MY_LEN=16;

     public byte   result;	//0: OK; or not fail
     public byte[] reserve{get;set;}//[15];
 };


#define MAX_NUM_USER_INFO	8
//SEP2P_MSG_GET_USER_INFO2_REQ
//- (no data body)

//SEP2P_MSG_GET_USER_INFO2_RESP
public static class E_ROLE_ID
{
	public const int ROLE_ID_ADMIN  = 0x01;
	public const int ROLE_ID_USER   = 0x02;
	public const int ROLE_ID_GUEST  = 0x03;
};

public static class E_OPCODE_USER_INFO
{
	public const int OP_USER_INFO_ADD  = 0x01;
	public const int OP_USER_INFO_DEL  = 0x02;
	public const int OP_USER_INFO_EDIT = 0x03;
};

public class USER_INFO
{
     public const int MY_LEN=72;

     public byte[] chUsername {get;set;} //32
	 public byte[] chUserPwd  {get;set;} //32
	 public Int32 nUserRoleID {get;set;}
	 public Int32 nUserID	  {get;set;}
};
public class MSG_GET_USER_INFO2_RESP
{
	public const int MY_LEN=8+72*MAX_NUM_USER_INFO;

	public byte nUserName		{get;set;}
	public byte nCurUserRoleID	{get;set;}
	public byte bCanAddDel		{get;set;}
	public byte reserve;
	public Int32 nCurUserID		{get;set;}
	USER_INFO[] arrUserInfo		{get;set;}//[MAX_NUM_USER_INFO];
}

//SEP2P_MSG_SET_USER_INFO2_REQ
//the user of nUserID=10001 can NOT change role id and delete it!!
public class MSG_SET_USER_INFO2_REQ
{
	public const int MY_LEN=8;

	public byte nOpUserNum  {get;set;}
	public byte nOpCode		{get;set;}
	public byte[] reserve;	//6
	USER_INFO[] arrUserInfo  {get;set;}//[MAX_NUM_USER_INFO];
}

//SEP2P_MSG_SET_USER_INFO2_RESP
public class MSG_SET_USER_INFO2_RESP
{
	public const int MY_LEN=8;

	public byte result	   {get;set;} //0: OK; or not fail, 1:unknown error, 2:wrong MSG_SET_USER_INFO2_REQ info, 3:permission denied
	public byte nOpUserNum  {get;set;}
	public byte nOpCode		{get;set;}
	public byte[] reserve;	//13
	public Int32[] nUserID   {get;set;} //MAX_NUM_USER_INFO
}

 //SEP2P_MSG_GET_DATETIME_REQ
 public class MSG_GET_DATETIME_REQ
 {
    public const int MY_LEN=16;
    public byte[] reserve{get;set;}//[16];
 };


 //@struct MSG_GET_DATETIME_RESP
 //@abstract MSG_GET_DATETIME_RESP define the struct that the response of send a get datetime command..
 //@discussion The current device timezone supported is:
 //{"Pacific/Apia",		GMT-11.00},
 //{"Pacific/Honolulu",	GMT-10.00},
 //{"America/Anchorage",	GMT-09.00},
 //{"America/Los_Angeles",GMT-08.00},
 //{"America/Denver",	    GMT-07.00},
 //{"America/Chicago",	GMT-06.00},
 //{"America/New_York",	GMT-05.00},
 //{"America/Montreal",	GMT-04.00},
 //{"America/St_Johns",	GMT-03.30},
 //{"America/Thule",	    GMT-03.00},
 //{"Atlantic/South_Georgia",	GMT-02.00},
 //{"Atlantic/Cape_Verde",	GMT-01.00},
 //{"Europe/Dublin",			GMT00.00},
 //{"Europe/Brussels",	    GMT01.00},
 //{"Europe/Athens",			GMT02.00},
 //{"Asia/Kuwait",		    GMT03.00},
 //{"Asia/Tehran",		    GMT03.30},
 //{"Asia/Baku",		    GMT+04.00},
 //{"Asia/Kabul",		    GMT+04.30},
 //{"Asia/Karachi",		GMT+05.00},
 //{"Asia/Calcutta",	    GMT+05.30},
 //{"Asia/Almaty",		GMT+6.00},
 //{"Asia/Bangkok",		GMT+7.00},
 //{"Asia/Hong_Kong",	    GMT+8.00},
 //{"Asia/Tokyo",		    GMT+9.00},
 //{"Australia/Adelaide",	GMT+9.30},
 //{"Pacific/Guam",		GMT+10.00},
 //{"Asia/Magadan",		GMT+11.00},
 //{"Pacific/Auckland",   GMT+12.00}.

 //NTP server suggested is:
 //time.windows.com
 //time.nist.gov
 //time.kriss.re.kr
 //time.nuri.net
 //@field nSecToNow type: INT32 ; means: the second number elapsed from 1970-01-01 0:0:0 to now at the zero timezone when getting. To MSG_SET_DATETIME_REQ: it used to calibrate the device if its value is NOT zero.
 //@field nSecTimeZone type: INT32 ; means: time interval from GMT in second.e.g.:28800 seconds is GMT+08; -28800 seconds is GMT-08.
 //@field bEnableNTP type:  INT32; means: whether the NTP(Network Time Protocol) is enable 1: enable , 0: disable .
 //@field chNTPServer type:CHAR*  ; means: the NTP server domain.

 //SEP2P_MSG_GET_DATETIME_RESP
 public class MSG_GET_DATETIME_RESP
 {
     public const int MY_LEN=80;

     public Int32 nSecToNow     {get;set;}	//[date timeIntervalSince1970], To MSG_SET_DATETIME_REQ: it used to calibrate the device time if its value is NOT zero.
     public Int32 nSecTimeZone  {get;set;}	//time interval from GMT in second, e.g.:28800 seconds is GMT+08; -28800 seconds is GMT-08
     public Int32 bEnableNTP    {get;set;}	//0->disable; 1->enable
     public byte[]  chNTPServer {get;set;}//[64];
     public Int32 nIndexTimeZoneTable{get;set;}//-1: invalid; >=0: ok//added on 2014-11-17, for x series
 };

 //SEP2P_MSG_SET_DATETIME_REQ
 public class MSG_SET_DATETIME_REQ:MSG_GET_DATETIME_RESP{}

 //SEP2P_MSG_SET_DATETIME_RESP
 public class MSG_SET_DATETIME_RESP
 {
     public const int MY_LEN=16;

     public byte   result   {get;set;}	//0: OK; or not fail
     public byte[] reserve  {get;set;}//[15];
 };

 //SEP2P_MSG_GET_FTP_INFO_REQ
 public class MSG_GET_FTP_INFO_REQ
 {
    public const int MY_LEN=16;

    public byte[]  reserve {get;set;}//[16];
 };

 //SEP2P_MSG_GET_FTP_INFO_RESP
 public class MSG_GET_FTP_INFO_RESP
 {
     public const int MY_LEN=332;

     public byte[]   chFTPSvr   {get;set;}//[64];
     public byte[]   chUser     {get;set;}//[64];
     public byte[]   chPwd      {get;set;}//[64];
     public byte[]   chDir  {get;set;}//[128];
     public Int32  nPort    {get;set;}
     public Int32  nMode    {get;set;} //0:port mode 1:passive mode
     public Int32  reserve  {get;set;}
 };

 //SEP2P_MSG_SET_FTP_INFO_REQ
 public class MSG_SET_FTP_INFO_REQ:MSG_GET_FTP_INFO_RESP{}

 //SEP2P_MSG_SET_FTP_INFO_RESP
 public class MSG_SET_FTP_INFO_RESP
 {
     public const int MY_LEN=16;

     public byte   result   {get;set;}	//0: OK; or not fail
     public byte[] reserve  {get;set;}//[15];
 };

 //SEP2P_MSG_GET_EMAIL_INFO_REQ
 public class MSG_GET_EMAIL_INFO_REQ
 {
    public const int MY_LEN=16;

    public byte[]  reserve {get;set;}//[16];
 };

 //SEP2P_MSG_GET_EMAIL_INFO_RESP
 public class MSG_GET_EMAIL_INFO_RESP
 {
     public const int MY_LEN=648;

     public byte[] chSMTPSvr    {get;set;}//[64];
     public byte[] chUser       {get;set;}//[64];
     public byte[] chPwd        {get;set;}//[64];
     public byte[] chSender     {get;set;}//[64];
     public byte[] chReceiver1  {get;set;}//[64];
     public byte[] chReceiver2  {get;set;}//[64];
     public byte[] chReceiver3  {get;set;}//[64];
     public byte[] chReceiver4  {get;set;}//[64];
     public byte[] chSubject    {get;set;}//[64];
     public Int32  nSMTPPort;
     public Int32  nSSLAuth; //0:NONE, 1:SSL, 2:TLS
     public byte[] chText       {get;set;}//[64]; //add 2014-11-13, x series
 };

 //SEP2P_MSG_SET_EMAIL_INFO_REQ
 public class MSG_SET_EMAIL_INFO_REQ:MSG_GET_EMAIL_INFO_RESP{}

 //SEP2P_MSG_SET_EMAIL_INFO_RESP
 public class MSG_SET_EMAIL_INFO_RESP
 {
     public const int MY_LEN=16;

     public byte   result;	//0: OK; or not fail
     public byte[] reserve {get;set;}//[15];
 };

 //SEP2P_MSG_GET_ALARM_INFO_REQ
 public class MSG_GET_ALARM_INFO_REQ
 {
    public const int MY_LEN=16;

    public byte[]  reserve {get;set;}//[16];
 };

 //SEP2P_MSG_GET_ALARM_INFO_RESP
 public class MSG_GET_ALARM_INFO_RESP
 {
    public const int MY_LEN=288;

    public Byte[] arr_bMDEnable      {get;set;}//[4], //arr_bMDEnable[0]--> enable flag of MD0,...; their value are 0->disable, 1->enable;
    public Byte[] arr_nMDSensitivity {get;set;}//[4], //nMDSensitivity[0]--> sensitivity value of MD0,...; their value are (0,9],0->the highest, 9->the lowest;
    public Int32 bInputAlarm    {get;set;} //0->disable; 1->enable
    public Int32 nInputAlarmMode{get;set;} //0->low level(close); 1->high level(open)
    public Int32 bIOLinkageWhenAlarm{get;set;} //0->disable linkage; 1->enable linkage
    public Int32 reserve1           {get;set;}
    public Int32 nPresetbitWhenAlarm    {get;set;} //[0,8]; 0->disable; >0 enable, position index from 1 to 8
    public Int32 bMailWhenAlarm         {get;set;}	//0->disable; 1->enable
    public Int32 bSnapshotToSDWhenAlarm {get;set;}  //0->disable; 1->enable	//None for L Series
    public Int32 bRecordToSDWhenAlarm   {get;set;}	//0->disable; 1->enable  //None for L Series
    public Int32 bSnapshotToFTPWhenAlarm{get;set;}  //0->disable; 1->enable
    public Int32 bRecordToFTPWhenAlarm  {get;set;}  //0->disable; 1->enable  //None for L Series
    public byte[] reserve2  {get;set;}//[8];
    public Int32  nAlarmTime_sun_0 {get;set;}		//24 hours every day, 15 minutes every hour, one flag every 15 minutes, one flag is one bit. bit0---bit95, 0 is invalid in this period(15 minutes), 1 is valid in this period(15 minutes). 
	public Int32  nAlarmTime_sun_1 {get;set;}		//nAlarmTime_sun_0.bit0=00:00:00--00:14:59; nAlarmTime_sun_0.bit1=00:15:00--00:29:59; nAlarmTime_sun_0.bit2=00:30:00--00:44:59; ......
	public Int32  nAlarmTime_sun_2 {get;set;}		//nAlarmTime_sun_1.bit0=08:00:00--08:14:59; nAlarmTime_sun_1.bit1=08:15:00--08:29:59; nAlarmTime_sun_1.bit2=08:30:00--08:44:59; ......
	public Int32  nAlarmTime_mon_0 {get;set;}		
	public Int32  nAlarmTime_mon_1 {get;set;}		//nAlarmTime_sun_0, nAlarmTime_sun_1, nAlarmTime_sun_2: Sunday
	public Int32  nAlarmTime_mon_2 {get;set;}		//nAlarmTime_mon_0, nAlarmTime_mon_1, nAlarmTime_mon_2: Monday
	public Int32  nAlarmTime_tue_0 {get;set;}		//......
	public Int32  nAlarmTime_tue_1 {get;set;}
	public Int32  nAlarmTime_tue_2 {get;set;}
	public Int32  nAlarmTime_wed_0 {get;set;}
	public Int32  nAlarmTime_wed_1 {get;set;}
	public Int32  nAlarmTime_wed_2 {get;set;}
	public Int32  nAlarmTime_thu_0 {get;set;}
	public Int32  nAlarmTime_thu_1 {get;set;}
	public Int32  nAlarmTime_thu_2 {get;set;}
	public Int32  nAlarmTime_fri_0 {get;set;}
	public Int32  nAlarmTime_fri_1 {get;set;}
	public Int32  nAlarmTime_fri_2 {get;set;}
	public Int32  nAlarmTime_sat_0 {get;set;}
	public Int32  nAlarmTime_sat_1 {get;set;}
	public Int32  nAlarmTime_sat_2 {get;set;}

    public byte nAudioAlarmSensitivity{get;set;}  //0->disable; >0 enable, [1,100] is sensitivity value. 1 is the lowest, 100 is the highest.
    public byte nTimeSecOfIOOut   {get;set;}	  //time of IO output, unit second, for X series
	public byte bSpeakerWhenAlarm {get;set;}	  //0->disable; 1->enable, for x series
	public byte nTimeSecOfSpeaker {get;set;}	  //unit: second, for x series
    public byte[]  md_name   {get;set;}//[64];	  //for x series

	public Int32[] arr_md_x		 {get;set;}	//[4], md_x[0] is the horizontal coordinates of MD0, md_x[1] is MD1,...;	valid firmware>=V0.1.0.30 for M, 1.0.0.22 for X
	public Int32[] arr_md_y		 {get;set;}	//[4], md_y[0] is the vertical coordinates of MD0, md_y[1] is MD1,...;	valid firmware>=V0.1.0.30 for M, 1.0.0.22 for X
	public Int32[] arr_md_width  {get;set;} //[4], md_width[0] is the width of MD0, md_width[1] is MD1,...;			valid firmware>=V0.1.0.30 for M, 1.0.0.22 for X
	public Int32[] arr_md_height {get;set;} //[4], md_height[0] is the height of MD0, md_height[1] is MD1,...;		valid firmware>=V0.1.0.30 for M, 1.0.0.22 for X
	
	public byte nTriggerAlarmType	{get;set;}	//0=Trigger independently: Trigger when detecting by any kind of trigger; 1=Trigger jointly
	public byte bTemperatureAlarm	{get;set;}	//0->disable; 1->enable, for x series
	public byte bHumidityAlarm		{get;set;}	//0->disable; 1->enable, for x series
	public byte[] reserve3 {get;set;}  //[5]
	public Int16 nTempMinValueWhenAlarm {get;set;}  //trigger alarm when temperature < this value, [-100, 100],unit 0C
	public Int16 nTempMaxValueWhenAlarm {get;set;}  //trigger alarm when temperature > this value, [-100, 100],unit 0C
	public Int16 nHumiMinValueWhenAlarm {get;set;}  //trigger alarm when humidity < this value,	  [0, 100], unit %
	public Int16 nHumiMaxValueWhenAlarm {get;set;}  //trigger alarm when humidity > this value,    [0, 100], unit %

    private void reset(){
            arr_bMDEnable = null;
            arr_nMDSensitivity = null;
            bInputAlarm = 0;
            nInputAlarmMode=0;
            bIOLinkageWhenAlarm = 0;
            reserve1 = 0;
            nPresetbitWhenAlarm = 0;
            bMailWhenAlarm = 0;
            bSnapshotToSDWhenAlarm=0;
            bRecordToSDWhenAlarm=0;
            bSnapshotToFTPWhenAlarm=0;
            bRecordToFTPWhenAlarm=0;
            reserve2=null;
            nAudioAlarmSensitivity=0;
            reserve3=null;
            md_name=null;
            arr_md_x=null;
            arr_md_y=null;
            arr_md_width=null;
            arr_md_height=null;
			
			nTriggerAlarmType=0;
			bTemperatureAlarm=0;
			bHumidityAlarm=0;
			nTempMinValueWhenAlarm=0;
			nTempMaxValueWhenAlarm=0;
			nHumiMinValueWhenAlarm=0;
			nHumiMaxValueWhenAlarm=0;
    }
    public MSG_GET_ALARM_INFO_RESP() { reset(); }
    public MSG_GET_ALARM_INFO_RESP(byte[] arrData){
        if (arrData == null || arrData.Length < MY_LEN){
            reset();
        }else{
            arr_bMDEnable = new byte[4];
            Array.Copy(arrData, 0, arr_bMDEnable,0,arr_bMDEnable.Length);
            arr_nMDSensitivity = new byte[4];
            Array.Copy(arrData, 4,arr_nMDSensitivity,0,arr_nMDSensitivity.Length);
            bInputAlarm = BitConverter.ToInt32(arrData, 8);
            nInputAlarmMode = BitConverter.ToInt32(arrData, 12);
            bIOLinkageWhenAlarm = BitConverter.ToInt32(arrData, 16);
            reserve1 = 0;
            nPresetbitWhenAlarm = BitConverter.ToInt32(arrData, 24);
            bMailWhenAlarm = BitConverter.ToInt32(arrData, 28);
            bSnapshotToSDWhenAlarm = BitConverter.ToInt32(arrData,32);
            bRecordToSDWhenAlarm = BitConverter.ToInt32(arrData, 36);
            bSnapshotToFTPWhenAlarm = BitConverter.ToInt32(arrData, 40);
            bRecordToFTPWhenAlarm = BitConverter.ToInt32(arrData, 44);
            reserve2 = null;
            nAudioAlarmSensitivity = arrData[140];
            reserve3 = null;
            md_name = new byte[64];
            Array.Copy(arrData, 144,md_name,0,md_name.Length);

            arr_md_x = new Int32[4];
                arr_md_x[0]=BitConverter.ToInt32(arrData,208);
                arr_md_x[1]=BitConverter.ToInt32(arrData,212);
                arr_md_x[2]=BitConverter.ToInt32(arrData,216);
                arr_md_x[3]=BitConverter.ToInt32(arrData,220);
            arr_md_y = new Int32[4];
                arr_md_y[0]=BitConverter.ToInt32(arrData,224);
                arr_md_y[1]=BitConverter.ToInt32(arrData,228);
                arr_md_y[2]=BitConverter.ToInt32(arrData,232);
                arr_md_y[3]=BitConverter.ToInt32(arrData,236);
            arr_md_width = new Int32[4];
                arr_md_width[0]=BitConverter.ToInt32(arrData,240);
                arr_md_width[1]=BitConverter.ToInt32(arrData,244);
                arr_md_width[2]=BitConverter.ToInt32(arrData,248);
                arr_md_width[3]=BitConverter.ToInt32(arrData,252);
            arr_md_height = new Int32[4];
                arr_md_height[0]=BitConverter.ToInt32(arrData,256);
                arr_md_height[1]=BitConverter.ToInt32(arrData,260);
                arr_md_height[2]=BitConverter.ToInt32(arrData,264);
                arr_md_height[3]=BitConverter.ToInt32(arrData,268);

			nTriggerAlarmType=arrData[272];
			bTemperatureAlarm=arrData[273];
			bHumidityAlarm=arrData[274];
            //reserve3[5]
            nTempMinValueWhenAlarm = BitConverter.ToInt16(arrData,280);
            nTempMaxValueWhenAlarm = BitConverter.ToInt16(arrData,282);
            nHumiMinValueWhenAlarm = BitConverter.ToInt16(arrData,284);
            nHumiMaxValueWhenAlarm = BitConverter.ToInt16(arrData,286);
        }
    }
 };

 //SEP2P_MSG_SET_ALARM_INFO_REQ
 public class MSG_SET_ALARM_INFO_REQ:MSG_GET_ALARM_INFO_RESP{}

 //SEP2P_MSG_SET_ALARM_INFO_RESP
 public class MSG_SET_ALARM_INFO_RESP
 {
     public const int MY_LEN=16;
     public byte   result  {get;set;}	//0: OK; or not fail
     public byte[] reserve {get;set;}//[15];
 };

 //SEP2P_MSG_GET_SDCARD_REC_PARAM_REQ
 public class MSG_GET_SDCARD_REC_PARAM_REQ
 {
    public const int MY_LEN=16;
    public byte[] reserve {get;set;}//[16];
 };

 //SEP2P_MSG_GET_SDCARD_REC_PARAM_RESP
 public class MSG_GET_SDCARD_REC_PARAM_RESP
 {
     public const int MY_LEN=112;

     public Int32 bRecordCoverInSDCard  {get;set;} //0->disable record in sd-card; 1->enable record in sd-card
     public Int32 nRecordTimeLen        {get;set;}	//record time length, in minutes
     public Int32 reserve1              {get;set;}
     public Int32 bRecordTime           {get;set;}	//0->disable 1->enable
     public byte[] reserve2             {get;set;}//[84];
     public Int32 nSDCardStatus         {get;set;}	//0: no sdcard; 1: sdcard inserted
     public Int32 nSDTotalSpace         {get;set;}	//in MBytes
     public Int32 nSDFreeSpace          {get;set;}	//in MBytes
 };

 //SEP2P_MSG_SET_SDCARD_REC_PARAM_REQ
 public class MSG_SET_SDCARD_REC_PARAM_REQ:MSG_GET_SDCARD_REC_PARAM_RESP{}

 //SEP2P_MSG_SET_SDCARD_REC_PARAM_RESP
 public class MSG_SET_SDCARD_REC_PARAM_RESP
 {
     public const int MY_LEN=16;

     public byte   result   {get;set;}	//0: OK; or not fail
     public byte[] reserve  {get;set;}//[15];
 };

 //SEP2P_MSG_FORMAT_SDCARD_REQ
 public class MSG_FORMAT_SDCARD_REQ
 {
    public const int MY_LEN=16;
    public byte[]  reserve{get;set;}//[16];
 };

 //SEP2P_MSG_GET_DEVICE_VERSION_REQ
 public class MSG_GET_DEVICE_VERSION_REQ
 {
    public const int MY_LEN=16;
    public byte[] reserve {get;set;}//[16];
 };

 //SEP2P_MSG_GET_DEVICE_VERSION_RESP
 public class MSG_GET_DEVICE_VERSION_RESP
 {
     public const int MY_LEN=352;
     public byte[] chP2papi_ver     {get;set;}//[32];	   //API version
     public byte[] chFwp2p_app_ver  {get;set;}//[32];	   //P2P firmware version
     public byte[] chFwp2p_app_buildtime{get;set;}//[32];  //P2P firmware build time
     public byte[] chFwddns_app_ver {get;set;}//[32];      //firmware version
     public byte[] chFwhard_ver     {get;set;}//[32];      //the device hard version
     public byte[] chVendor         {get;set;}//[32];      //factory name
     public byte[] chProduct        {get;set;}//[32];      //product mode
     public byte[] product_series   {get;set;}//[4];	   //product main category, L series: product_series[0]='L'; M series:product_series[0]='M',product_series[1]='1';
	
	 public Int32 imn_ver_of_device {get;set;}//little-endian  //2015-08-21
	 public byte  is_push_function	{get;set;}//0: no, 1: yes; //2015-08-21
	 public byte[] reserve1			{get;set;}//[3];
	 public byte[] imn_server_port	{get;set;}//[96];//e.g.: domain_or_ip1:port1|domain_or_ip2:port2|...
	 public byte[] reserve          {get;set;}//[20];
 };

 //SEP2P_MSG_ALARM_IO_CTRL_REQ
 public class MSG_ALARM_IO_CTRL_REQ
 {
     public const int MY_LEN=24;
     public byte   bAlarmOutIO1 {get;set;}	//0-->off, 1-->on
     public byte[] reserve      {get;set;}//[23];
 };

 //SEP2P_MSG_ALARM_IO_CTRL_RESP
 public class MSG_ALARM_IO_CTRL_RESP
 {
     public const int MY_LEN=16;
     public byte   result   {get;set;}	//0: OK; or not fail
     public byte[] reserve  {get;set;}//[15];
 };

 //SEP2P_MSG_GET_UART_CTRL_REQ
 public class MSG_GET_UART_CTRL_REQ
 {
    public const int MY_LEN=16;
    public byte[]  reserve  {get;set;}//[16];
 };

 //SEP2P_MSG_GET_UART_CTRL_RESP
 public class MSG_GET_UART_CTRL_RESP
 {
     public const int MY_LEN=152;
     public byte[]   bUartAlarmEnable   {get;set;}//[24]; //24 pcs uart alarm port enable/disable. bUartAlarmEnable[0]=0,first uart alarm port disable,bUartAlarmEnable[0]=1,first uart alarm port enable,......
     public byte[]   chUartAlarmServer  {get;set;}//[64];
     UInt16 nUartAlarmServerPort        {get;set;}
     public byte[]   reserve            {get;set;}//[62];
 };

 public static class ENUM_BIT_MASK_UART_CTRL
 {
     public const int BIT_MASK_UART_ALARM_ENABLE     =0x01;
     public const int BIT_MASK_UART_ALARM_SERVER	 =0x02;
     public const int BIT_MASK_UART_ALARM_SERVER_PORT=0x04;
 };
 //SEP2P_MSG_SET_UART_CTRL_REQ
 public class MSG_SET_UART_CTRL_REQ
 {
     public const int MY_LEN=152;

     public byte[]   bUartAlarmEnable   {get;set;}//[24]; //24 pcs uart alarm port enable/disable. bUartAlarmEnable[0]=0,first uart alarm port disable,bUartAlarmEnable[0]=1,first uart alarm port enable,bUartAlarmEnable[0]=other value,invalid;......
     public byte[]   chUartAlarmServer  {get;set;}//[64];
     UInt16 nUartAlarmServerPort        {get;set;}
     public byte[]   reserve            {get;set;}//[58];

     public Int32  nBitMaskToSet        {get;set;}//refer to ENUM_BIT_MASK_UART_CTRL
                                 //Bit0:=1->set uart alarm port enable/disable(field bUartAlarmEnable); Bit1:=1->set uart alarm server ip or domain(field chUartAlarmServer);
                                 //Bit2:=1->set uart alarm server port(field nUartAlarmServerPort);
 };

 //SEP2P_MSG_SET_UART_CTRL_RESP
 public class MSG_SET_UART_CTRL_RESP
 {
     public const int MY_LEN=16;

     public byte   result       {get;set;}	//0: OK; or not fail
     public byte[] reserve      {get;set;}//[11];
     public Int32  nBitMaskToSet{get;set;}//refer to ENUM_BIT_MASK_UART_CTRL
 };

 //SEP2P_MSG_REBOOT_DEVICE = 0x0196,
 public class MSG_REBOOT_DEVICE
 {
    public const int MY_LEN=16;
    public byte[]  reserve{get;set;}//[16];
 };

 //SEP2P_MSG_GET_CUSTOM_PARAM_REQ  = 0x0198,
 public class MSG_GET_CUSTOM_PARAM_REQ
 {
     public const int MY_LEN=40;
     public byte[]  reserve     {get;set;}//[8];
     public byte[]  chParamName {get;set;}//[32];
 };

 //SEP2P_MSG_GET_CUSTOM_PARAM_RESP = 0x0199,
 public class MSG_GET_CUSTOM_PARAM_RESP
 {
     public const int MY_LEN=104;

     public byte   result       {get;set;}	//0: OK; 1:config_custom.ini does not exist; 2:chParamName does not exist
     public byte[] reserve      {get;set;}//[7];
     public byte[] chParamName  {get;set;}//[32];
     public byte[] chParamValue {get;set;}//[64];
 };

 //SEP2P_MSG_SET_CUSTOM_PARAM_REQ  = 0x019A,
 public class MSG_SET_CUSTOM_PARAM_REQ
 {
     public const int MY_LEN=104;

     public byte[]  reserve     {get;set;}//[8];
     public byte[]  chParamName {get;set;}//[32];
     public byte[]  chParamValue{get;set;}//[64];
 };

 //SEP2P_MSG_SET_CUSTOM_PARAM_RESP = 0x019B
 //discussion:
 //	after setting is ok. return MSG_SET_CUSTOM_PARAM_RESP.chParamName=MSG_SET_CUSTOM_PARAM_REQ.chParamName
 //	MSG_SET_CUSTOM_PARAM_RESP.chParamValue=MSG_SET_CUSTOM_PARAM_REQ.chParamValue
 //
 public class MSG_SET_CUSTOM_PARAM_RESP
 {
     public const int MY_LEN=104;

     public byte   result       {get;set;}	//0: OK; 1:config_custom.ini does not exist; 2:chParamName does not exist
     public byte[] reserve      {get;set;}//[7];
     public byte[] chParamName  {get;set;}//[32];
     public byte[] chParamValue {get;set;}//[64];
 };

 //SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ		= 0x01A6
 public class MSG_GET_REMOTE_REC_DAY_BY_MONTH_REQ
 {
     public const int MY_LEN=144;

     public Int32	nYearMon    {get;set;}	//e.g:201412
     public Int32	nRecType    {get;set;}	//1:plan record; 2:alarm record; 3:all
     public byte[]	reserve     {get;set;}//[136];
 };

 //SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP	= 0x01A7
 public class MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP
 {
     public const int MY_LEN=144;

     public Int32	nYearMon    {get;set;}	//e.g:201412
     public Int32	nRecType    {get;set;}	//1:plan record; 2:alarm record; 3:all
     public byte[]	chDay       {get;set;}//[128];	//e.g: "01,02" means that there is record file on the 20141201 and the 20141202
     public byte[]  reserve     {get;set;}//[8];
 };

 //SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ      = 0x01A8
 public class MSG_GET_REMOTE_REC_FILE_BY_DAY_REQ
 {
     public const int MY_LEN=24;

     public Int32	nYearMonDay {get;set;}//e.g:20141201
     public Int32	nRecType    {get;set;}	//1:plan record; 2:alarm record; 3:all
     public Int32   nBeginNoOfThisTime  {get;set;}	//>=0; want to return the first file index in this time
     public Int32   nEndNoOfThisTime    {get;set;}	//>=0 and >=nBeginNoOfThisTime; want to return the last file index in this time. when nEndNoOfThisTime=nBeginNoOfThisTime and is zero, get all record of given condition
     public byte[]	reserve     {get;set;}//[8];
 };

 public class REC_FILE_INFO
 {
     public const int MY_LEN=200;

     public byte[]  chStartTime {get;set;}//[32];
     public byte[]  chEndTime   {get;set;}//[32];
     public byte    nRecType    {get;set;}
     public byte[]  reserve     {get;set;}//[7];
     public Int32 nTimeLen_sec  {get;set;}	//unit: second, record file playback time
     public Int32 nFileSize_KB  {get;set;}	//unit: KBytes, record file size in byte
     public byte[]  chFilePath  {get;set;}//[120];
 }

 //SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP     = 0x01A9
 public class MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP
 {
     public const int MY_LEN=24;

     public Int32 nResult       {get;set;}	//0:OK, 1:NO SDCard; 2:fail
     public Int32 nFileTotalNum {get;set;}		//total file num >=0
     public Int32 nBeginNoOfThisTime{get;set;}	//to return the first file index in this time
     public Int32 nEndNoOfThisTime  {get;set;}	//to return the last file index in this time
     public byte[]  reserve         {get;set;}//[8];

     //REC_FILE_INFO[x], x=nEndNoOfThisTime-nBeginNoOfThisTime+1
 };

 //SEP2P_MSG_START_PLAY_REC_FILE_REQ				   = 0x01AA
 public class MSG_START_PLAY_REC_FILE_REQ
 {
     public const int MY_LEN=136;

     public byte    bOnlyPlayIFrame {get;set;}
     public byte[]  reserve1        {get;set;}//[3];
     public Int32   nBeginPos_sec   {get;set;}	//unit: second
     public byte[]  chFilePath      {get;set;}//[120];
     public byte[]  reserve2        {get;set;}//[8];
 };
 //SEP2P_MSG_START_PLAY_REC_FILE_RESP               = 0x01AB
 public class MSG_START_PLAY_REC_FILE_RESP
 {
     public const int MY_LEN=144;

     public byte    nResult     {get;set;} //0:OK; 1:playback end;    Error:[11:already playback; 12:failed to open file; 13:playback fail;]
     public byte    nAudioParam {get;set;} //-1 no audio; >=0 with audio:[samplerate << 2) | (databits << 1) | (channel)], samplerate refer to SEP2P_ENUM_AUDIO_SAMPLERATE; databits refer to SEP2P_ENUM_AUDIO_DATABITS; channel refer to SEP2P_ENUM_AUDIO_CHANNEL
     public byte[]  reserve1    {get;set;}//[2];
     public Int32 nBeginPos_sec {get;set;}	//unit: second
     public byte[]  chFilePath  {get;set;}//[120];
     public Int32 nTimeLen_sec  {get;set;}		//unit: second, record file playback total time
     public Int32 nFileSize_KB  {get;set;}		//unit: KBytes, record file size in byte
     public UInt32 nPlaybackID  {get;set;}		//this id will assign to STREAM_HEAD.reserve2[2] ~ STREAM_HEAD.reserve2[5]
     public byte[]   reserve2   {get;set;}//[4];
 };

 //SEP2P_MSG_STOP_PLAY_REC_FILE_REQ                   = 0x01AC
 public class MSG_STOP_PLAY_REC_FILE_REQ
 {
     public const int MY_LEN=136; 
     public byte[]  reserve     {get;set;}//[16];
     public byte[]  chFilePath  {get;set;}//[120];
 };
 //SEP2P_MSG_STOP_PLAY_REC_FILE_RESP                  = 0x01AD
 public class MSG_STOP_PLAY_REC_FILE_RESP
 {
     public const int MY_LEN=136;

     public byte    nResult     {get;set;}		//0:OK;
     public byte[]  reserve     {get;set;}//[15];
     public byte[]  chFilePath  {get;set;}//[120];
 };




 //SEP2P_MSG_GET_REC_DATE_RANGE_REQ				   = 0x01AE,
 public class MSG_GET_REC_DATE_RANGE_REQ
 {
     public const int MY_LEN=64;
     public byte[]	reserve {get;set;}//[64];
 };

 //SEP2P_MSG_GET_REC_DATE_RANGE_RESP		        = 0x01AF,
 public class MSG_GET_REC_DATE_RANGE_RESP
 {
     public const int MY_LEN=24;

     public byte   result       {get;set;}
     public byte[] reserve      {get;set;}//[7];
     public Int32	nYearMonDay1{get;set;}	//e.g:20141201, is 2014-12-01
     public Int32	nHourMinSec1{get;set;}	//e.g:160110, is 16:01:10
     public Int32	nYearMonDay2{get;set;}	//e.g:20141206, is 2014-12-01
     public Int32	nHourMinSec2{get;set;}	//e.g:160110, is 16:01:10
 };

 //SEP2P_MSG_START_PLAY_REC_FILE2_REQ               = 0x01B0,
 public class MSG_START_PLAY_REC_FILE2_REQ
 {

 };

 //SEP2P_MSG_START_PLAY_REC_FILE2_RESP              = 0x01B1,
 public class MSG_START_PLAY_REC_FILE2_RESP
 {

 };

 //SEP2P_MSG_STOP_PLAY_REC_FILE2_REQ                = 0x01B2,
 public class MSG_STOP_PLAY_REC_FILE2_REQ
 {

 };

 //SEP2P_MSG_STOP_PLAY_REC_FILE2_RESP               = 0x01B3,
 public class MSG_STOP_PLAY_REC_FILE2_RESP
 {

 };


	//SEP2P_MSG_GET_IPUSH_INFO_REQ					= 0x01BA,	//X series, supported on the v1.0.0.14 of firmware 
	//- (no data)

	//SEP2P_MSG_GET_IPUSH_INFO_RESP					= 0x01BB,	//X series, supported on the v1.0.0.14 of firmware 
	public class MSG_GET_IPUSH_INFO_RESP
	{
		public const int MY_LEN=104;

		public byte  bEnable; //0: disable; 1:enable
		public byte  nResult; //0: OK; other fail
		public byte  reserve; //[102];
	};

	//SEP2P_MSG_SET_IPUSH_INFO_REQ					= 0x01BC,	//X series, supported on the v1.0.0.14 of firmware 
	public class MSG_SET_IPUSH_INFO_REQ:MSG_GET_IPUSH_INFO_RESP{};


	//SEP2P_MSG_SET_IPUSH_INFO_RESP					= 0x01BD,	//X series, supported on the v1.0.0.14 of firmware 
	public class MSG_SET_IPUSH_INFO_RESP:MSG_GET_IPUSH_INFO_RESP{};

}
