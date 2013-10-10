
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

#define INTERFACE_IP  _T("192.168.80.130")

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
	ON_BN_CLICKED(IDC_STATIC_LIST, &CTractorGameDlg::OnStaticListClick)
	ON_BN_CLICKED(IDC_STATIC_ROOM, &CTractorGameDlg::OnStaticRoomClick)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_RoomList, &CTractorGameDlg::OnNMDblclkRoomlist)
	ON_NOTIFY(NM_DBLCLK, IDC_TableList, &CTractorGameDlg::OnNMDblclkTablelist)
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
	m_SelectTableIndex = -1;


	HICON  hicon;
	hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ImageList.Create(64,64, ILC_COLORDDB, 1,1);
	m_ImageList.Add(hicon);

	//房间列表
	m_RoomListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB |LVS_EX_SUBITEMIMAGES );
	m_RoomListCtrl.SetImageList(&m_ImageList,  LVSIL_NORMAL);

	//桌子列表
	m_TableListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT |LVS_EX_FLATSB |LVS_EX_SUBITEMIMAGES);
	m_TableListCtrl.SetImageList(&m_ImageList,  LVSIL_NORMAL);
	m_TableListCtrl.ShowWindow(SW_HIDE);

	//WSADATA wsadata;
	//WSAStartup(MAKEWORD(2,0), &wsadata);

	m_CurStatus = Status_PrintRoomList;

	m_RoomListCtrl.GetWindowRect(&m_TableRect);
	ScreenToClient(&m_TableRect);

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
		if(m_CurStatus == Status_PrintTableList)
		{
			OnPaint_TableList(m_TableRect);
		}		


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

//打印房间列表
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
	if(!m_InterfaceSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game interface,wait please...\r\n"));

		m_InterfaceSocket.Create();
		if(TRUE==m_InterfaceSocket.Connect(INTERFACE_IP, 3000))
		{
			m_CurStatus = Status_PrintRoomList;
			m_InterfaceSocket.IsConnected = TRUE;

			AppendMsg(_T("connect game_interface successful.\r\n"));
		}
		else
		{
			CString temp;
			uint32_t error_code = WSAGetLastError();

			if(error_code == WSAEWOULDBLOCK)
			{
				temp.Format(_T("connect game interface WouldBlock\r\n"));
				AppendMsg(temp);
				m_InterfaceSocket.AsyncSelect(FD_READ|FD_CONNECT);
			}
			else
			{
				temp.Format(_T("connect game interface failed. error_code=%u\r\n"), error_code);
				AppendMsg(temp);
				m_InterfaceSocket.Close();
			}
			return true;
		}
	}
	KillTimer(1);

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

	m_InterfaceSocket.m_SendContext = context_tmp;
	m_InterfaceSocket.AsyncSelect(FD_WRITE|FD_CLOSE);;
	return true;
}

bool CTractorGameDlg::OnGetAllRoomRsp(KVData *kvdata)
{
	int RoomNum;
	char *NumArray;

	AppendMsg(_T("OnGetAllRoomRsp\r\n"));
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

	if(m_CurStatus != Status_PrintRoomList)
		return true;

	m_RoomListCtrl.SetRedraw(FALSE);
	m_RoomListCtrl.DeleteAllItems();
	for(int i=0; i<m_RoomList.size(); ++i)
	{
		CString temp;
		temp.Format(_T("房间[%2d] %d人"), i+1, m_RoomList[i].ClientNum);
		m_RoomListCtrl.InsertItem(i, temp, 0);
	}
	m_RoomListCtrl.SetRedraw(TRUE);

	SetTimer(1, 5000, NULL);
	return true;
}

bool CTractorGameDlg::GetRoomAddrReq()
{
	AppendMsg(_T("GetRoomAddrReq\r\n"));
	if(!m_InterfaceSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game interface,wait please...\r\n"));

		m_InterfaceSocket.Create();
		if(TRUE==m_InterfaceSocket.Connect(INTERFACE_IP, 3000))
		{
			m_CurStatus = Status_PrintRoomList;
			m_InterfaceSocket.IsConnected = TRUE;

			AppendMsg(_T("connect game_interface successful.\r\n"));
		}
		else
		{
			uint32_t error_code = WSAGetLastError();
			CString temp;
			if(error_code == WSAEWOULDBLOCK)
			{
				temp.Format(_T("connect interface WouldBlock\r\n"));
				AppendMsg(temp);
				m_InterfaceSocket.AsyncSelect(FD_READ|FD_CONNECT);
			}
			else
			{
				temp.Format(_T("connect interface failed. error_code=%u\r\n"), error_code);
				AppendMsg(temp);
			}
			return true;
		}
	}

	AppendMsg(_T("send GetRoomAddrReq\r\n"));
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, GetRoomAddr);
	kvdata.SetValue(KEY_RoomID, room_info.RoomID);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	m_InterfaceSocket.m_SendContext = context;
	m_InterfaceSocket.AsyncSelect(FD_WRITE|FD_CLOSE);
	return true;
}

