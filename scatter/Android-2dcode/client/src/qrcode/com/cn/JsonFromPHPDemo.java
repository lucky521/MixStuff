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

//���ݿ����Ӳ���


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
		
		setContentView(R.layout.xml_handler);//��ʾ����

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
			// ���������䷽��
			getAdapter();

		} catch (Exception e)
		{
			e.printStackTrace();
		}

	}
	
	/**
	 * �Զ��巽��,������ȡ������,�������ݷ���ListView��
	 */
	private void getAdapter()
	{
		//��ַ
		String url = getResources().getString(R.string.server_host) + "index.php/Index/show";
		
		//�������ӷ���
		HttpConnectionUtil connUtil = new HttpConnectionUtil();
		
		try{//ʹ��post����
		connUtil.asyncConnect(url, HttpMethod.POST,
				new HttpConnectionCallback()
				{
					@Override
					public void execute(String response)
					{
						persons = JsonToList(response);//response�Ǵӷ������˵õ�����Ӧ���ݣ���json�ַ�����
						setInAdapter();
						listView.setAdapter(adapter);
					}
				});
		}catch(Exception e)
		{	
			//�������ʧ��
			Toast.makeText(this,"����ʧ�ܣ���������ͷ�����״̬��", Toast.LENGTH_LONG).show();
	         
			Intent intent=new Intent();
	        intent.setClass(JsonFromPHPDemo.this, CQRcode.class);
	        startActivity(intent);
	        JsonFromPHPDemo.this.finish();
		}

	}
	
	/**
	 * ����������ListView��
	 */
	protected void setInAdapter()
	{
		List<Map<String, String>> lists = new ArrayList<Map<String, String>>();
		// ��persons�е�����ת����ArrayList<Map<String,String>>��
		// String>>��,��ΪSimpleAdapterҪ��������͵����ݽ�������
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

		// HashMap<String, String>�е�key
		String[] from = { "id", "status", "name", "tool", "number" };
		// list_item.xml�ж�Ӧ�Ŀؼ�ID
		int[] to = { R.id.item_id, R.id.item_status, R.id.item_name,
				R.id.item_tool, R.id.item_number };

		adapter = new SimpleAdapter(this, lists, R.layout.handler_list_item, from, to);//���listview����

	}

	/**
	 * ��ʽת������Json�ַ���ת����List<Person>����
	 * 
	 * @param response
	 *            �����Json�ַ���
	 * @return
	 */
	protected List<Person> JsonToList(String response)
	{
		List<Person> list = new ArrayList<Person>();

		try
		{
			// ���ַ���ת��ΪJson����
			JSONArray array = new JSONArray(response);
			// ���鳤��
			int length = array.length();
			for (int i = 0; i < length; i++)
			{
				// ��ÿһ��������ת����Json����
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
