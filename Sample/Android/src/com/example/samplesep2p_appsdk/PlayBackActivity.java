package com.example.samplesep2p_appsdk;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;

import com.p2p.MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP;
import com.p2p.MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP;
import com.p2p.MSG_START_PLAY_REC_FILE_RESP;
import com.p2p.MSG_STOP_PLAY_REC_FILE_RESP;
import com.p2p.REC_FILE_INFO;
import com.p2p.SEP2P_Define;

public class PlayBackActivity extends Activity implements IAVListener{
	private EditText recDayEdit,recDateEdit,recFilesEdit;
    private Button RecDayBtn,RecordFilesBtn,PlaybackBtn;
	private Spinner recordTypeSpinner;
	
	private String strRecDay,strRecDate,strRecFiles;
	
	private String m_strFilePlayback=null;
	private boolean bStartPlayback=false;
	
	String [] record_spinner_list = null;
	ArrayAdapter<String> spinnerAdapter = null;
	
	private int PLAY_BACK_TYPE = SEP2P_Define.RECORD_TYPE_PLAN;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.playback);
		init();
		MainActivity.m_arrObjCam[0].regAVListener(PlayBackActivity.this);
	}
	
	public void init(){
		record_spinner_list = new String[]
		{getResources().getString(R.string.plan_record),
				getResources().getString(R.string.alarm_record),
				getResources().getString(R.string.plan_alarm_record)};
		
		recDayEdit = (EditText)findViewById(R.id.recorddayedit);
		recDateEdit = (EditText)findViewById(R.id.recorddateedit);
		recFilesEdit = (EditText)findViewById(R.id.recordfilesedit);
		
		RecDayBtn = (Button)findViewById(R.id.getrecdaybtn);
		RecordFilesBtn = (Button)findViewById(R.id.getrecordfilesbtn);
		PlaybackBtn = (Button)findViewById(R.id.startplaybackbtn);
		
		recordTypeSpinner = (Spinner)findViewById(R.id.recordtypespinner);
		spinnerAdapter = new ArrayAdapter<String>(this,android.R.layout.simple_dropdown_item_1line,record_spinner_list);
		recordTypeSpinner.setAdapter(spinnerAdapter);
		
		recordTypeSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
				@Override
				public void onItemSelected(AdapterView<?> arg0, View arg1,int arg2, long arg3) {
					if(arg2 == 0) PLAY_BACK_TYPE = SEP2P_Define.RECORD_TYPE_PLAN;
					if(arg2 == 1) PLAY_BACK_TYPE = SEP2P_Define.RECORD_TYPE_ALARM;
					if(arg2 == 2) PLAY_BACK_TYPE = SEP2P_Define.RECORD_TYPE_ALL;
				}
				@Override
				public void onNothingSelected(AdapterView<?> arg0) {
				}
			});
	}
	
	
	public void onClick (View v){
		switch(v.getId()){
		
		/**
		 * 返回
		 */
		case R.id.back:
			finish();
			break;
		
			/**
			 * 通过年月获得下面有哪些天有录像文件
			 */
		case R.id.getrecdaybtn:
			strRecDay = recDayEdit.getText().toString();
			System.out.println("PlaybackActivity.java] request:"+strRecDay);
			if(!strRecDay.equals("")){
				MainActivity.m_arrObjCam[0].getRemote_rec_day_by_yyyymm(Integer.parseInt(strRecDay),3);
			}
			break;
			
			/**
			 * 通过年月日及录象类型获得所有的录像文件
			 */
		case R.id.getrecordfilesbtn:
			strRecDate = recDateEdit.getText().toString();
			System.out.println("PlaybackActivity.java] request: record files on "+strRecDate+",recordType="+PLAY_BACK_TYPE);
			if(!strRecDate.equals("")){
				MainActivity.m_arrObjCam[0].getRemote_rec_file_by_yyyymmdd(Integer.parseInt(strRecDate),PLAY_BACK_TYPE, 0,0);
			}
			break;
			
			/**
			 * 开始启动与停止录像回放
			 */
		case R.id.startplaybackbtn:
			strRecFiles = recFilesEdit.getText().toString();
			if(bStartPlayback){
				bStartPlayback = false;
				PlaybackBtn.setText(getResources().getString(R.string.start_rec_play));
				if(!strRecFiles.equals("")){
					MainActivity.m_arrObjCam[0].stopPlayback(strRecFiles);
				}
				
			}else{
				if(m_strFilePlayback==null) break;
				bStartPlayback=true;
				PlaybackBtn.setText(getResources().getString(R.string.stop_rec_play));
				if(!strRecFiles.equals("")){
					MainActivity.m_arrObjCam[0].startPlayback(0, 0, strRecFiles);
				}
			}
			break;
		}
	}
	
	Handler MsgPlaybackHandler = new Handler() {
		@SuppressLint("HandlerLeak")
		public void handleMessage(Message msg) {
			switch (msg.what) {
				case SEP2P_Define.SEP2P_MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP:{
					if(msg.obj==null) break;
					byte[] byts=(byte[])msg.obj;
					MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP day_by_month = new MSG_GET_REMOTE_REC_DAY_BY_MONTH_RESP(byts);
					System.out.println("PlaybackActivity.java] response:"+day_by_month.getnChDay()+" of "+day_by_month.getnYearMon());
					}
					break;
				
				case SEP2P_Define.SEP2P_MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP:{
					if(msg.obj==null) break;
					byte[] byts=(byte[])msg.obj;
					MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP stResp=new MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP(byts);
					stResp.getnResult();
					stResp.getnFileTotalNum();
					System.out.println("PlaybackActivity.java] response: result="+stResp.getnResult()+",TotalNum="+stResp.getnFileTotalNum()+
							", range of no=["+stResp.getnBeginNoOfThisTime()+","+stResp.getnEndNoOfThisTime()+"]");
					REC_FILE_INFO stRecFileInfo=null;
					String str=null;
					int i=0, nFileNumOfThisTime=stResp.getnEndNoOfThisTime()-stResp.getnBeginNoOfThisTime()+1;
					for(i=0; i<nFileNumOfThisTime; i++){
						stRecFileInfo=new REC_FILE_INFO(byts, MSG_GET_REMOTE_REC_FILE_BY_DAY_RESP.MY_LEN+i*REC_FILE_INFO.MY_LEN);
						str=String.format("  %d, type=%d %dsec %dKB, %s", i, stRecFileInfo.getchnRecType(), 
										  stRecFileInfo.getnTimeLen_sec(), 
										  stRecFileInfo.getnFileSize_KB(),
										  stRecFileInfo.getchFilePath());
						System.out.println(str);
						if(i==0) m_strFilePlayback=stRecFileInfo.getchFilePath();//for test
					}
					recFilesEdit.setText(m_strFilePlayback);
					}break;
					
				case SEP2P_Define.SEP2P_MSG_START_PLAY_REC_FILE_RESP:{
					if(msg.obj==null) break;
					byte[] byts=(byte[])msg.obj;
					String str=null;
					MSG_START_PLAY_REC_FILE_RESP stResp=new MSG_START_PLAY_REC_FILE_RESP(byts);
					str=String.format("result=%d aparam=%d file=%s", stResp.getnResult(), stResp.getnAudioParam(), stResp.getchFilePath());
					System.out.println("StartPlayback]"+str);
					}break;
	
				case SEP2P_Define.SEP2P_MSG_STOP_PLAY_REC_FILE_RESP:{
					if(msg.obj==null) break;
					byte[] byts=(byte[])msg.obj;
					String str=null;
					MSG_STOP_PLAY_REC_FILE_RESP stResp = new MSG_STOP_PLAY_REC_FILE_RESP(byts);
					System.out.println("StopPlayback]"+ stResp.getnResult());
					}
					break;
				default:;
			}
		}
	};

	@Override
	public void updateFrameInfo(Object obj, int nWidth, int nHeigh) {
	}
	@Override
	public void updateBmpFrame(Object obj, byte[] rawVideoData, Bitmap bmp) {
	}
	@Override
	public void updateMsg(Object obj, int nMsgType, byte[] pMsg, int nMsgSize,int pUserData) {
		Message msg = MsgPlaybackHandler.obtainMessage();
		msg.what= nMsgType;
		msg.obj = pMsg;
		MsgPlaybackHandler.sendMessage(msg);
	}
}
