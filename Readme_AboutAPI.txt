
说明：
App与Camera交互要点:
======================================================================================================
1>App发送消息给Camera,  使用INT32 SEP2P_SendMsg(const CHAR* pDID, INT32 nMsgType, const CHAR* pMsgData, INT32 nMsgDataSize); API
2>Camera发送消息给App, 通过回调函数 POnRecvMsgCallback
  如果Camera有报警事件，则通过回调函数 POnEventCallback
  android是: SEP2P_AppSDK.SEP2P_SendMsg发送消息，CamObj.onMsg接收到IPCamera的消息

3>发送的消息类型及消息内容都在SEP2P_Define.h里有定义(android: com.p2p.SEP2P_Define)
	3.1>与WiFi有关的消息有:
	  SEP2P_MSG_GET_CURRENT_WIFI_REQ	//app--->ipcamera 向IPCamera请求当前连线的wifi
	  SEP2P_MSG_GET_CURRENT_WIFI_RESP	//ipcamera--->App 
	  SEP2P_MSG_SET_CURRENT_WIFI_REQ	//app--->ipcamera 向IPCamera请求设置当前wifi
	  SEP2P_MSG_SET_CURRENT_WIFI_RESP	//ipcamera--->App
	  SEP2P_MSG_GET_WIFI_LIST_REQ		//app--->ipcamera 向IPCamera请求IPCamera搜索周边的wifi
	  SEP2P_MSG_GET_WIFI_LIST_RESP		//ipcamera--->App 
	  
	  android的wifi sample见：MainActivity.java, public void btnClick(View v) {
					......
					case R.id.btn_other_test:
					......
					}
	3.2>远程回放SD/TF卡上的录像:
	  Sample界面上填写DID等信息后 --> 点击Connect --> 点击SendCommand --> 选择getRecFileListBy
	  --> 选择OK -->会跳到远程回放界面 --> 选择有录像的日期后，就会列出SD/TF卡上的录像文件
	  --> 选中某个文件，就开始远程回放了。

4>SEP2P_SendMsg返回值>=0是表示发送成功，并不表示Camera执行成功，
Camera执行成功后，会从回调POnRecvMsgCallback(见SEP2P_API.h)收到相应回复消息(…RESP)。

5>所有消息(在SEP2P_Define.h)都是SEP2P_打头的, ……_REQ(值为偶数), 表示App发给Camera的消息；……_RESP(值为奇数)表示接收到Camera的消息。
如控制Camera参数的消息: SEP2P_MSG_SET_CAMERA_PARAM_REQ
	 说明:获取Camera的一些参数，如: nResolution, nBright, …, OSD是否显示等等
	 如设置亮度:
		MSG_SET_CAMERA_PARAM_REQ stReq;
		memset(&stReq, 0, sizeof(stReq));
		stReq.nBright=100;
		stReq.nBitMaskToSet= BIT_MASK_CAM_PARAM_BRIGHT;
		SEP2P_SendMsg(DID_of_device, SEP2P_MSG_SET_CAMERA_PARAM_REQ, (CHAR *)&stReq, sizeof(stReq));

	如同时设置亮度，对比度
		MSG_SET_CAMERA_PARAM_REQ stReq;
		memset(&stReq, 0, sizeof(stReq));
		stReq.nBright=100;
		stReq.nContrast=100
		stReq.nBitMaskToSet= BIT_MASK_CAM_PARAM_BRIGHT| BIT_MASK_CAM_PARAM_CONTRAST;
		SEP2P_SendMsg(DID_of_device, SEP2P_MSG_SET_CAMERA_PARAM_REQ, (CHAR *)&stReq, sizeof(stReq));
 6>其他消息的交互都是类似的

