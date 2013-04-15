// luckyThirdDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lucky003.h"
#include "luckyThirdDlg.h"
#include "afxdialogex.h"


// luckyThirdDlg �Ի���

IMPLEMENT_DYNAMIC(luckyThirdDlg, CDialog)

luckyThirdDlg::luckyThirdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(luckyThirdDlg::IDD, pParent)
{

}

luckyThirdDlg::~luckyThirdDlg()
{
}

void luckyThirdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(luckyThirdDlg, CDialog)
END_MESSAGE_MAP()


// luckyThirdDlg ��Ϣ�������

char * GetIpList()  
{  
    WORD wVersionRequested;  
    WSADATA wsaData;  
    int err;  
    wVersionRequested = MAKEWORD( 2, 2 );  
    err = WSAStartup( wVersionRequested, &wsaData );  
    if ( err != 0 )  
    {
        //MessageBox(_T("WSAStartup failed !"));
        return false;  
    }
    char szhn[256];  
    int nStatus = gethostname(szhn, sizeof(szhn));  
    if (nStatus == SOCKET_ERROR )  
    {  
        //MessageBox("gethostname failed!");
        return false;  
    }  
  
    HOSTENT *host = gethostbyname(szhn);  
    char * ipaddress =NULL;  
    if (host != NULL)  
    {  
        ipaddress = inet_ntoa( *(IN_ADDR*)host->h_addr_list[0]);  
    }
    WSACleanup();  
    return ipaddress;  
}  



BOOL luckyThirdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CIPAddressCtrl* m_ipAddr = (CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1);//��ȡIPCTRL�ؼ���ָ��
     
	CString   strIP;
	DWORD   dwIP;   
	  
	strIP = CString(GetIpList());
	dwIP   =   inet_addr(strIP);
	unsigned char *pIP = (unsigned char*)&dwIP;
	m_ipAddr->SetAddress(*pIP,   *(pIP+1),   *(pIP+2),   *(pIP+3));   


	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


