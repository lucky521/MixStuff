// MysqlDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "mymfc.h"
#include "MysqlDlg.h"
#include "afxdialogex.h"


// MysqlDlg 对话框

IMPLEMENT_DYNAMIC(MysqlDlg, CDialog)

MysqlDlg::MysqlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(MysqlDlg::IDD, pParent)
{

}

MysqlDlg::~MysqlDlg()
{
}

void MysqlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list);

	
	//设置Clistctrl的属性
	LONG lStyle;
    lStyle = GetWindowLong(m_list.m_hWnd, GWL_STYLE);//获取当前窗口style
    lStyle &= ~LVS_TYPEMASK; //清除显示方式位
    lStyle |= LVS_REPORT; //设置style
    SetWindowLong(m_list.m_hWnd, GWL_STYLE, lStyle);//设置style

	DWORD dwStyle = m_list.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;//选中某行使整行高亮（只适用与report风格的listctrl）
    dwStyle |= LVS_EX_GRIDLINES;//网格线（只适用与report风格的listctrl）
    m_list.SetExtendedStyle(dwStyle); //设置扩展风格

	m_list.InsertColumn(0,"id", LVCFMT_LEFT, 50);
	m_list.InsertColumn(1,"name", LVCFMT_LEFT, 80);
	m_list.InsertColumn(2,"words", LVCFMT_LEFT, 80);


}


BEGIN_MESSAGE_MAP(MysqlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &MysqlDlg::OnClickedButtonConnect)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &MysqlDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// MysqlDlg 消息处理程序


void MysqlDlg::OnClickedButtonConnect()
{
	// TODO: 在此添加控件通知处理程序代码
	//当我按下“连接”按钮时


	//注意下面中由mysql提供的API接口函数
	MYSQL mysql;
	mysql_init(&mysql);
	//if(mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf-8"))
	//	AfxMessageBox("设置字符集时出错！");
	if(!mysql_real_connect(&mysql, "12.12.12.114", "root", "liulu","mymfc",3306, NULL,0))
	{
		AfxMessageBox("恭喜你，数据库连接失败了！");
		return;
	}

	m_list.DeleteAllItems();
	char *ch_query;
	ch_query="select * from test";
	if(mysql_real_query(&mysql,ch_query,(UINT)strlen(ch_query))!=0){ 
		AfxMessageBox("数据库查询语句出错");
	}

	CString str;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if(!(result = mysql_use_result(&mysql))){ 
		AfxMessageBox("读取数据查询结果集失败"); 
	}
	int i=0;
	int index = 0;
	
	while(row = mysql_fetch_row(result)) {
		str.Format("%s",row[0]);
		index = m_list.InsertItem(i, str); //插入新的一行

		str.Format("%s",row[1]);
		m_list.SetItemText(i,1,str);	//设置该行的不同列的显示字符
		str.Format("%s",row[2]);
		m_list.SetItemText(i,2,str);

		i++;
	}
	
	mysql_free_result(result);
	mysql_close(&mysql);
}


void MysqlDlg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}
