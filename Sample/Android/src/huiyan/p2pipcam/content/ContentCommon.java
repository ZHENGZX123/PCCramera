package huiyan.p2pipcam.content;

public class ContentCommon {
	public static final String STR_WINDOW_ID_MORE ="camera_more_window";
	public static final String STR_WINDOW_ID_LIST ="camera_list_window";
	
	public static final String STR_CAMERA_INFO_RECEIVER = "object.ipcam.client.camerainforeceiver";
	public static final String STR_CAMERA_ADDR 		= "camera_addr";
	public static final String STR_CAMERA_PORT 		= "camera_port";
	public static final String STR_CAMERA_NAME 		= "camera_name";
	public static final String STR_CAMERA_MAC 		= "camera_mac";
	public static final String STR_CAMERA_USER		= "camera_user";
	public static final String STR_CAMERA_PWD 		= "camera_pwd";
	public static final String STR_CAMERA_ID  		= "cameraid";
	public static final String STR_CAMERA_SNAPSHOT 	= "camera_snapshot";
	public static final String STR_CAMERA_USER_AUTHORITY = "camera_user_authority";

	public static final String STR_CAMERA_OLD_ADDR = "camera_old_addr";
	public static final String STR_CAMERA_OLD_PORT = "camera_old_port";
	public static final String STR_CAMERA_OLD_ID   = "camera_old_id";

	public static final String STR_CAMERA_TYPE		= "camera_type";
	public static final String STR_STREAM_TYPE		= "stream_type";
	public static final String STR_H264_MAIN_STREAM = "h264_main_stream";
	public static final String STR_H264_SUB_STREAM	= "h264_sub_stream";
	public static final String STR_MJPEG_SUB_STREAM = "mjpeg_sub_stream";

	public static final String STR_CAMOBJ_INDEX		="camobj_index";
	public static final String STR_CAMOBJ			="camobj"; 
	
	public static final int DEFAULT_PORT 			= 81;
	public static final String DEFAULT_USER_NAME	= "admin";
	public static final String DEFAULT_USER_PWD		= "";

	public static final String CAMERA_OPTION 	= "camera_option";
	public static final int ADD_CAMERA			= 1;
	public static final int EDIT_CAMERA			= 2;
	public static final int CHANGE_CAMERA_USER 	= 3;
	public static final int DEL_CAMERA 			= 4;
	public static final int INVALID_OPTION 		= 0xffff;

	public static final int CAMERA_TYPE_UNKNOW = 0;
	public static final int CAMERA_TYPE_MJPEG  = 1;
	public static final int CAMERA_TYPE_H264   = 2;

	public static final int H264_MAIN_STREAM = 0;
	public static final int H264_SUB_STREAM  = 1;
	public static final int MJPEG_SUB_STREAM = 3;

	public static final int L_SERIES  		=0x4C; 
	public static final String L_SERIES_STR ="L";
	public static final int M_SERIES_1		=0x4D01;

	public static final int MOTION_ALARM	= 0x01;
	public static final int GPIO_ALARM 		= 0x02;
}