bool CTractorGameDlg::OnGetRoomAddrRsp(KVData *kvdata)
{
	AppendMsg(_T("OnGetRoomAddrRsp\r\n"));
	int RoomID = -1;
	kvdata->GetValue(KEY_RoomID, RoomID);
	assert(RoomID>=0 && RoomID<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[RoomID];
	kvdata->GetValue(KEY_RoomIP, room_info.IP);
	kvdata->GetValue(KEY_RoomPort, room_info.Port);

	if(m_CurStatus == Status_PrintTableList)
		GetRoomInfoReq();
	return true;
}

void CTractorGameDlg::PrintTableList()
{
	if(m_CurStatus != Status_PrintTableList)
	{
		AppendMsg(_T("Error:current status is [PrintTableList]\r\n"));
		return ;
	}

	AppendMsg(_T("PrintTableList\r\n"));
	if(!m_RoomSocket.IsConnected)
	{
		GetRoomAddrReq();
	}
	else
	{
		GetRoomInfoReq();
	}
}


bool CTractorGameDlg::GetRoomInfoReq()
{
	AppendMsg(_T("Get Room info\r\n"));
	if(!m_RoomSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game room,wait please...\r\n"));

		assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
		RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];
		CString IP(room_info.IP.c_str());
		m_RoomSocket.Create();
		if(TRUE==m_RoomSocket.Connect(IP, room_info.Port))
		{
			m_RoomSocket.IsConnected = TRUE;
			AppendMsg(_T("connect game room successful.\r\n"));
		}
		else
		{
			uint32_t error_code = WSAGetLastError();
			CString temp;
			if(error_code == WSAEWOULDBLOCK)
			{
				temp.Format(_T("connect game room WouldBlock\r\n"));
				AppendMsg(temp);
				m_InterfaceSocket.AsyncSelect(FD_READ|FD_CONNECT);
			}
			else
			{
				temp.Format(_T("connect interface failed. error_code=%u\r\n"), error_code);
				AppendMsg(temp);
			}
			return true;
		}
	}
	//KillTimer(2);

	AppendMsg(_T("send GetRoomInfoReq\r\n"));
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, GetRoomInfo);
	kvdata.SetValue(KEY_RoomID, room_info.RoomID);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	m_RoomSocket.m_SendContext = context;
	m_RoomSocket.AsyncSelect(FD_WRITE);

	
	return true;
}

bool CTractorGameDlg::OnGetRoomInfoRsp(KVData *kvdata)
{
	int RoomID;
	int TableNum;
	unsigned int size;
	char *NumArray;

	AppendMsg(_T("OnGetRoomInfoRsp\r\n"));

	kvdata->GetValue(KEY_RoomID, RoomID);
	RoomInfo &room_info = m_RoomList[RoomID];
	assert(room_info.RoomID == RoomID);

	kvdata->GetValue(KEY_ClientNum, room_info.ClientNum);
	kvdata->GetValue(KEY_TableNum, TableNum);
	kvdata->GetValue(KEY_PlayerNum, room_info.PlayerNum);
	kvdata->GetValue(KEY_NumArray, NumArray, size);
	assert(size == sizeof(int)*TableNum);
	room_info.TableArray.clear();
	for(int i=0; i<TableNum; ++i)
	{
		room_info.TableArray.push_back(ntohl(*(int*)NumArray));
		NumArray += sizeof(int);
	}

	if(m_CurStatus == Status_PrintTableList)
	{
		assert(m_SelectRoomIndex == RoomID);
		CString temp;
		temp.Format(_T("->房间[%02d] %d 人"), room_info.RoomID, room_info.ClientNum);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);

		m_TableListCtrl.SetRedraw(FALSE);
		m_TableListCtrl.DeleteAllItems();
		for(int i=0; i<room_info.TableArray.size(); ++i)
		{
			int cur_num = 0;
			int player_array = room_info.TableArray[i];
			for(int j=0; j<room_info.PlayerNum; ++j)
				if(player_array & 1<<j)
					++cur_num;
			temp.Format(_T("桌子[%d] %d人"), i+1, cur_num);
			m_TableListCtrl.InsertItem(i, temp, 0);
		}
		m_TableListCtrl.SetRedraw(TRUE);

		InvalidateRect(m_TableRect);
	}

	//SetTimer(2, 2000, NULL);
	return true;
}

