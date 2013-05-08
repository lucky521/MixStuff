package qrcode.com.cn;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.json.JSONArray;
import org.json.JSONObject;

import qrcode.com.cn.HttpConnectionUtil.HttpConnectionCallback;
import qrcode.com.cn.HttpConnectionUtil.HttpMethod;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.Toast;

//数据库连接测试


public class JsonFromPHPDemo extends Activity
{
	private ListView listView;
	private SimpleAdapter adapter;
	protected List<Person> persons;
	private Person person;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		
		setContentView(R.layout.xml_handler);//显示布局

		listView = (ListView) findViewById(R.id.xml_list);
		
		
	    Button back = (Button)findViewById(R.id.back_2);
	    back.setOnClickListener(new Button.OnClickListener()
	    {
	       @Override
	       public void onClick(View arg0)
	       {
	         // TODO Auto-generated method stub
	         Intent intent=new Intent();
	         intent.setClass(JsonFromPHPDemo.this, CQRcode.class);
	         startActivity(intent);
	         JsonFromPHPDemo.this.finish();
	    	
	       }
	    });
		
		
		try
		{
			// 自完义适配方法
			getAdapter();

		} catch (Exception e)
		{
			e.printStackTrace();
		}

	}
	
	/**
	 * 自定义方法,从网络取得数据,并将数据放入ListView中
	 */
	private void getAdapter()
	{
		//地址
		String url = getResources().getString(R.string.server_host) + "index.php/Index/show";
		
		//网络连接方法
		HttpConnectionUtil connUtil = new HttpConnectionUtil();
		
		try{//使用post方法
		connUtil.asyncConnect(url, HttpMethod.POST,
				new HttpConnectionCallback()
				{
					@Override
					public void execute(String response)
					{
						persons = JsonToList(response);//response是从服务器端得到的相应数据，是json字符串。
						setInAdapter();
						listView.setAdapter(adapter);
					}
				});
		}catch(Exception e)
		{	
			//如果连接失败
			Toast.makeText(this,"连接失败，请检查网络和服务器状态。", Toast.LENGTH_LONG).show();
	         
			Intent intent=new Intent();
	        intent.setClass(JsonFromPHPDemo.this, CQRcode.class);
	        startActivity(intent);
	        JsonFromPHPDemo.this.finish();
		}

	}
	
	/**
	 * 将数据配入ListView中
	 */
	protected void setInAdapter()
	{
		List<Map<String, String>> lists = new ArrayList<Map<String, String>>();
		// 将persons中的数据转换到ArrayList<Map<String,String>>中
		// String>>中,因为SimpleAdapter要用这个类型的数据进行适配
		Map<String, String> map;
		for (Person p : persons)
		{
			map = new HashMap<String, String>();

			map.put("id", p.getId());
			map.put("status", p.getStatus());
			map.put("name", p.getName());
			map.put("tool", p.getTool());
			map.put("number", p.getNumber());

			lists.add(map);
		}

		// HashMap<String, String>中的key
		String[] from = { "id", "status", "name", "tool", "number" };
		// list_item.xml中对应的控件ID
		int[] to = { R.id.item_id, R.id.item_status, R.id.item_name,
				R.id.item_tool, R.id.item_number };

		adapter = new SimpleAdapter(this, lists, R.layout.handler_list_item, from, to);//填充listview布局

	}

	/**
	 * 格式转换：将Json字符串转换成List<Person>数据
	 * 
	 * @param response
	 *            输入的Json字符串
	 * @return
	 */
	protected List<Person> JsonToList(String response)
	{
		List<Person> list = new ArrayList<Person>();

		try
		{
			// 将字符串转换为Json数组
			JSONArray array = new JSONArray(response);
			// 数组长度
			int length = array.length();
			for (int i = 0; i < length; i++)
			{
				// 将每一个数组再转换成Json对象
				JSONObject obj = array.getJSONObject(i);

				person = new Person();
				person.setId(obj.getString("id"));
				person.setStatus(obj.getString("status"));
				person.setName(obj.getString("name"));
				person.setTool(obj.getString("tool"));
				person.setNumber(obj.getString("number"));

				list.add(person);

			}
			return list;
		} catch (Exception e)
		{
			e.printStackTrace();
		}
		return null;
	}

}
