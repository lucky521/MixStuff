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

//��ά��ɨ��

public class testCamera extends Activity {
	/** Called when the activity is first created. */
	private SurfaceView sfvCamera;
	private SFHCamera sfhCamera;
	private ImageView imgView;
	private View centerView;
	private TextView txtScanResult;
	private Timer mTimer;
	private MyTimerTask mTimerTask;
	// ���ձ�׼HVGA
	final static int width = 480;
	final static int height = 320;
	int dstLeft, dstTop, dstWidth, dstHeight;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.scanner); //չʾ��ά��ɨ�����
		
		this.setTitle("��ά��ʶ���ȡ");
		imgView = (ImageView) this.findViewById(R.id.ImageView01);
		centerView = (View) this.findViewById(R.id.centerView);
		sfvCamera = (SurfaceView) this.findViewById(R.id.sfvCamera);
		txtScanResult=(TextView)this.findViewById(R.id.txtScanResult);
		
		sfhCamera = new SFHCamera(sfvCamera.getHolder(), width, height, previewCallback); //����SFHCamera
		
		
		// ��ʼ����ʱ��
		mTimer = new Timer();
		mTimerTask = new MyTimerTask();
		mTimer.schedule(mTimerTask, 0, 80);
		
		//���ذ�ť���
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
			if (dstLeft == 0) {//ֻ��ֵһ��
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

	//��ʾ��ά��������ı���Ϣ
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
	 *  �Զ��Խ������������Ϣ
	 */
	private Camera.PreviewCallback previewCallback = new Camera.PreviewCallback() {
		@Override
		public void onPreviewFrame(byte[] data, Camera arg1) {
			//ȡ��ָ����Χ��֡������
			PlanarYUVLuminanceSource source = new PlanarYUVLuminanceSource(
					data, width, height, dstLeft, dstTop, dstWidth, dstHeight);
			//ȡ�ûҶ�ͼ
			Bitmap mBitmap = source.renderCroppedGreyscaleBitmap();
			//��ʾ�Ҷ�ͼ
			imgView.setImageBitmap(mBitmap);
			BinaryBitmap bitmap = new BinaryBitmap(new HybridBinarizer(source));
			MultiFormatReader reader = new MultiFormatReader();
			try {
				Result result = reader.decode(bitmap); //��ͼƬ��Ϣת��õ��ı���Ϣ
				/*
				String strResult = "BarcodeFormat:"
						+ result.getBarcodeFormat().toString() + "  text:"
						+ result.getText();
				txtScanResult.setText(strResult);	
				*/
				
				openOptionsDialog(result.getText()); //ɨ������ʾ���������

			} catch (Exception e) {
				txtScanResult.setText("Scanning...");
			}
		}
	};

}