void CTractorGameDlg::OnAddGame()
{
	if(m_CurStatus != Status_Playing)
		return ;
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		PrintTableList();
		return;
	}

	//TODO:画图
	//
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, AddGame);
	kvdata.SetValue(KEY_RoomID, room_info.RoomID);
	kvdata.SetValue(KEY_TableID, m_SelectTableIndex);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "AddGame";

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);
	
	m_RoomSocket.m_SendContext = context;
	m_RoomSocket.AsyncSelect(FD_WRITE|FD_READ|FD_CLOSE);

	CString temp;
	temp.Format(_T("send AddGame request\r\n"));
	AppendMsg(temp);
}

void CTractorGameDlg::OnAddGameRsp(KVData *kvdata)
{
	if(m_CurStatus != Status_Playing)
		return ;

	string WelcomeMsg;
	int    PlayerNum;
	int    ClientNum;
	vector<PlayerStatus> ClientStatus;
	vector<PlayerStatus> AudienceStatus;
	int    AudienceNum;
	char   *NumArray;
	int    Status;

	kvdata->GetValue(KEY_WelcomeMsg, WelcomeMsg);
	kvdata->GetValue(KEY_PlayerNum, PlayerNum);
	kvdata->GetValue(KEY_ClientNum, ClientNum);
	kvdata->GetValue(KEY_AudienceNum, AudienceNum);
	unsigned int size=0;
	kvdata->GetValue(KEY_NumArray, NumArray, size);
	assert(size == sizeof(int)*2*(ClientNum+AudienceNum));

	int *temp_buf = (int*)NumArray, i=0;
	for(i=0; i<ClientNum; ++i)
	{
		PlayerStatus player_status;
		player_status.client_id = ntohl(*temp_buf++);
		player_status.status = ntohl(*temp_buf++);
		ClientStatus.push_back(player_status);
		if(player_status.client_id == m_UID)
			Status = player_status.status;
	}
	for(int i=0; i<AudienceNum; ++i)
	{
		PlayerStatus player_status;
		player_status.client_id = ntohl(*temp_buf++);
		player_status.status = ntohl(*temp_buf++);
		AudienceStatus.push_back(player_status);
		if(player_status.client_id == m_UID)
			Status = player_status.status;
	}

	//TODO:绘图
}

void CTractorGameDlg::QuitGameReq()
{
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		PrintTableList();
		return;
	}

	//TODO:画图
	//
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, QuitGame);
	kvdata.SetValue(KEY_RoomID, room_info.RoomID);
	kvdata.SetValue(KEY_TableID, m_SelectTableIndex);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "QuitGame";

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	m_RoomSocket.m_SendContext = context;
	m_RoomSocket.AsyncSelect(FD_WRITE|FD_READ|FD_CLOSE);

	m_SelectTableIndex = -1;
	CString temp;
	temp.Format(_T("send QuitGame request\r\n"));
	AppendMsg(temp);
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
	uname.ReleaseBuffer();
	m_UName = unameA.GetBuffer();
	unameA.ReleaseBuffer();

	CString msg = _T("user sign in:uid=")+uid+_T(",uname=")+uname+_T("\r\n");
	AppendMsg(msg);

	PrintRoomList();
}

void CTractorGameDlg::OnStaticListClick()
{
	if(m_CurStatus == Status_PrintTableList)
	{
		//KillTimer(2);
		m_TableListCtrl.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_HIDE);
		m_RoomListCtrl.ShowWindow(SW_NORMAL);

		m_CurStatus = Status_PrintRoomList;
		PrintRoomList();
	}
}

void CTractorGameDlg::OnStaticRoomClick()
{
	if(m_CurStatus == Status_Playing)
	{
		GetDlgItem(IDC_STATIC_TABLE)->ShowWindow(SW_HIDE);

		CString temp;

		temp.Format(_T("->房间[%02d] %d 人"), m_SelectRoomIndex+1, m_RoomList[m_SelectRoomIndex].ClientNum);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		m_TableListCtrl.ShowWindow(SW_NORMAL);
		QuitGameReq();
		m_CurStatus = Status_PrintTableList;
		//PrintTableList();
	}
}

void CTractorGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent == 1)  //打印房间列表
	{
		AppendMsg(_T("PrintRoomList\r\n"));
		PrintRoomList();
	}
	else if(nIDEvent == 2)  //打印房间信息
	{
		AppendMsg(_T("PrintTableList\r\n"));
		PrintTableList();
	}
	CDialog::OnTimer(nIDEvent);
}

void CTractorGameDlg::OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index = pNMItemActivate->iItem;
	if(index >=0 && index<m_RoomListCtrl.GetItemCount())
	{
		CString temp;
		temp.Format(_T("OnDBClick. Into Room[%d]\r\n"), index);
		AppendMsg(temp);

		KillTimer(1);
		m_SelectRoomIndex = index;
		m_RoomListCtrl.ShowWindow(SW_HIDE);

		temp.Format(_T("->房间[%02d]"), index+1);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		//m_TableListCtrl.ShowWindow(SW_NORMAL);

		
		m_CurStatus = Status_PrintTableList;
		PrintTableList();
	}

	*pResult = 0;
}

