package com.example.samplesep2p_appsdk;

import huiyan.p2pipcam.content.ContentCommon;
import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.graphics.PointF;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;
import android.view.View.OnTouchListener;
import android.widget.ImageView;
import com.p2p.SEP2P_Define;

public class TouchedView extends ImageView implements IAVListener, OnTouchListener,
		GestureDetector.OnGestureListener {
	private static final int WHAT_updateMVFrame = 1;
	private static final int WHAT_updateLVFrame = 2;
	private static final int WHAT_requestMyLayout = 3;
	private static final int WHAT_updateVWhenStop = 4;
	public static final float MIN_SCALE = 1.0f;
	public static final float MAX_SCALE = 3.0f;
	public static final int NONE = 0;
	public static final int DRAG = 1;
	public static final int ZOOM = 2;
	public static final int FLING_MIN_DISTANCE = 100;
	public static final int FLING_MIN_VELOCITY = 0;
	public static final int CLICK = 3;
	private Bitmap mLastFrame;
	private Paint mVideoPaint = new Paint();
	private float redundantXSpace = 0.0f, redundantYSpace = 0.0f;
	private float m_viewWidth = 0.0f, m_viewHeight = 0.0f;
	private float saveScale = 1.0f;
	private float right, bottom, origWidth, origHeight;
	private int m_bmWidth = -1, m_bmHeight = -1;
	private PointF mPointFLast = new PointF();
	private PointF mPointFStart = new PointF();
	@SuppressWarnings("unused")
	private Context mContext = null;
	private Canvas canvas = null;
	private Paint mPaint = new Paint();
	private CamObj m_curCamera = null;
	private Matrix mMatrix = new Matrix();
	private float[] mArrFloat = null;
	private boolean mIsZooming = false;
	private int mTouchMode = NONE;
	private GestureDetector mGestureDetector;
	private ScaleGestureDetector mScaleDetector;
	public String str_did;
	@SuppressWarnings("unused")
	private GestureDetector gesturedetector;
	/** �Ƿ�ץ�� **/
	public static boolean isTakepic = false;
	public static boolean isRecording = false;
	public static int[] pprecordHandle = new int[1];
	/** �Ƿ�¼�� **/
	public boolean isTakeVideo = false;
	public static Bitmap mDefaultBmp = null;
	public static int nStreamCodecType;

	@SuppressWarnings("deprecation")
	public TouchedView(Context context, AttributeSet attrs) {
		super(context, attrs);
		this.mContext = context;
		mVideoPaint.setARGB(255, 200, 200, 200);
		mGestureDetector = new GestureDetector(
				(android.view.GestureDetector.OnGestureListener) this);
		mScaleDetector = new ScaleGestureDetector(context, new ScaleListener());
		mMatrix.setTranslate(1.0f, 1.0f);
		mArrFloat = new float[9];
		setImageMatrix(mMatrix);
		setScaleType(ScaleType.MATRIX);
		this.setLongClickable(true);
		this.setOnTouchListener(this);
		if(mDefaultBmp == null) mDefaultBmp = BitmapFactory.decodeResource(getResources(),
				R.drawable.ic_launcher);
	}

	public void attachCamera(CamObj camObj) {
		m_curCamera = camObj;
		if(m_curCamera != null) {
			m_curCamera.regAVListener(this);
		}
	}

	public void deattachCamera() {
		if(m_curCamera != null) {
			m_curCamera.unregAVListener(this);
		}
	}

	public synchronized Bitmap getLastFrame() {
		return mLastFrame;
	}

	public void ctrlPTZ(String did, int nCmd) {
		if(m_curCamera != null) m_curCamera.PTZCtrol(nCmd, 0);
	}

	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
		m_viewWidth = MeasureSpec.getSize(widthMeasureSpec);
		m_viewHeight = MeasureSpec.getSize(heightMeasureSpec);
		float scale;
		float scaleX = (float) m_viewWidth / (float) m_bmWidth;
		float scaleY = (float) m_viewHeight / (float) m_bmHeight;
		scale = Math.min(scaleX, scaleY);
		mMatrix.setScale(scale, scale);
		setImageMatrix(mMatrix);
		saveScale = 1f;
		redundantYSpace = (float) m_viewHeight - (scale * (float) m_bmHeight);
		redundantXSpace = (float) m_viewWidth - (scale * (float) m_bmWidth);
		redundantYSpace /= (float) 2;
		redundantXSpace /= (float) 2;
		mMatrix.postTranslate(redundantXSpace, redundantYSpace);
		origWidth = m_viewWidth - 2 * redundantXSpace;
		origHeight = m_viewHeight - 2 * redundantYSpace;
		right = m_viewWidth * saveScale - m_viewWidth - (2 * redundantXSpace * saveScale);
		bottom = m_viewHeight * saveScale - m_viewHeight - (2 * redundantYSpace * saveScale);
		setImageMatrix(mMatrix);
	}

	private class ScaleListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
		@Override
		public boolean onScaleBegin(ScaleGestureDetector detector) {
			mTouchMode = ZOOM;
			return true;
		}

		@Override
		public boolean onScale(ScaleGestureDetector detector) {
			float mScaleFactor = (float) Math.min(Math.max(.95f, detector.getScaleFactor()), 1.05);
			float origScale = saveScale;
			saveScale *= mScaleFactor;
			mTouchMode = ZOOM;
			if(saveScale > MAX_SCALE) {
				saveScale = MAX_SCALE;
				mScaleFactor = MAX_SCALE / origScale;
			} else if(saveScale < MIN_SCALE) {
				saveScale = MIN_SCALE;
				mScaleFactor = MIN_SCALE / origScale;
			}
			right = m_viewWidth * saveScale - m_viewWidth - (2 * redundantXSpace * saveScale);
			bottom = m_viewHeight * saveScale - m_viewHeight - (2 * redundantYSpace * saveScale);
			mIsZooming = mScaleFactor != 1.0f;
			if(origWidth * saveScale <= m_viewWidth || origHeight * saveScale <= m_viewHeight) {
				mMatrix.postScale(mScaleFactor, mScaleFactor, m_viewWidth / 2, m_viewHeight / 2);
				if(mScaleFactor < 1) {
					mMatrix.getValues(mArrFloat);
					float x = mArrFloat[Matrix.MTRANS_X];
					float y = mArrFloat[Matrix.MTRANS_Y];
					if(mScaleFactor < 1) {
						if(Math.round(origWidth * saveScale) < m_viewWidth) {
							if(y < -bottom) mMatrix.postTranslate(0, -(y + bottom));
							else if(y > 0) mMatrix.postTranslate(0, -y);
						} else {
							if(x < -right) mMatrix.postTranslate(-(x + right), 0);
							else if(x > 0) mMatrix.postTranslate(-x, 0);
						}
					}
				}
			} else {
				mMatrix.postScale(mScaleFactor, mScaleFactor, detector.getFocusX(),	detector.getFocusY());
				mMatrix.getValues(mArrFloat);
				float x = mArrFloat[Matrix.MTRANS_X];
				float y = mArrFloat[Matrix.MTRANS_Y];
				if(mScaleFactor < 1) {
					if(x < -right) mMatrix.postTranslate(-(x + right), 0);
					else if(x > 0) mMatrix.postTranslate(-x, 0);
					if(y < -bottom) mMatrix.postTranslate(0, -(y + bottom));
					else if(y > 0) mMatrix.postTranslate(0, -y);
				}
				mIsZooming = true;
			}
			return true;
		}
	}

	@SuppressLint("HandlerLeak")
	private Handler handler = new Handler() {
		@Override
		public void handleMessage(Message msg) {
			super.handleMessage(msg);
			switch (msg.what) {
				case WHAT_requestMyLayout: {
					canvas = null;
					requestLayout();
				}
					break;
				case WHAT_updateMVFrame: {
					Bundle bundle = msg.getData();
					Bitmap bmp = (Bitmap) bundle.getParcelable("image");
					mLastFrame = bmp;
					if(canvas == null) {
						canvas = new Canvas();
						setImageBitmap(mLastFrame);
					}
					canvas.drawBitmap(mLastFrame, 0, 0, mPaint);
					TouchedView.this.invalidate();
				}
					break;

				case WHAT_updateLVFrame: {
					Bundle bundle = msg.getData();
					Bitmap bmp = (Bitmap) bundle.getParcelable("limage");
					mLastFrame = bmp;
					if(canvas == null) {
						canvas = new Canvas();
						setImageBitmap(mLastFrame);
					}
					canvas.drawBitmap(mLastFrame, 0, 0, mPaint);
					TouchedView.this.invalidate();
				}
					break;

				case WHAT_updateVWhenStop:
					if(m_bmWidth > 0 && m_bmHeight > 0) {
						Bitmap bitmap = Bitmap.createBitmap(m_bmWidth, m_bmHeight,
								mDefaultBmp.getConfig());
						canvas = new Canvas(bitmap);
						setImageBitmap(bitmap);
						canvas.drawBitmap(mDefaultBmp, 0, 0, new Paint());
						TouchedView.this.invalidate();
					}
					break;
				default:
					;
			}
		}
	};

	protected void requestMyLayout() {
		Message msg = new Message();
		msg.what = WHAT_requestMyLayout;
		handler.sendMessage(msg);
	}

	protected void updateVWhenStop() {
		Message message = new Message();
		message.what = WHAT_updateVWhenStop;
		handler.sendMessage(message);
	}

	protected void updateMVFrame(Bitmap bmp) {
		Message message = new Message();
		message.what = WHAT_updateMVFrame;
		Bundle data = new Bundle();
		data.putParcelable("image", bmp);
		message.setData(data);
		handler.sendMessage(message);
	}

	protected void updateLVFrame(Bitmap bmp) {
		Message message = new Message();
		message.what = WHAT_updateLVFrame;
		Bundle data = new Bundle();
		data.putParcelable("limage", bmp);
		message.setData(data);
		handler.sendMessage(message);
	}

	@Override
	public boolean onDown(MotionEvent e) {
		return false;
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) {
		if(mIsZooming || mTouchMode == ZOOM) return false;
		System.out.println("velocityX: " + Math.abs(velocityX) + ", velocityY: "+ Math.abs(velocityY));
		if(e1.getX() - e2.getX() > FLING_MIN_DISTANCE && Math.abs(velocityX) > FLING_MIN_VELOCITY) {
			if(m_curCamera != null) m_curCamera.PTZCtrol(SEP2P_Define.PTZ_CTRL_RIGHT, 0);
		} else if(e2.getX() - e1.getX() > FLING_MIN_DISTANCE
				&& Math.abs(velocityX) > FLING_MIN_VELOCITY) {
			if(m_curCamera != null) m_curCamera.PTZCtrol(SEP2P_Define.PTZ_CTRL_LEFT, 0);
		}
		if(e1.getY() - e2.getY() > FLING_MIN_DISTANCE && Math.abs(velocityY) > FLING_MIN_VELOCITY) {
			if(m_curCamera != null) m_curCamera.PTZCtrol(SEP2P_Define.PTZ_CTRL_DOWN, 0);
		} else if(e2.getY() - e1.getY() > FLING_MIN_DISTANCE
				&& Math.abs(velocityY) > FLING_MIN_VELOCITY) {
			if(m_curCamera != null) m_curCamera.PTZCtrol(SEP2P_Define.PTZ_CTRL_UP, 0);
		}
		return true;
	}

	@Override
	public void onLongPress(MotionEvent e) {
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) {
		return false;
	}

	@Override
	public void onShowPress(MotionEvent e) {
	}

	@Override
	public boolean onSingleTapUp(MotionEvent e) {
		return false;
	}

	@Override
	public boolean onTouch(View v, MotionEvent event) {
		mScaleDetector.onTouchEvent(event);
		mGestureDetector.onTouchEvent(event);
		mMatrix.getValues(mArrFloat);
		float x = mArrFloat[Matrix.MTRANS_X];
		float y = mArrFloat[Matrix.MTRANS_Y];
		PointF curr = new PointF(event.getX(), event.getY());

		switch (event.getAction()) {
			case MotionEvent.ACTION_DOWN:
				mPointFLast.set(event.getX(), event.getY());
				mPointFStart.set(mPointFLast);
				mTouchMode = DRAG;
				break;

			case MotionEvent.ACTION_MOVE:
				if(mTouchMode == DRAG) {
					float deltaX = curr.x - mPointFLast.x;
					float deltaY = curr.y - mPointFLast.y;
					float scaleWidth = Math.round(origWidth * saveScale);
					float scaleHeight = Math.round(origHeight * saveScale);
					if(scaleWidth < m_viewWidth) {
						deltaX = 0;
						if(y + deltaY > 0) deltaY = -y;
						else if(y + deltaY < -bottom) deltaY = -(y + bottom);

					} else if(scaleHeight < m_viewHeight) {
						deltaY = 0;
						if(x + deltaX > 0) deltaX = -x;
						else if(x + deltaX < -right) deltaX = -(x + right);

					} else {
						if(x + deltaX > 0) deltaX = -x;
						else if(x + deltaX < -right) deltaX = -(x + right);

						if(y + deltaY > 0) deltaY = -y;
						else if(y + deltaY < -bottom) deltaY = -(y + bottom);
					}
					mMatrix.postTranslate(deltaX, deltaY);
					mPointFLast.set(curr.x, curr.y);
				}
				break;

			case MotionEvent.ACTION_UP:
				mTouchMode = NONE;
				int xDiff = (int) Math.abs(curr.x - mPointFStart.x);
				int yDiff = (int) Math.abs(curr.y - mPointFStart.y);
				if(xDiff < CLICK && yDiff < CLICK) performClick();
				break;

			case MotionEvent.ACTION_POINTER_UP:
				mTouchMode = NONE;
				break;
		}
		setImageMatrix(mMatrix);
		invalidate();
		return true;
	}

	@Override
	public void updateFrameInfo(Object obj, int nWidth, int nHeigh) {
		m_bmWidth = nWidth;
		m_bmHeight = nHeigh;
		requestMyLayout();
	}

	@Override
	public void updateBmpFrame(Object obj, byte[] rawVideoData, Bitmap bmp) {
		int nSeries = m_curCamera.getProductSeriesInt();
		if(nSeries == ContentCommon.L_SERIES) updateLVFrame(bmp);
		else updateMVFrame(bmp);
	}

	@Override
	public void updateMsg(Object obj, int nMsgType, byte[] pMsg, int nMsgSize, int pUserData) {
	}

}