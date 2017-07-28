using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using SEP2P_AppSDK;
using SEP2P_AppSDK.SEP2P_Define;

namespace SEP2PAppSDKDemo
{
    public class CamObj
    {
        public const int  ERR_INVALID_PARAMETER					=-400;
        public const int  ERR_P2PAPI_ALREADY_OPEN_AUDIO		    =(ERR_INVALID_PARAMETER-1);
        public const int  ERR_P2PAPI_UNKNOWN_CODEC_WHEN_TALK	=(ERR_INVALID_PARAMETER-2);

        //delegate---------------------------------------------------------
        public delegate Int32 Deleg_OnLANSearchCallback(byte[] pData, UInt32  nDataSize);
        public delegate Int32 Deleg_OnRecvMsgCallback(string strDID, UInt32 nMsgType, byte[] arrMsg, UInt32 nDataSize);
        public delegate Int32 Deleg_OnStreamCallback(string strDID,  byte[] arrData, Int32 nDataSize);
        public delegate Int32 Deleg_OnEventCallback(string strDID, UInt32 nEventType, byte[] arrEvent, UInt32 nEventDataSize);

        //Method-----------------------------------------------------------
        public CamObj() { 
            m_nConnStatus=CONNECT_STATUS.CONNECT_STATUS_UNKNOWN;
        }
        public static UInt32 P2PAPI_Version(){
            String str;
            UInt32 nVer=API.SEP2P_GetSDKVersion(out str);
            return nVer;
        }
	    public static void P2PAPI_Init()        { API.SEP2P_Initialize(null);                       }
	    public static void P2PAPI_DeInit()      { API.SEP2P_DeInitialize();                         }

        public static Int32 P2PAPI_StartSearch(){
            CamObj obj=new CamObj();
            return API.SEP2P_StartSearch(obj.OnLANSearchCallback);
        }
        public static void P2PAPI_StopSearch()  { API.SEP2P_StopSearch();                           }

        public Int32 P2PAPI_Connect()           {
            Int32 nRet=ERR_INVALID_PARAMETER;
            if(String.IsNullOrEmpty(m_strDID)) return nRet;
            nRet=API.SEP2P_Connect(m_strDID,m_strUser,m_strPasswd);
            if (nRet >= 0){
                API.SEP2P_SetRecvMsgCallback(m_strDID, OnRecvMsgCallback);
                API.SEP2P_SetStreamCallback(m_strDID, OnStreamCallback);
                API.SEP2P_SetEventCallback(m_strDID, OnEventCallback);
            }
            return nRet;
        }
        public void  P2PAPI_Disconnect()        { API.SEP2P_Disconnect(m_strDID);                   }
        public Int32 P2PAPI_StartLiveVideo()    {
            Int32 nRet=P2PAPI_SendMsg(SEP2P_ENUM_MSGTYPE.SEP2P_MSG_START_VIDEO, null, 0);
            if(nRet >= 0)
            {
                //Create thread or task to handle the video data from OnStreamCallback
            }
            return nRet;
        }
        public Int32 P2PAPI_StopLiveVideo()     { return P2PAPI_SendMsg(SEP2P_ENUM_MSGTYPE.SEP2P_MSG_STOP_VIDEO,null,0);     }
        public Int32 P2PAPI_StartLiveAudio()    { 
            Int32 nRet= P2PAPI_SendMsg(SEP2P_ENUM_MSGTYPE.SEP2P_MSG_START_AUDIO, null, 0);
            if(nRet >= 0)
            {
                //Create thread or task to handle the audio data from OnStreamCallback
            }
            return nRet;
        }

        public Int32 P2PAPI_StopLiveAudio()     { return P2PAPI_SendMsg(SEP2P_ENUM_MSGTYPE.SEP2P_MSG_STOP_AUDIO, null, 0);   }
        public Int32 P2PAPI_GetConnectStatus()  { return m_nConnStatus;}
        public Boolean P2PAPI_IsConnect()       { return m_nConnStatus==CONNECT_STATUS.CONNECT_STATUS_CONNECTED? true : false;}

