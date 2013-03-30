package tscolari.mobile_sample;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLConnection;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebSettings.LayoutAlgorithm;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.Toast;


public class AndroidMobileAppSampleActivity extends Activity {

	public static WebView mainWebView;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        requestWindowFeature(Window.FEATURE_PROGRESS);
        
        setContentView(R.layout.main);
        mainWebView = (WebView) findViewById(R.id.mainWebView);
        
        WebSettings webSettings = mainWebView.getSettings();
        webSettings.setJavaScriptEnabled(true);
        
        
        webSettings.setSupportZoom(true); //缩放
        webSettings.setDefaultZoom(WebSettings.ZoomDensity.FAR);  
        //webSettings.setBuiltInZoomControls(true);
        

        
        mainWebView.setWebViewClient(new MyCustomWebViewClient()); 
        
        mainWebView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
        
        //mainWebView.setOnTouchListener((OnTouchListener) this);//监听手势
        
        
        mainWebView.setWebChromeClient(new WebChromeClient() {  
            public void onProgressChanged(WebView view, int progress) {  
            	AndroidMobileAppSampleActivity.this.setProgress(progress * 100);  
            }  
        }); 
        
        //mainWebView.loadUrl("http://www.baidu.com");
        String url = "http://www.lucky521.com";
        getFormatData(mainWebView, url);
    }     


    private void getFormatData(WebView view, String Url) {
        String text = "";
        try {
                URL url = new URL(Url);
                URLConnection conn = url.openConnection();
                BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
                String line = "";
                while ((line = rd.readLine()) != null) {
                		
	            		String regEx = ".*phoneNumber.*";
	            		Pattern pat = Pattern.compile(regEx);
	            		Matcher mat = pat.matcher(line);
	            		boolean result = mat.find();
	            		if (result == true )
	            			line = line.replace("phoneNumber", "phoneNumber<br>");
                	
                		regEx = ".*tag-link.*";
                		result = Pattern.matches(regEx, line);
                		if (result == true )  
                			continue;
                		
                		
                		text = text + line ;
		
                } 
        } catch (IOException e) {
                e.printStackTrace();
        }
        view.loadDataWithBaseURL(Url, text, "text/html", "UTF-8", null);

}
    
    
    
    @Override  
    public boolean onCreateOptionsMenu(Menu menu){  
            menu.add(Menu.NONE, Menu.NONE, 1, "后退");  
            menu.add(Menu.NONE, Menu.NONE, 2, "刷新");
            menu.add(Menu.NONE, Menu.NONE, 3, "关于"); 
            menu.add(Menu.NONE, Menu.NONE, 4, "退出");  
            return true;  
        }  
    
    @Override  
    public boolean onOptionsItemSelected(MenuItem item){  
    	//Toast.makeText(this, item.getOrder()+","+item.getTitle(), Toast.LENGTH_LONG).show();
    	if (item.getOrder() == 3)
    		Toast.makeText(this,"版本号：1.0\n作者：Lucky\n声明：不断升级！\n祝每天心情快乐，学业有成！", Toast.LENGTH_LONG).show();
    	else if (item.getOrder() == 2)
    		mainWebView.reload();
    	else if (item.getOrder() == 1)
    		mainWebView.goBack();
    	else if (item.getOrder() == 4)
    		android.os.Process.killProcess(android.os.Process.myPid());
      return true;  
    }  
    

    
    private class MyCustomWebViewClient extends WebViewClient {
        @Override
        public boolean shouldOverrideUrlLoading(WebView view, String url) {
        	getFormatData(mainWebView, url);
            //view.loadUrl(url);
            return true;
        }
    }
}