
using System;
using System.Net;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using System.Diagnostics;
using SEP2P_AppSDK;
using SEP2P_AppSDK.SEP2P_Define;


namespace SEP2PAppSDKDemo
{
    public partial class MainPage : PhoneApplicationPage
    {
        public const int MAX_NUM_HANDLED    =2;

        CamObj[] arrObjCam;
        // 构造函数
        public MainPage()
        {
            InitializeComponent();
            BackKeyPress +=PageBackKeyPress;

            String strTip= String.Format(" nVer=0x{0:X}\n", CamObj.P2PAPI_Version());
            Debug.WriteLine("public MainPage()] " + strTip);
            CamObj.P2PAPI_Init();

            arrObjCam =new CamObj[MAX_NUM_HANDLED];
            for(Int32 i=0; i<MAX_NUM_HANDLED; i++){
                arrObjCam[i]=new CamObj();
                arrObjCam[i].deleg_OnLANSearchCallback +=OnLANSearchCallback;
                arrObjCam[i].deleg_OnRecvMsgCallback   +=OnRecvMsgCallback;
                arrObjCam[i].deleg_OnStreamCallback    +=OnStreamCallback;
                arrObjCam[i].deleg_OnEventCallback     +=OnEventCallback;
            }
            arrObjCam[0].m_strDID      ="your device id";
            arrObjCam[0].m_strUser     ="admin";
            arrObjCam[0].m_strPasswd   ="your device password";

            arrObjCam[1].m_strDID   = "your device id";
            arrObjCam[1].m_strUser  = "admin";
            arrObjCam[1].m_strPasswd= "your device password";

            // 将 listbox 控件的数据上下文设置为示例数据
            DataContext = App.ViewModel;
        }

        // 为 ViewModel 项加载数据
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (!App.ViewModel.IsDataLoaded)
            {
                App.ViewModel.LoadData();
            }
        }

        //----delegate method----------------------------------------------------
        private static Int32 OnLANSearchCallback(byte[] pData, UInt32 nDataSize)
        {
            SEARCH_RESP stResp=new SEARCH_RESP(pData);
            Debug.WriteLine("OnLANSearchCallback, nDataSize=" + nDataSize+" ip="+stResp.sIpAddr);
            return 0;
        }

        private static Int32 OnStreamCallback(string strDID, byte[] arrData, Int32 nDataSize)
        {
            //String str = String.Format("OnStreamCallback] {0} nDataSize={1}", strDID, nDataSize);
            //Debug.WriteLine(str);

            return 0;
        }
        private static Int32 OnRecvMsgCallback(string strDID, UInt32 nMsgType, byte[] arrMsg, UInt32 nDataSize)
        {
            String str = String.Format("OnRecvMsgCallback] {0} nMsgType=0x{1:X} nDataSize={2}", strDID, nMsgType, nDataSize);
            Debug.WriteLine(str);
            switch (nMsgType)
            {
                case SEP2P_ENUM_MSGTYPE.SEP2P_MSG_CONNECT_MODE:
                    int nConnMode = BitConverter.ToInt32(arrMsg, 0);
                    Debug.WriteLine("OnRecvMsgCallback] nConnMode="+nConnMode);
                    break;
                default:
                    break;
            }
            return 0;
        }
        private static Int32 OnEventCallback(string strDID, UInt32 nEventType, byte[] arrEvent, UInt32 nEventDataSize)
        {
            String str = String.Format("OnEventCallback] {0} nEventType={1} nEventDataSize={2}", strDID, nEventType, nEventDataSize);
            Debug.WriteLine(str);
            
            return 0;
        }

        //Some click
        void PageBackKeyPress(object sender, System.ComponentModel.CancelEventArgs e)
        {
            arrObjCam[0].P2PAPI_StopLiveAudio();
            arrObjCam[1].P2PAPI_StopLiveAudio();
            arrObjCam[0].P2PAPI_StopLiveVideo();
            arrObjCam[1].P2PAPI_StopLiveVideo();

            arrObjCam[0].P2PAPI_Disconnect();
            arrObjCam[1].P2PAPI_Disconnect();
            CamObj.P2PAPI_DeInit();
        }
        private void Exit_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            arrObjCam[0].P2PAPI_StopLiveAudio();
            arrObjCam[1].P2PAPI_StopLiveAudio();
            arrObjCam[0].P2PAPI_StopLiveVideo();
            arrObjCam[1].P2PAPI_StopLiveVideo();

            arrObjCam[0].P2PAPI_Disconnect();
            arrObjCam[1].P2PAPI_Disconnect();
            CamObj.P2PAPI_DeInit();
        }
        private void Button_Click(object sender, RoutedEventArgs e) //other test
        {
            if (arrObjCam[0].P2PAPI_IsConnect()) { 
                arrObjCam[0].P2PAPI_SendMsg(SEP2P_ENUM_MSGTYPE.SEP2P_MSG_GET_ALARM_INFO_REQ,null,0);
            }
        }

        private void Button_Click1(object sender, RoutedEventArgs e) //connect
        {
            arrObjCam[0].P2PAPI_Connect();
            arrObjCam[1].P2PAPI_Connect();
        }

        private void Button_Click2(object sender, RoutedEventArgs e)//disconnect
        {
            arrObjCam[0].P2PAPI_Disconnect();
            arrObjCam[1].P2PAPI_Disconnect();
        }

        private void Button_Click3(object sender, RoutedEventArgs e)//start video
        {
            //start video
            arrObjCam[0].P2PAPI_StartLiveVideo();
        }

        private void Button_Click4(object sender, RoutedEventArgs e)//stop video
        {
            //stop video
            arrObjCam[0].P2PAPI_StopLiveVideo();
        }

        private void Button_Click5(object sender, RoutedEventArgs e)//start audio
        {
            //start audio
            arrObjCam[0].P2PAPI_StartLiveAudio();
        }

        private void Button_Click6(object sender, RoutedEventArgs e)//stop audio
        {
            //stop audio
            arrObjCam[0].P2PAPI_StopLiveAudio();
        }

    }
}