        public Int32 P2PAPI_SendMsg(Int32 nMsgType, byte[] pMsgData, Int32 nMsgDataSize) { 
            if(m_nConnStatus!=CONNECT_STATUS.CONNECT_STATUS_CONNECTED) return CONNECT_STATUS.CONNECT_STATUS_DEVICE_NOT_ONLINE;
            if(String.IsNullOrEmpty(m_strDID)) return ERR_INVALID_PARAMETER;
            return API.SEP2P_SendMsg(m_strDID, nMsgType, pMsgData, nMsgDataSize);
        }
        public Int32 P2PAPI_TalkData(byte[] pData, UInt32 nDataSize) { 
            //return API.SEP2P_SendTalkData(m_strDID, pData,nDataSize, nTimestamp);
            return 0;
        }


        //Callback---------------------------------------------------------
        private Int32 OnLANSearchCallback(byte[] pData, UInt32 nDataSize)
        {
            Int32 nRet=SEP2P_Error.ERR_SEP2P_SUCCESSFUL;
            if(deleg_OnLANSearchCallback!=null) nRet=deleg_OnLANSearchCallback(pData, nDataSize);
            else nRet=ERR_INVALID_PARAMETER;

            return nRet;
        }

        private Int32 OnRecvMsgCallback(string strDID, UInt32 nMsgType, byte[] arrMsg, UInt32 nDataSize)
        {
            if(strDID.CompareTo(m_strDID)!=0) return 0;
            Int32 nRet = SEP2P_Error.ERR_SEP2P_SUCCESSFUL;
            switch (nMsgType)
            {
                case SEP2P_ENUM_MSGTYPE.SEP2P_MSG_CONNECT_STATUS:
                    {
                        m_nConnStatus = BitConverter.ToInt32(arrMsg, 0);
                        String str = String.Format("\tCamObj.OnRecvMsgCallback] m_nConnStatus={0}", m_nConnStatus);
                        Debug.WriteLine(str);
                        if (m_nConnStatus == CONNECT_STATUS.CONNECT_STATUS_CONNECTED)
                        {
                            AV_PARAMETER stParam = null;
                            API.SEP2P_GetAVParameterSupported(strDID, out stParam);
                            m_stAVParam=stParam;

                            str = String.Format("\tCamObj.OnRecvMsgCallback] nAudioCodecID={0} nVideoCodecID={1}",
                                stParam.nAudioCodecID, stParam.nVideoCodecID);
                            Debug.WriteLine(str);
                        }
                    } break;

                case SEP2P_ENUM_MSGTYPE.SEP2P_MSG_GET_DEVICE_VERSION_RESP:
                    {

                    }break;

                case SEP2P_ENUM_MSGTYPE.SEP2P_MSG_GET_ALARM_INFO_RESP:
                    {
                        MSG_GET_ALARM_INFO_RESP objAlarmInfoResp=new MSG_GET_ALARM_INFO_RESP(arrMsg);
                        String str;
                        str = String.Format("\tCamObj.OnRecvMsgCallback] SEP2P_MSG_GET_ALARM_INFO_REQ MDEnable={0},{1},{2},{3} MDSensitivity={4},{5},{6},{7} md_x={8},{9},{10},{11}," +
                    "md_y={12},{13},{14},{15} md_w={16},{17},{18},{19} md_h={20},{21},{22},{23}",
                                objAlarmInfoResp.arr_bMDEnable[0],objAlarmInfoResp.arr_bMDEnable[1],
                                objAlarmInfoResp.arr_bMDEnable[2],objAlarmInfoResp.arr_bMDEnable[3],
                                objAlarmInfoResp.arr_nMDSensitivity[0],objAlarmInfoResp.arr_nMDSensitivity[1],
                                objAlarmInfoResp.arr_nMDSensitivity[2],objAlarmInfoResp.arr_nMDSensitivity[3],
                                objAlarmInfoResp.arr_md_x[0],objAlarmInfoResp.arr_md_x[1],objAlarmInfoResp.arr_md_x[2],objAlarmInfoResp.arr_md_x[3],
                                objAlarmInfoResp.arr_md_y[0],objAlarmInfoResp.arr_md_y[1],objAlarmInfoResp.arr_md_y[2],objAlarmInfoResp.arr_md_y[3],
                                objAlarmInfoResp.arr_md_width[0],objAlarmInfoResp.arr_md_width[1],objAlarmInfoResp.arr_md_width[2],objAlarmInfoResp.arr_md_width[3],
                                objAlarmInfoResp.arr_md_height[0],objAlarmInfoResp.arr_md_height[1],objAlarmInfoResp.arr_md_height[2],objAlarmInfoResp.arr_md_height[3]);
                        Debug.WriteLine(str);
                    } break;

                default:
                    break;
            }

            if (deleg_OnRecvMsgCallback != null) nRet = deleg_OnRecvMsgCallback(strDID,nMsgType,arrMsg,nDataSize);
            else nRet = ERR_INVALID_PARAMETER;

            return nRet;
        }

