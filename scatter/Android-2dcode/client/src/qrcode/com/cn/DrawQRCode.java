package qrcode.com.cn;
import qrcode.com.cn.HttpConnectionUtil.HttpConnectionCallback;
import qrcode.com.cn.HttpConnectionUtil.HttpMethod;
import android.app.Activity;
import android.content.Intent;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Rect;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.Window;
import android.widget.Button;
import android.widget.TextView;

//���ɶ�ά��ͼ�Σ������ı���Ϣ���뵽���ݿ��С�

public  class DrawQRCode extends Activity implements SurfaceHolder.Callback
{
  private String TAG = "HIPPO"; 
  private SurfaceView mSurfaceView01;
  private SurfaceHolder mSurfaceHolder01;
  private TextView tv; 
  private String codetxt;
  
  public String sort,name,occupation,fixedphone,mobilephone;
  
  /** Called when the activity is first created. */
  @Override
  public void onCreate(Bundle savedInstanceState)
  {
    super.onCreate(savedInstanceState);
    
    //ʹӦ�ó���ȫ��Ļ����
    requestWindowFeature(Window.FEATURE_NO_TITLE);
   
    setContentView(R.layout.drawqrcode);
    
    //�����Ļ��������
    DisplayMetrics dm = new DisplayMetrics();
    getWindowManager().getDefaultDisplay().getMetrics(dm);
    //��SurfaceView��Ϊ���Preview֮��
    mSurfaceView01 = (SurfaceView) findViewById(R.id.mSurfaceView1);
    //��SurfaceView
    mSurfaceHolder01 = mSurfaceView01.getHolder();
    mSurfaceHolder01.addCallback(DrawQRCode.this);
    
    Bundle bundle = this.getIntent().getExtras();
    
    tv=(TextView)findViewById(R.id.result);

    name=bundle.getString("name");
    occupation=bundle.getString("occupation");
    fixedphone=bundle.getString("fixedphone");
    mobilephone=bundle.getString("mobilephone");

    codetxt = "���ƣ�"+ name + "\n���أ�" + occupation + "\n����ʱ�䣺" + fixedphone+ "\n��ϵ�绰��" + mobilephone;
    tv.setText("������Ĳ�Ʒ��Ϣ���£�\n" + codetxt);

    
    //���ɶ�ά�밴ť�����
    Button draw = (Button)findViewById(R.id.draw);
     draw.setOnClickListener(new Button.OnClickListener()
     {
       @Override
       public void onClick(View arg0)
       {
         // TODO Auto-generated method stub
    	 AndroidQREncode(codetxt,10);	//������ά��
    	   
    	
    	//����������ݲ��뵽�������˵����ݿ�
    	String u = "status=" + name + "&name=" + occupation + "&tool=" + fixedphone + "&number=" + mobilephone;

		String url = getResources().getString(R.string.server_host) + "index.php/Index/insert?" + u;
		HttpConnectionUtil connUtil = new HttpConnectionUtil();
		
		connUtil.asyncConnect(url, HttpMethod.POST,
				new HttpConnectionCallback()
				{
					@Override
					public void execute(String response)
					{
						
					}
				});
    	    
       }
     });
     
     
     //���ذ�ť�����
     Button back = (Button)findViewById(R.id.back);
     back.setOnClickListener(new Button.OnClickListener()
     {
       @Override
       public void onClick(View arg0)
       {
         // TODO Auto-generated method stub
         Intent intent=new Intent();
         intent.setClass(DrawQRCode.this,CQRcode.class);
        startActivity(intent);
        DrawQRCode.this.finish();
       }
     });
     
  }
  
  
  //�Զ���QRCode����
  public void AndroidQREncode(String strEncoding, int qrcodeVersion)
  {
    try
    {
      //����QRCode�������
      com.swetake.util.Qrcode testQrcode = 
      new com.swetake.util.Qrcode();
      
      /* L','M','Q','H' */
      testQrcode.setQrcodeErrorCorrect('M');
      /* "N","A" or other */
      testQrcode.setQrcodeEncodeMode('B');
      /* 0-20 */
      testQrcode.setQrcodeVersion(qrcodeVersion);
      
      // getBytes
      byte[] bytesEncoding = strEncoding.getBytes("utf-8");
      
      if (bytesEncoding.length>0)
      {
        //ת����boolean����
        boolean[][] bEncoding = testQrcode.calQrcode(bytesEncoding);
        
        //��ͼ
        drawQRCode(bEncoding, getResources().getColor(R.drawable.black));
      }
    }
    catch (Exception e)
    {
      e.printStackTrace();
    }
  }
  
  //��SurfaceView�ϻ���QRCode������
  private void drawQRCode(boolean[][] bRect, int colorFill)
  {
    /* test Canvas*/
    int intPadding = 20;
    
  //��ͼǰ������Surfaceholder
    Canvas mCanvas01 = mSurfaceHolder01.lockCanvas();
  //���û�ͼ��ͼ��ɫ
    mCanvas01.drawColor(getResources().getColor(R.drawable.white));
    
    //��������
    Paint mPaint01 = new Paint();
    
    //���û�����ɫ��ģʽ
    mPaint01.setStyle(Paint.Style.FILL);
    mPaint01.setColor(colorFill);
    mPaint01.setStrokeWidth(1.0F);
    
    //��һ����boolean����
    for (int i=0;i<bRect.length;i++)
    {
      for (int j=0;j<bRect.length;j++)
      {
        if (bRect[j][i])
        {
          //��������뷽��
          mCanvas01.drawRect
          (
            new Rect
            (
              intPadding+j*3+2,
              intPadding+i*3+2,
              intPadding+j*3+2+3,
              intPadding+i*3+2+3
             ), mPaint01
          );
        }
      }
    }
   //��������ͼ
    mSurfaceHolder01.unlockCanvasAndPost(mCanvas01);
  }
  
  
  @Override
  public void surfaceChanged(SurfaceHolder surfaceholder, int format, int w, int h)
  {
    // TODO Auto-generated method stub
    Log.i(TAG, "Surface Changed");
  }
  
  @Override
  public void surfaceCreated(SurfaceHolder surfaceholder)
  {
    // TODO Auto-generated method stub
    Log.i(TAG, "Surface Changed");
  }
  
  @Override
  public void surfaceDestroyed(SurfaceHolder surfaceholder)
  {
    // TODO Auto-generated method stub
    Log.i(TAG, "Surface Destroyed");
  }
}
