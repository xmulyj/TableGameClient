
// TractorGameDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "TractorGame.h"
#include "TractorGameDlg.h"

#include "common/KVDataProtocolFactory.h"
using namespace easynet;

#include "KeyDefine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTractorGameDlg 对话框




CTractorGameDlg::CTractorGameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTractorGameDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTractorGameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RoomList, m_RoomListCtrl);
	DDX_Control(pDX, IDC_TableList, m_TableListCtrl);
}

BEGIN_MESSAGE_MAP(CTractorGameDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOAD, &CTractorGameDlg::OnBnClickedLoad)
	ON_WM_TIMER()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_RoomList, &CTractorGameDlg::OnLvnItemchangedRoomlist)
	ON_NOTIFY(NM_DBLCLK, IDC_RoomList, &CTractorGameDlg::OnNMDblclkRoomlist)
END_MESSAGE_MAP()


// CTractorGameDlg 消息处理程序

BOOL CTractorGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_UID = -1;
	m_SelectRoomIndex = -1;

	//桌子列表
	m_TableListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_FLATSB |LVS_EX_GRIDLINES );
	m_TableListCtrl.InsertColumn(0, _T("Index"), LVCFMT_LEFT, 50);
	m_TableListCtrl.InsertColumn(2, _T("PlayerNum"), LVCFMT_LEFT, 100);
	m_TableListCtrl.ShowWindow(SW_HIDE);

	//房间列表
	m_RoomListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_FLATSB |LVS_EX_GRIDLINES );
	m_RoomListCtrl.InsertColumn(0, _T("Index"), LVCFMT_LEFT, 50);
	m_RoomListCtrl.InsertColumn(1, _T("RoomID"), LVCFMT_LEFT, 80);
	m_RoomListCtrl.InsertColumn(2, _T("PlayerNum"), LVCFMT_LEFT, 100);
	/*
	m_RoomListCtrl.InsertItem(0, _T("0"));
	m_RoomListCtrl.SetItemText(0, 1, _T("0"));
	m_RoomListCtrl.SetItemText(0, 2, _T("2"));

	m_RoomListCtrl.InsertItem(1, _T("1"));
	m_RoomListCtrl.SetItemText(1, 1, _T("1"));
	m_RoomListCtrl.SetItemText(1, 2, _T("3"));
	*/
	//WSADATA wsadata;
	//WSAStartup(MAKEWORD(2,0), &wsadata);

	m_GameSocket.Create();
	m_RoomSocket.Create();

	m_CurStatus = Status_PrintRoomList;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CTractorGameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CTractorGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CTractorGameDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTractorGameDlg::AppendMsg(LPCTSTR msg)
{
	CRichEditCtrl* msg_ctrl = (CRichEditCtrl*)GetDlgItem(IDC_MSG);

	msg_ctrl->SetSel(-1, -1);
	msg_ctrl->ReplaceSel(msg);
}

void CTractorGameDlg::PrintRoomList()
{
	if(m_CurStatus != Status_PrintRoomList)
	{
		AppendMsg(_T("Error:current status is [PrintRoomList]\r\n"));
		return ;
	}

	GetAllRoomReq();
}

bool CTractorGameDlg::GetAllRoomReq()
{
	if(!m_GameSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game_interface,wait please...\r\n"));

		if(TRUE==m_GameSocket.Connect(_T("192.168.80.130"), 3000))
		{
			m_CurStatus = Status_PrintRoomList;
			m_GameSocket.IsConnected = TRUE;

			AppendMsg(_T("connect game_interface successful.\r\n"));
		}
		else
		{
			return true;
		}
	}

	//发送GetAllRoom请求到interface

	KVDataProtocolFactory factory;
	ProtocolContext *context_tmp = new ProtocolContext;
	ProtocolContext &context = *context_tmp;
	unsigned int header_size, body_size;

	context.type = DTYPE_BIN;
	context.Info = "GetAllRoom";

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, GetAllRoom);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	header_size = factory.HeaderSize();
	body_size = kvdata.Size();
	context.CheckSize(header_size+body_size);
	kvdata.Serialize(context.Buffer+header_size);
	context.Size = header_size+body_size;
	factory.EncodeHeader(context.Buffer, body_size);

	m_GameSocket.m_SendContext = context_tmp;
	m_GameSocket.AsyncSelect(FD_WRITE);

	KillTimer(1);
	return true;
}
bool CTractorGameDlg::OnInterfaceRsp()
{
	//recv resp
	KVDataProtocolFactory factory;
	ProtocolContext context;

	context.header_size = factory.HeaderSize();
	context.CheckSize(context.header_size);

	int recv_size = m_GameSocket.Receive(context.Buffer, context.header_size);
	assert(recv_size == context.header_size);

	if(DECODE_SUCC != factory.DecodeHeader(context.Buffer, context.type, context.body_size))
	{
		return false;
	}

	context.CheckSize(context.body_size);
	recv_size = m_GameSocket.Receive(context.Buffer+context.header_size, context.body_size);
	assert(recv_size == context.body_size);
	context.Size = context.header_size+context.body_size;

	if(DECODE_SUCC != factory.DecodeBinBody(&context))
	{
		return false;
	}

	KVData *recv_kvdata = (KVData*)context.protocol;
	int Protocol;
	recv_kvdata->GetValue(KEY_Protocol, Protocol);
	if(Protocol == GetAllRoomRsp)
		OnGetAllRoomRsp(recv_kvdata);
	else if(Protocol == GetRoomAddr)
		OnGetRoomAddrRsp(recv_kvdata);
	else
		assert(0);
	factory.DeleteProtocol(-1, context.protocol);
	return true;
}


