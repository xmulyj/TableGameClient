
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

		m_GameSocket.Create();
		if(TRUE==m_GameSocket.Connect(INTERFACE_IP, 3000))
		{
			m_CurStatus = Status_PrintRoomList;
			m_GameSocket.IsConnected = TRUE;

			AppendMsg(_T("connect game_interface successful.\r\n"));
		}
		else
		{
			CString temp;
			uint32_t error_code = WSAGetLastError();

			if(error_code == WSAEWOULDBLOCK)
			{
				temp.Format(_T("connect interface WouldBlock\r\n"));
				AppendMsg(temp);
				m_GameSocket.AsyncSelect(FD_READ|FD_CONNECT);
			}
			else
			{
				temp.Format(_T("connect interface failed. error_code=%u\r\n"), error_code);
				AppendMsg(temp);
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

	m_GameSocket.m_SendContext = context_tmp;
	m_GameSocket.AsyncSelect(FD_WRITE|FD_CLOSE);;

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
	if(recv_size == 0)
	{
		m_GameSocket.IsConnected = FALSE;
		m_GameSocket.Close();
		AppendMsg(_T("game interface close connect\r\n"));
		return false;
	}
	else if(recv_size < 0)
	{
		CString temp;
		temp.Format(_T("Error: OnInterfaceRsp:%u\r\n"),WSAGetLastError());
		AppendMsg(temp);
		return false;
	}
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
	else if(Protocol == GetRoomAddrRsp)
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

	int i=0;
	for(i=0; i<m_RoomList.size(); ++i)  //替换现有的
	{
		CString temp;
		temp.Format(_T("%d"), i+1);
		if(i < m_RoomListCtrl.GetItemCount())
			m_RoomListCtrl.SetItemText(i, 0, temp);
		else
			m_RoomListCtrl.InsertItem(i, temp);

		temp.Format(_T("%d"), m_RoomList[i].RoomID);
		m_RoomListCtrl.SetItemText(i, 1, temp);
		
		temp.Format(_T("%d"), m_RoomList[i].ClientNum);
		m_RoomListCtrl.SetItemText(i, 2, temp);
	}
	for(; i<m_RoomListCtrl.GetItemCount(); ++i)  //删除多余的
		m_RoomListCtrl.DeleteItem(i);

	SetTimer(1, 2000, NULL);
	return true;
}

bool CTractorGameDlg::GetRoomAddrReq()
{
	AppendMsg(_T("GetRoomAddrReq\r\n"));
	if(!m_GameSocket.IsConnected)
	{
		//connect to interface
		AppendMsg(_T("connect to game interface,wait please...\r\n"));

		m_GameSocket.Create();
		if(TRUE==m_GameSocket.Connect(INTERFACE_IP, 3000))
		{
			m_CurStatus = Status_PrintRoomList;
			m_GameSocket.IsConnected = TRUE;

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
				m_GameSocket.AsyncSelect(FD_READ|FD_CONNECT);
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

	m_GameSocket.m_SendContext = context;
	m_GameSocket.AsyncSelect(FD_WRITE|FD_CLOSE);
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
				m_GameSocket.AsyncSelect(FD_READ|FD_CONNECT);
			}
			else
			{
				temp.Format(_T("connect interface failed. error_code=%u\r\n"), error_code);
				AppendMsg(temp);
			}
			return true;
		}
	}

	KillTimer(2);
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

void CTractorGameDlg::OnRoomRsp()
{
	AppendMsg(_T("OnRoomRsp\r\n"));
	//recv resp
	KVDataProtocolFactory factory;
	ProtocolContext context;

	context.header_size = factory.HeaderSize();
	context.CheckSize(context.header_size);

	int recv_size = m_RoomSocket.Receive(context.Buffer, context.header_size);
	if(recv_size == 0)
	{
		AppendMsg(_T("room server close socket\r\n"));
		m_RoomSocket.IsConnected = FALSE;
		return ;
	}
	else if(recv_size < 0)
	{
		return ;
	}
	assert(recv_size == context.header_size);

	if(DECODE_SUCC != factory.DecodeHeader(context.Buffer, context.type, context.body_size))
	{
		return ;
	}

	context.CheckSize(context.body_size);
	recv_size = m_RoomSocket.Receive(context.Buffer+context.header_size, context.body_size);
	assert(recv_size == context.body_size);
	context.Size = context.header_size+context.body_size;

	if(DECODE_SUCC != factory.DecodeBinBody(&context))
	{
		return ;
	}

	KVData *recv_kvdata = (KVData*)context.protocol;
	int Protocol = -1;
	recv_kvdata->GetValue(KEY_Protocol, Protocol);
	if(Protocol == GetRoomInfoRsp)
		OnGetRoomInfoRsp(recv_kvdata);
	if(Protocol == AddGameRsp)
		OnAddGameRsp(recv_kvdata);
	else
		assert(0);
	factory.DeleteProtocol(-1, context.protocol);
}

bool CTractorGameDlg::OnGetRoomInfoRsp(KVData *kvdata)
{
	int RoomID;
	int TableNum;
	unsigned int size;
	char *NumArray;

	kvdata->GetValue(KEY_RoomID, RoomID);
	RoomInfo &room_info = m_RoomList[RoomID];
	assert(room_info.RoomID == RoomID);

	kvdata->GetValue(KEY_ClientNum, room_info.ClientNum);
	kvdata->GetValue(KEY_TableNum, TableNum);
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

		int i=0;
		for(i=0; i<room_info.TableArray.size(); ++i)  //替换现有的
		{
			temp.Format(_T("%d"), i+1);
			if(i < m_TableListCtrl.GetItemCount())
				m_TableListCtrl.SetItemText(i, 0, temp);
			else
				m_TableListCtrl.InsertItem(i, temp);

			temp.Format(_T("%d"), room_info.TableArray[i]);
			m_TableListCtrl.SetItemText(i, 1, temp);
		}

		for(; i<m_TableListCtrl.GetItemCount(); ++i)  //删掉多余的
			m_TableListCtrl.DeleteItem(i);
	}

	SetTimer(2, 2000, NULL);
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

	kvdata.Serialize(context.Buffer+header_size);
	context.Size = header_size+body_size;
	factory.EncodeHeader(context.Buffer, body_size);
	
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
		KillTimer(2);
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

		m_CurStatus = Status_PrintTableList;
		PrintTableList();
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
		temp.Format(_T("Into Room[%d]\r\n"), index);
		AppendMsg(temp);

		KillTimer(1);
		m_SelectRoomIndex = index;
		m_RoomListCtrl.ShowWindow(SW_HIDE);

		temp.Format(_T("->房间[%02d]"), index+1);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		m_TableListCtrl.ShowWindow(SW_NORMAL);
		

		m_CurStatus = Status_PrintTableList;

		AppendMsg(_T("OnDBClick\r\n"));
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

		KillTimer(2);
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
