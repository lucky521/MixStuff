package qrcode.com.cn;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import org.apache.http.HttpResponse;
import org.apache.http.HttpStatus;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.client.methods.HttpUriRequest;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;

import android.os.Handler;
import android.util.Log;

public class HttpConnectionUtil
{
	public static enum HttpMethod
	{
		GET, POST
	}

	/**
	 * �첽����
	 * 
	 * @param url
	 *            ��ַ
	 * @param method
	 *            Http����,POST��GET
	 * @param callback
	 *            �ص�����,���ظ�ҳ�������������
	 */
	public void asyncConnect(final String url, final HttpMethod method,
			final HttpConnectionCallback callback)
	{
		asyncConnect(url, null, method, callback);
	}

	/**
	 * ͬ������
	 * 
	 * @param url
	 *            ��ַ
	 * @param method
	 *            Http����,POST��GET
	 * @param callback
	 *            �ص�����,���ظ�ҳ�������������
	 */
	public void syncConnect(final String url, final HttpMethod method,
			final HttpConnectionCallback callback)
	{
		syncConnect(url, null, method, callback);
	}

	/**
	 * �첽����������
	 * 
	 * @param url
	 *            ��ַ
	 * @param params
	 *            POST��GETҪ���ݵĲ���
	 * @param method
	 *            ����,POST��GET
	 * @param callback
	 *            �ص�����
	 */
	public void asyncConnect(final String url,
			final Map<String, String> params, final HttpMethod method,
			final HttpConnectionCallback callback)
	{
		Handler handler = new Handler();
		Runnable runnable = new Runnable()
		{
			public void run()
			{
				syncConnect(url, params, method, callback);
			}
		};
		handler.post(runnable);
	}

	/**
	 * ͬ������������
	 * 
	 * @param url
	 *            ��ַ
	 * @param params
	 *            POST��GETҪ���ݵĲ���
	 * @param method
	 *            ����,POST��GET
	 * @param callback
	 *            �ص�����
	 */
	public void syncConnect(final String url, final Map<String, String> params,
			final HttpMethod method, final HttpConnectionCallback callback)
	{
		String json = null;
		BufferedReader reader = null;

		try
		{
			HttpClient client = new DefaultHttpClient();
			HttpUriRequest request = getRequest(url, params, method);
			HttpResponse response = client.execute(request);
			if (response.getStatusLine().getStatusCode() == HttpStatus.SC_OK)
			{
				reader = new BufferedReader(new InputStreamReader(response
						.getEntity().getContent()));
				StringBuilder sb = new StringBuilder();
				for (String s = reader.readLine(); s != null; s = reader
						.readLine())
				{
					sb.append(s);
				}

				json = sb.toString();
			}
		} catch (ClientProtocolException e)
		{
			Log.e("HttpConnectionUtil", e.getMessage(), e);
		} catch (IOException e)
		{
			Log.e("HttpConnectionUtil", e.getMessage(), e);
		} finally
		{
			try
			{
				if (reader != null)
				{
					reader.close();
				}
			} catch (IOException e)
			{
				// ignore me
			}
		}
		callback.execute(json);
	}

	/**
	 * POST��GET���ݲ�����ͬ,POST����ʽ����,GET����ʽ����
	 * 
	 * @param url
	 *            ��ַ
	 * @param params
	 *            ����
	 * @param method
	 *            ����
	 * @return
	 */
	private HttpUriRequest getRequest(String url, Map<String, String> params,
			HttpMethod method)
	{
		if (method.equals(HttpMethod.POST))
		{
			List<NameValuePair> listParams = new ArrayList<NameValuePair>();
			if (params != null)
			{
				for (String name : params.keySet())
				{
					listParams.add(new BasicNameValuePair(name, params
							.get(name)));
				}
			}
			try
			{
				UrlEncodedFormEntity entity = new UrlEncodedFormEntity(
						listParams);
				HttpPost request = new HttpPost(url);
				request.setEntity(entity);
				return request;
			} catch (UnsupportedEncodingException e)
			{
				// Should not come here, ignore me.
				throw new java.lang.RuntimeException(e.getMessage(), e);
			}
		} else
		{
			if (url.indexOf("?") < 0)
			{
				url += "?";
			}
			if (params != null)
			{
				for (String name : params.keySet())
				{
					try
					{
						url += "&" + name + "="
								+ URLEncoder.encode(params.get(name), "UTF-8");

					} catch (UnsupportedEncodingException e)
					{
						e.printStackTrace();
					}
				}
			}
			HttpGet request = new HttpGet(url);
			return request;
		}
	}
	
	/**
	 * �ص��ӿ�
	 * @author Administrator
	 *
	 */
	public interface HttpConnectionCallback
	{
		/**
		 * Call back method will be execute after the http request return.
		 * 
		 * @param response
		 *            the response of http request. The value will be null if
		 *            any error occur.
		 */
		void execute(String response);
	}

}