bool CTractorGameDlg::OnGetAllRoomRsp(KVData *kvdata)
{
	int RoomNum;
	char *NumArray;

	kvdata->GetValue(KEY_RoomNum, RoomNum);
	m_RoomList.clear();
	if(RoomNum > 0)
	{
		uint32_t size;
		kvdata->GetValue(KEY_NumArray, NumArray, size);
		assert(size == RoomNum*sizeof(int)*2);
		int *temp_buff = (int*)NumArray;
		for(int i=0; i<RoomNum; ++i)
		{
			RoomInfo room_info;
			room_info.RoomID = ntohl(temp_buff[0]);
			room_info.ClientNum = ntohl(temp_buff[1]);
			temp_buff += 2;
			m_RoomList.push_back(room_info);
		}
	}

	m_RoomListCtrl.DeleteAllItems();
	for(int i=0; i<m_RoomList.size(); ++i)
	{
		CString temp;
		temp.Format(_T("%d"), i+1);
		m_RoomListCtrl.InsertItem(i, temp);

		temp.Format(_T("%d"), m_RoomList[i].RoomID);
		m_RoomListCtrl.SetItemText(i, 1, temp);
		
		temp.Format(_T("%d"), m_RoomList[i].ClientNum);
		m_RoomListCtrl.SetItemText(i, 2, temp);
	}

	SetTimer(1, 2000, NULL);
	return true;
}

bool CTractorGameDlg::OnGetRoomAddrRsp(KVData *kvdata)
{

}

void CTractorGameDlg::PrintTableList()
{
	if(m_CurStatus != Status_PrintTableList)
	{
		AppendMsg(_T("Error:current status is [PrintTableList]\r\n"));
		return ;
	}

	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];
	if(!m_RoomSocket.IsConnected)
	{
		GetRoomAddrReq();
	}
}

bool CTractorGameDlg::GetRoomAddrReq()
{
	if(!m_GameSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game_interface,wait please...\r\n"));

		if(TRUE==m_GameSocket.Connect(_T("192.168.80.130"), 3000))
		{
			m_CurStatus = Status_PrintRoomList;
			m_GameSocket.IsConnected = TRUE;

			AppendMsg(_T("connect game_interface successful.\r\n"));
		}
		else
		{
			return true;
		}
	}

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, GetRoomAddr);
	kvdata.SetValue(KEY_RoomID, room_info.RoomID);
	kvdata.SetValue(KEY_ClientID, gUID);
	kvdata.SetValue(KEY_ClientName, gUName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	m_GameSocket->m_SendContext = context;
	return true;
}



bool CTractorGameDlg::GetRoomInfoReq()
{
	if(!m_GameSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game_interface,wait please...\r\n"));

		if(TRUE==m_GameSocket.Connect(_T("192.168.80.130"), 3000))
		{
			m_GameSocket.IsConnected = TRUE;
			AppendMsg(_T("connect game_interface successful.\r\n"));
		}
		else
		{
			return true;
		}
	}

	
	return true;
}

bool CTractorGameDlg::OnGetRoomInfoRsp()
{
	return true;
}

void CTractorGameDlg::OnBnClickedLoad()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *UIDCtrl = (CEdit*)GetDlgItem(IDC_UID);
	CString uid;
	UIDCtrl->GetWindowText(uid);

	CEdit *UNameCtrl = (CEdit*)GetDlgItem(IDC_UNAME);
	CString uname;
	CStringA unameA;
	UNameCtrl->GetWindowText(uname);

	if(uid.IsEmpty() || uname.IsEmpty())
	{
		AppendMsg(_T("uid or uname is empty.\r\n"));
		return;
	}

	m_UID = _ttoi(uid);
	unameA = uname.GetBuffer();
	m_UName = unameA.GetBuffer();

	CString msg = _T("user sign in:uid=")+uid+_T(",uname=")+uname+_T("\r\n");
	AppendMsg(msg);

	PrintRoomList();
}

void CTractorGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent == 1)  //请求RoomList
	{
		AppendMsg(_T("PrintRoomList\r\n"));
		PrintRoomList();
	}
	CDialog::OnTimer(nIDEvent);
}

void CTractorGameDlg::OnLvnItemchangedRoomlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}

void CTractorGameDlg::OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index = pNMItemActivate->iItem;
	if(index >=0 && index<m_RoomListCtrl.GetItemCount())
	{
		CString temp;
		temp.Format(_T("Into Room[%d]\r\n"), index);
		AppendMsg(temp);

		m_SelectRoomIndex = index;
		m_RoomListCtrl.ShowWindow(SW_HIDE);

		temp.Format(_T("->房间[%02d]"), index+1);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		m_TableListCtrl.ShowWindow(SW_NORMAL);
		KillTimer(1);

		m_CurStatus = Status_PrintTableList;
	}

	*pResult = 0;
}
