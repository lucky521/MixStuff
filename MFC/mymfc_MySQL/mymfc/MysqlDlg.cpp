// MysqlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "mymfc.h"
#include "MysqlDlg.h"
#include "afxdialogex.h"


// MysqlDlg �Ի���

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

	
	//����Clistctrl������
	LONG lStyle;
    lStyle = GetWindowLong(m_list.m_hWnd, GWL_STYLE);//��ȡ��ǰ����style
    lStyle &= ~LVS_TYPEMASK; //�����ʾ��ʽλ
    lStyle |= LVS_REPORT; //����style
    SetWindowLong(m_list.m_hWnd, GWL_STYLE, lStyle);//����style

	DWORD dwStyle = m_list.GetExtendedStyle();
    dwStyle |= LVS_EX_FULLROWSELECT;//ѡ��ĳ��ʹ���и�����ֻ������report����listctrl��
    dwStyle |= LVS_EX_GRIDLINES;//�����ߣ�ֻ������report����listctrl��
    m_list.SetExtendedStyle(dwStyle); //������չ���

	m_list.InsertColumn(0,"id", LVCFMT_LEFT, 50);
	m_list.InsertColumn(1,"name", LVCFMT_LEFT, 80);
	m_list.InsertColumn(2,"words", LVCFMT_LEFT, 80);


}


BEGIN_MESSAGE_MAP(MysqlDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &MysqlDlg::OnClickedButtonConnect)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &MysqlDlg::OnLvnItemchangedList1)
END_MESSAGE_MAP()


// MysqlDlg ��Ϣ�������


void MysqlDlg::OnClickedButtonConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//���Ұ��¡����ӡ���ťʱ


	//ע����������mysql�ṩ��API�ӿں���
	MYSQL mysql;
	mysql_init(&mysql);
	//if(mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "utf-8"))
	//	AfxMessageBox("�����ַ���ʱ����");
	if(!mysql_real_connect(&mysql, "12.12.12.114", "root", "liulu","mymfc",3306, NULL,0))
	{
		AfxMessageBox("��ϲ�㣬���ݿ�����ʧ���ˣ�");
		return;
	}

	m_list.DeleteAllItems();
	char *ch_query;
	ch_query="select * from test";
	if(mysql_real_query(&mysql,ch_query,(UINT)strlen(ch_query))!=0){ 
		AfxMessageBox("���ݿ��ѯ������");
	}

	CString str;
	MYSQL_RES *result;
	MYSQL_ROW row;
	if(!(result = mysql_use_result(&mysql))){ 
		AfxMessageBox("��ȡ���ݲ�ѯ�����ʧ��"); 
	}
	int i=0;
	int index = 0;
	
	while(row = mysql_fetch_row(result)) {
		str.Format("%s",row[0]);
		index = m_list.InsertItem(i, str); //�����µ�һ��

		str.Format("%s",row[1]);
		m_list.SetItemText(i,1,str);	//���ø��еĲ�ͬ�е���ʾ�ַ�
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
