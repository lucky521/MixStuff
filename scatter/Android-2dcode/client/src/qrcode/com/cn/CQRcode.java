package qrcode.com.cn;

//整个程序的主界面。



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
    
    //展示主界面
    public void jumptolayoutmain()
    {
    	 setContentView(R.layout.main);//展示主界面
         mRadioGroup1=(RadioGroup) findViewById(R.id.select);
         
         mRadioname=(RadioButton) findViewById(R.id.select1);//二维码扫描
         mRadiotext=(RadioButton) findViewById(R.id.select2);//二维码生成
         mRadiosms=(RadioButton) findViewById(R.id.select3); //数据库连接测试
         mRadiourl=(RadioButton) findViewById(R.id.select4); 
         
         mRadioGroup1.setOnCheckedChangeListener(mChangeRadio);
    }
    
    //监听列表的按下事件
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
    
	//菜单项设计
    public boolean onCreateOptionsMenu(Menu menu)
    {
      menu.add(0, 0, 0, R.string.app_about);
      menu.add(0, 1, 1, R.string.str_exit);
      return super.onCreateOptionsMenu(menu);
    }
    
    //菜单项的响应设计
    public boolean onOptionsItemSelected(MenuItem item)
    {
      super.onOptionsItemSelected(item);
      switch(item.getItemId())
      {
        case 0:
          openOptionsDialog(); //打开关于对话框
          break;
        case 1:
          finish();//退出程序
          break;
      }
      return true;
    }
    
    //弹出关于对话框
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
    
    
    //跳到输入信息界面
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
    			//获取信息
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
    

    
    //跳到二维码扫描的界面
    public void jumptolayoutscanner()
    {
    		Intent intent=new Intent();
    		intent.setClass(CQRcode.this, testCamera.class);
    		Bundle bundle=new Bundle();
	
    		intent.putExtras(bundle);
    			
    		startActivity(intent); //开始扫描模块的activity
    		CQRcode.this.finish();	//关闭当前的activity
    		
    }
    
	//跳到数据库连接测试的界面
    public void jumptolayoutconnecttest()
    {
		Intent intent=new Intent();
		intent.setClass(CQRcode.this, JsonFromPHPDemo.class);
		Bundle bundle=new Bundle();

		intent.putExtras(bundle);
			
		startActivity(intent); //开始扫描模块的activity
		CQRcode.this.finish();	//关闭当前的activity
    	
    }
    
    
    private void beingdevelop()
    {
      new AlertDialog.Builder(this)
      .setMessage("功能正在扩展中...")
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