package qrcode.com.cn;

public class Person
{
	private String id;
	private String status;
	private String name;
	private String tool;
	private String number;
	

	public String getId()
	{
		return id;
	}
	public void setId(String id)
	{
		this.id = id;
	}
	public String getStatus()
	{
		return status;
	}
	public void setStatus(String status)
	{
		this.status = status;
	}
	public String getName()
	{
		return name;
	}
	public void setName(String name)
	{
		this.name = name;
	}
	public String getTool()
	{
		return tool;
	}
	public void setTool(String tool)
	{
		this.tool = tool;
	}
	public String getNumber()
	{
		return number;
	}
	public void setNumber(String number)
	{
		this.number = number;
	}
	@Override
	public String toString()
	{
		return "Person [id=" + id + ", status=" + status + ", name=" + name
				+ ", tool=" + tool + ", number=" + number + "]";
	}
	
}
