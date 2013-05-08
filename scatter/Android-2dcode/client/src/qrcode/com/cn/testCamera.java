package qrcode.com.cn;

import java.util.Timer;
import java.util.TimerTask;

import qrcode.com.cn.R;

//import qrcode.com.cn.R;

import com.google.zxing.BinaryBitmap;
import com.google.zxing.MultiFormatReader;
import com.google.zxing.Result;
import com.google.zxing.Android.PlanarYUVLuminanceSource;
import com.google.zxing.common.HybridBinarizer;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceView;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

//二维码扫描

public class testCamera extends Activity {
	/** Called when the activity is first created. */
	private SurfaceView sfvCamera;
	private SFHCamera sfhCamera;
	private ImageView imgView;
	private View centerView;
	private TextView txtScanResult;
	private Timer mTimer;
	private MyTimerTask mTimerTask;
	// 按照标准HVGA
	final static int width = 480;
	final static int height = 320;
	int dstLeft, dstTop, dstWidth, dstHeight;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.scanner); //展示二维码扫描界面
		
		this.setTitle("二维码识别读取");
		imgView = (ImageView) this.findViewById(R.id.ImageView01);
		centerView = (View) this.findViewById(R.id.centerView);
		sfvCamera = (SurfaceView) this.findViewById(R.id.sfvCamera);
		txtScanResult=(TextView)this.findViewById(R.id.txtScanResult);
		
		sfhCamera = new SFHCamera(sfvCamera.getHolder(), width, height, previewCallback); //调用SFHCamera
		
		
		// 初始化定时器
		mTimer = new Timer();
		mTimerTask = new MyTimerTask();
		mTimer.schedule(mTimerTask, 0, 80);
		
		//返回按钮设计
	    Button back = (Button)findViewById(R.id.back_2);
	    back.setOnClickListener(new Button.OnClickListener()
	    {
	       @Override
	       public void onClick(View arg0)
	       {
	         // TODO Auto-generated method stub
	         Intent intent=new Intent();
	         intent.setClass(testCamera.this, CQRcode.class);
	         startActivity(intent);
	         testCamera.this.finish();
	    	
	       }
	    });
			
	}
	
	class MyTimerTask extends TimerTask {
		@Override
		public void run() {
			if (dstLeft == 0) {//只赋值一次
				dstLeft = centerView.getLeft() * width
						/ getWindowManager().getDefaultDisplay().getWidth();
				dstTop = centerView.getTop() * height
						/ getWindowManager().getDefaultDisplay().getHeight();
				dstWidth = (centerView.getRight() - centerView.getLeft())* width
						/ getWindowManager().getDefaultDisplay().getWidth();
				dstHeight = (centerView.getBottom() - centerView.getTop())* height
						/ getWindowManager().getDefaultDisplay().getHeight();
			}
			sfhCamera.AutoFocusAndPreviewCallback();
		}
	}

	//显示二维码解码后的文本信息
    private void openOptionsDialog(String s)
    {
      new AlertDialog.Builder(this)
      .setTitle(R.string.scanner_result_title)
      .setMessage(s)
      .setPositiveButton(R.string.str_ok,
          new DialogInterface.OnClickListener()
          {
           public void onClick(DialogInterface dialoginterface, int i)
           {
           }
           }
          )
      .show();
    }
	
	
	/**
	 *  自动对焦后输出生成信息
	 */
	private Camera.PreviewCallback previewCallback = new Camera.PreviewCallback() {
		@Override
		public void onPreviewFrame(byte[] data, Camera arg1) {
			//取得指定范围的帧的数据
			PlanarYUVLuminanceSource source = new PlanarYUVLuminanceSource(
					data, width, height, dstLeft, dstTop, dstWidth, dstHeight);
			//取得灰度图
			Bitmap mBitmap = source.renderCroppedGreyscaleBitmap();
			//显示灰度图
			imgView.setImageBitmap(mBitmap);
			BinaryBitmap bitmap = new BinaryBitmap(new HybridBinarizer(source));
			MultiFormatReader reader = new MultiFormatReader();
			try {
				Result result = reader.decode(bitmap); //由图片信息转码得到文本信息
				/*
				String strResult = "BarcodeFormat:"
						+ result.getBarcodeFormat().toString() + "  text:"
						+ result.getText();
				txtScanResult.setText(strResult);	
				*/
				
				openOptionsDialog(result.getText()); //扫描结果显示输出到这里

			} catch (Exception e) {
				txtScanResult.setText("Scanning...");
			}
		}
	};

}