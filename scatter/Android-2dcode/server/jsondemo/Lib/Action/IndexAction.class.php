<?php

class IndexAction extends Action
{
	//查询数据库中的所有数据
	public function show(){

		$demo = M("data");

		$data = $demo->select();
		
		echo $this->myjson($data);


	}

	//向数据库中插入新数据
	public function insert(){

		$status = $_GET['status'];
		$name = $_GET['name'];
		$tool = $_GET['tool'];
		$number = $_GET['number'];

		$model = new Model();		
		$data = $model->execute("insert into demo_data (`id`,`status`,`name`,`tool`, `number`) values(null, '$status', '$name', '$tool', '$number')  ");

	}

	//查询数据库中的某一条数据
	public function check()
	{
	//有待扩展。。。
	
	}

	



	/**
	 * 将UTF-8编码转换为中文
	 * Enter description here ...
	 * @param unknown_type $code
	 * @return string
	 */
	function myjson($code)
	{
		$code = json_encode($this->urlencodeAry($code));
		return urldecode($code);
	}
	
	function urlencodeAry($data)
	{
		if(is_array($data))
		{
			foreach($data as $key=>$val)
			{
				$data[$key] =$this->urlencodeAry($val);
			}
			return $data;
		}
		else
		{
			return urlencode($data);
		}
	}
	
	/**
	 +----------------------------------------------------------
	 * 默认操作
	 +----------------------------------------------------------
	 */
	public function index()
	{
		$this->display(THINK_PATH.'/Tpl/Autoindex/hello.html');
	}

	/**
	 +----------------------------------------------------------
	 * 探针模式
	 +----------------------------------------------------------
	 */
	public function checkEnv()
	{
		load('pointer',THINK_PATH.'/Tpl/Autoindex');//载入探针函数
		$env_table = check_env();//根据当前函数获取当前环境
		echo $env_table;
	}

}
?>