        //arrData=STREAM_HEAD + raw data
        //nDataSize is the size of arrData
        private Int32 OnStreamCallback(string strDID,  byte[] arrData, Int32 nDataSize)
        {
            if(strDID.CompareTo(m_strDID)!=0) return 0;

            STREAM_HEAD stStreamHead=new STREAM_HEAD(arrData);
            if (stStreamHead.nCodecID < SEP2P_ENUM_AV_CODECID.AV_CODECID_AUDIO_ADPCM) { 
                //video data
                //AddToVideoFIFO
                String str;
                str=String.Format("OnStreamCallback] Video data: strDID={0},nDataSize={1},nTimestamp={2},nParameter={3} {4} {5} {6} {7} {8}",
                            strDID, nDataSize, stStreamHead.nTimestamp, stStreamHead.nParameter, 
                            arrData[STREAM_HEAD.MY_LEN],arrData[STREAM_HEAD.MY_LEN+1],arrData[STREAM_HEAD.MY_LEN+2],
                            arrData[STREAM_HEAD.MY_LEN+3],arrData[STREAM_HEAD.MY_LEN+4]);
                Debug.WriteLine(str);
            }else{
                //audio data
                //AddToAudioFIFO
                Debug.WriteLine("OnStreamCallback] Audio data: strDID="+strDID+",nDataSize="+nDataSize);
            }

            Int32 nRet = SEP2P_Error.ERR_SEP2P_SUCCESSFUL;
            if (deleg_OnStreamCallback != null) nRet = deleg_OnStreamCallback(strDID, arrData, nDataSize);
            else nRet = ERR_INVALID_PARAMETER;

            return nRet;
        }

        private Int32 OnEventCallback(string strDID, UInt32 nEventType, byte[] arrEvent, UInt32 nEventDataSize)
        {
            if(strDID.CompareTo(m_strDID)!=0) return 0;

            Int32 nRet = SEP2P_Error.ERR_SEP2P_SUCCESSFUL;
            if (deleg_OnEventCallback != null) nRet = deleg_OnEventCallback(strDID, nEventType, arrEvent, nEventDataSize);
            else nRet = ERR_INVALID_PARAMETER;

            return nRet;
        }

        //Property---------------------------------------------------------
        public event  Deleg_OnLANSearchCallback deleg_OnLANSearchCallback;
        public event  Deleg_OnRecvMsgCallback   deleg_OnRecvMsgCallback;
        public event  Deleg_OnStreamCallback    deleg_OnStreamCallback;
        public event  Deleg_OnEventCallback     deleg_OnEventCallback;

        public String m_strDID          {get;set;}
        public String m_strUser         {get;set;}
        public String m_strPasswd       {get;set;}
        public AV_PARAMETER m_stAVParam {get;set;}
        
        private Int32 m_nConnStatus     {get;set;}

    };
}
