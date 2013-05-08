package qrcode.com.cn;

//��������������档



import android.app.Activity;
import android.app.AlertDialog;
import android.os.Bundle;
import android.view.View;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuItem;
import android.content.DialogInterface;

public class CQRcode extends Activity {
	
	public RadioGroup mRadioGroup1;
	public RadioButton mRadioname,mRadiosms,mRadioemail,mRadiotext,mRadiourl;
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        jumptolayoutmain();
       
    }
    
    //չʾ������
    public void jumptolayoutmain()
    {
    	 setContentView(R.layout.main);//չʾ������
         mRadioGroup1=(RadioGroup) findViewById(R.id.select);
         
         mRadioname=(RadioButton) findViewById(R.id.select1);//��ά��ɨ��
         mRadiotext=(RadioButton) findViewById(R.id.select2);//��ά������
         mRadiosms=(RadioButton) findViewById(R.id.select3); //���ݿ����Ӳ���
         mRadiourl=(RadioButton) findViewById(R.id.select4); 
         
         mRadioGroup1.setOnCheckedChangeListener(mChangeRadio);
    }
    
    //�����б�İ����¼�
    private RadioGroup.OnCheckedChangeListener mChangeRadio=new RadioGroup.OnCheckedChangeListener()
    {
    	@Override
    	public void onCheckedChanged(RadioGroup group,int checkedId)
    	{
    		if (checkedId == mRadiotext.getId())
    			jumptolayoutinput();
    		else if(checkedId == mRadioname.getId())
    			jumptolayoutscanner();
    		else if(checkedId==mRadiosms.getId())
    			jumptolayoutconnecttest();
    		else if(checkedId==mRadiourl.getId())
    			beingdevelop();
    		else
    		{}
    	}
    };
    
	//�˵������
    public boolean onCreateOptionsMenu(Menu menu)
    {
      menu.add(0, 0, 0, R.string.app_about);
      menu.add(0, 1, 1, R.string.str_exit);
      return super.onCreateOptionsMenu(menu);
    }
    
    //�˵������Ӧ���
    public boolean onOptionsItemSelected(MenuItem item)
    {
      super.onOptionsItemSelected(item);
      switch(item.getItemId())
      {
        case 0:
          openOptionsDialog(); //�򿪹��ڶԻ���
          break;
        case 1:
          finish();//�˳�����
          break;
      }
      return true;
    }
    
    //�������ڶԻ���
    private void openOptionsDialog()
    {
      new AlertDialog.Builder(this)
      .setTitle(R.string.app_about)
      .setMessage(R.string.app_about_msg)
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
    
    
    //����������Ϣ����
    public void jumptolayoutinput()
    {
    	setContentView(R.layout.businesscard);
    	Button back1=(Button)findViewById(R.id.back);
    	Button draw=(Button)findViewById(R.id.draw);
    	back1.setOnClickListener(new Button.OnClickListener()
    	{
			@Override
			public void onClick(View arg0) {
				// TODO Auto-generated method stub
				jumptolayoutmain();
			}
    	});
    	
    	draw.setOnClickListener(new Button.OnClickListener()
    	{
    		@Override
    		public void onClick(View arg0)
    		{
    			//��ȡ��Ϣ
    		    EditText name1=(EditText)findViewById(R.id.name);
    			EditText occupation1=(EditText)findViewById(R.id.occupation);
    			EditText fixedphone1=(EditText)findViewById(R.id.fixedphone);
    			EditText mobilephone1=(EditText)findViewById(R.id.mobilephone);
    			String name2=name1.getText().toString();
    			String occupation2=occupation1.getText().toString();
    			String fixedphone2=fixedphone1.getText().toString();
    			String mobilephone2=mobilephone1.getText().toString();
    			
    			Intent intent=new Intent();
    			
    			intent.setClass(CQRcode.this, DrawQRCode.class);
    			Bundle bundle=new Bundle();
    			bundle.putString("name",name2);
    			bundle.putString("occupation",occupation2);
    			bundle.putString("fixedphone",fixedphone2);
    			bundle.putString("mobilephone",mobilephone2);
    			intent.putExtras(bundle);
    			
    			startActivity(intent);
    			CQRcode.this.finish();
    			
    		}
    	});
    }
    

    
    //������ά��ɨ��Ľ���
    public void jumptolayoutscanner()
    {
    		Intent intent=new Intent();
    		intent.setClass(CQRcode.this, testCamera.class);
    		Bundle bundle=new Bundle();
	
    		intent.putExtras(bundle);
    			
    		startActivity(intent); //��ʼɨ��ģ���activity
    		CQRcode.this.finish();	//�رյ�ǰ��activity
    		
    }
    
	//�������ݿ����Ӳ��ԵĽ���
    public void jumptolayoutconnecttest()
    {
		Intent intent=new Intent();
		intent.setClass(CQRcode.this, JsonFromPHPDemo.class);
		Bundle bundle=new Bundle();

		intent.putExtras(bundle);
			
		startActivity(intent); //��ʼɨ��ģ���activity
		CQRcode.this.finish();	//�رյ�ǰ��activity
    	
    }
    
    
    private void beingdevelop()
    {
      new AlertDialog.Builder(this)
      .setMessage("����������չ��...")
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
    
    
}