void CTractorGameDlg::OnNMDblclkTablelist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int index = pNMItemActivate->iItem;
	if(index >=0 && index<m_TableListCtrl.GetItemCount())
	{
		CString temp;
		temp.Format(_T("Into Table[%d]\r\n"), index);
		AppendMsg(temp);

		//KillTimer(2);
		m_SelectTableIndex = index;
		m_TableListCtrl.ShowWindow(SW_HIDE);

		temp.Format(_T("->房间[%02d]"), m_SelectRoomIndex+1);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		temp.Format(_T("->桌子[%02d]"), m_SelectTableIndex+1);
		GetDlgItem(IDC_STATIC_TABLE)->SetWindowText(temp);
		GetDlgItem(IDC_STATIC_TABLE)->ShowWindow(SW_NORMAL);
		m_TableListCtrl.ShowWindow(SW_HIDE);

		m_CurStatus = Status_Playing;
		
		OnAddGame();
	}

	*pResult = 0;
}


void CTractorGameDlg::OnPaint_TableList(CRect &rect)
{
	const int HEIGHT = 100;
	const int WIDTH = 100;
	const int PADDING = 40;
	const int VSCROLL_WIDTH = 16;
	
	int x_offer =((rect.right-rect.left-VSCROLL_WIDTH)-(6*WIDTH+5*PADDING))/2;
	int bottom = 0;

	CRect draw_rect = rect;
	draw_rect.left += x_offer;
	draw_rect.right = draw_rect.left+WIDTH;
	draw_rect.bottom = draw_rect.top+HEIGHT;

	CPaintDC dc(this);
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];
	for(int i=0; i<room_info.TableArray.size(); ++i)
	{
		bottom = draw_rect.bottom;
		CRect table_rect, player_rect;
		CBrush *old_brush = (CBrush *)dc.SelectObject(GetStockObject(LTGRAY_BRUSH)); 
		dc.Rectangle(&draw_rect);
		dc.SelectObject(GetStockObject(WHITE_BRUSH)); 

		//table
		table_rect.left = draw_rect.left+25;
		table_rect.right = table_rect.left+50;
		table_rect.top = draw_rect.top+25;
		table_rect.bottom = table_rect.top+50;
		dc.Rectangle(&table_rect);

		int PlayerArray = room_info.TableArray[i];

		for(int j=0; j<room_info.PlayerNum; ++j)
		{
			if((PlayerArray & (1<<j)) == 0)
				continue;

			int x, y;
			if(j==0)
			{
				x = (draw_rect.left+table_rect.left)/2;
				y = (draw_rect.top+draw_rect.bottom)/2;
			}
			else if(j==1)
			{
				x = (draw_rect.left+draw_rect.right)/2;
				y = (draw_rect.top+table_rect.top)/2;
			}
			else if(j==2)
			{
				x = (table_rect.right+draw_rect.right)/2;
				y = (draw_rect.top+draw_rect.bottom)/2;
			}
			else
			{
				x = (draw_rect.left+draw_rect.right)/2;
				y = (table_rect.bottom+draw_rect.bottom)/2;
			}

			player_rect.left = x-10;
			player_rect.right = x+10;
			player_rect.top = y-10;
			player_rect.bottom = y+10;

			dc.Ellipse(&player_rect);
			dc.SelectObject(old_brush);
		}

		if((i+1) % 6 == 0)
		{
			draw_rect.left = rect.left+x_offer;
			draw_rect.right = draw_rect.left+WIDTH;

			draw_rect.top = draw_rect.bottom+PADDING;
			draw_rect.bottom = draw_rect.top+HEIGHT;
		}
		else
		{
			draw_rect.left = draw_rect.right+PADDING;
			draw_rect.right = draw_rect.left+WIDTH;
		}
	}

	//画滚动条
	draw_rect.left = rect.right-VSCROLL_WIDTH;
	draw_rect.right = rect.right;
	draw_rect.top = rect.top;
	draw_rect.bottom = rect.bottom;

	CPen pen(PS_SOLID, 1, RGB(200,200,200));
	CPen *old_pen = dc.SelectObject(&pen);
	dc.Rectangle(&draw_rect);

	if(bottom-rect.top > rect.Height())
	{
		int vscroll_height = rect.Height()*rect.Height()/(bottom-rect.top);
		draw_rect.bottom = draw_rect.top+vscroll_height;
		draw_rect.left++;
		draw_rect.right--;
		CPen pen(PS_SOLID, 2, RGB(100,100,100));
		dc.SelectObject(&pen);
		dc.Rectangle(&draw_rect);
	}
	dc.SelectObject(old_pen);
}