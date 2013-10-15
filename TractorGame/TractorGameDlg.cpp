
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
//#define INTERFACE_IP  _T("192.168.179.129")

#define TABLE_NUM             6       //一行显示桌子数
#define HEIGHT                      100  //桌子高
#define WIDTH                       100  //桌子宽
#define PADDING                   10    //桌子间隔
#define VSCROLL_WIDTH   16   //滚动条宽


void CTractorGameDlg::SendProtocol(ProtocolContext *context, CGameSocket *gamesocket)
{
	CString msg(context->Info.c_str());
	msg += _T(":Start to send\r\n");

	//设置socket数据
	if(gamesocket->IsConnected == FALSE)
	{
		//connect to interface
		AppendMsg(_T("connect to server,wait please...\r\n"));

		gamesocket->Create();
		if(TRUE==gamesocket->Connect(gamesocket->IP, gamesocket->Port))
		{
			gamesocket->IsConnected = TRUE;
			AppendMsg(_T("connect to server successful.\r\n"));
		}
		else
		{
			CString temp;
			uint32_t error_code = WSAGetLastError();

			if(error_code == WSAEWOULDBLOCK)
			{
				temp.Format(_T("connect server WouldBlock Port=%d\r\n"), gamesocket->Port);
				AppendMsg(temp);

				gamesocket->m_SendContext = context;
				gamesocket->AsyncSelect(FD_CONNECT);
			}
			else
			{
				temp.Format(_T("connect server failed. error_code=%u\r\n"), error_code);
				AppendMsg(temp);
				gamesocket->Close();
			}
			return ;
		}
	}
	else
	{
		int send_size = gamesocket->Send(context->Buffer, context->Size);
		if(send_size != context->Size)
		{
			if((send_size == -1&&WSAGetLastError()==WSAEWOULDBLOCK) || send_size>0)
			{
				context->send_size = send_size>0?send_size:0;
				gamesocket->m_SendContext = context;
				gamesocket->AsyncSelect(FD_WRITE|FD_CLOSE);
				return ;
			}
			else
				AppendMsg(_T("send GetRoomListReq error.\r\n"));
		}
		else
		{
			AppendMsg(_T("send GetRoomListReq finished.\r\n"));
		}
		delete context;
	}
}

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
}

BEGIN_MESSAGE_MAP(CTractorGameDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_LOAD, &CTractorGameDlg::OnBnClickedLoad)
	ON_BN_CLICKED(IDC_STATIC_ROOM, &CTractorGameDlg::OnStaticRoomClick)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_RoomList, &CTractorGameDlg::OnNMDblclkRoomlist)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_STARTGAME, &CTractorGameDlg::OnBnClickedStartgame)
	ON_BN_CLICKED(IDC_ADDGAME, &CTractorGameDlg::OnBnClickedAddgame)
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

	//Socket
	m_InterfaceSocket.IP = INTERFACE_IP;
	m_InterfaceSocket.Port = 3000;


	// TODO: 在此添加额外的初始化代码
	m_UID = -1;
	m_SelectRoomIndex = -1;
	m_SelectTableIndex = -1;

	//房间列表
	m_RoomListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB |LVS_EX_GRIDLINES);
	m_RoomListCtrl.InsertColumn(0, _T(""), LVCFMT_LEFT, 150);

	//WSADATA wsadata;
	//WSAStartup(MAKEWORD(2,0), &wsadata);

	m_CurStatus = Status_PrintTableList;

	//绘图
	m_VScrollBarHeight = 0;
	m_VScrollBarYOffset = 0;
	m_TableRectXOffset = 0;
	m_LButtonDown = FALSE;
	GetDlgItem(IDC_RoomRect)->GetWindowRect(&m_TableRect);
	ScreenToClient(&m_TableRect);

	m_BgBmp.LoadBitmap(IDB_BG);
	m_TableBmp.LoadBitmap(IDB_TABLE);
	m_BgBmp2.LoadBitmap(IDB_BG2);

	//加载扑克牌
	for(int i=0; i<54; ++i)
		m_PokerBmp[i].LoadBitmap(IDB_HongXin2+i);
	m_BgMaskBmp.LoadBitmap(IDB_PokerBgMask);
	m_FtMaskBmp.LoadBitmap(IDB_PokerFtMask);
	m_HBackBmp.LoadBitmap(IDB_PokerHBack);
	m_VBackBmp.LoadBitmap(IDB_PokerVBack);

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
			AppendMsg(_T("Status_PrintTableList\r\n"));
			OnPaint_TableList(m_TableRect);
		}
		else if(m_CurStatus == Status_Playing)
		{
			AppendMsg(_T("Status_Playing\r\n"));
			OnPaint_Talbe(m_TableRect);
		}
		else
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

void CTractorGameDlg::OnReceiveProtocol(KVData *kvdata)
{
	int Protocol;
	kvdata->GetValue(KEY_Protocol, Protocol);
	if(Protocol == PRO_GetRoomListRsp)
		OnGetRoomListRsp(kvdata);
	else if(Protocol == PRO_GetRoomAddrRsp)
		OnGetRoomAddrRsp(kvdata);
	else if(Protocol == PRO_RoomInfoBroadCast)
		OnRoomInfoBroadCast(kvdata);
	else if(Protocol == PRO_TableInfoBroadCast)
		OnTableInfoBroadCast(kvdata);
	else if(Protocol == PRO_DealPoker)
		OnDealPoker(kvdata);
	else
		assert(0);
}

bool CTractorGameDlg::GetRoomListReq()
{
	KillTimer(1);

	//发送GetAllRoom请求到interface
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_GetRoomList);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	//序列化数据
	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "GetRoomList";
	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	SendProtocol(context, &m_InterfaceSocket);
		
	return true;
}

bool CTractorGameDlg::OnGetRoomListRsp(KVData *kvdata)
{
	int RoomNum;
	char *NumArray;

	AppendMsg(_T("OnGetRoomListRsp\r\n"));
	kvdata->GetValue(KEY_RoomNum, RoomNum);
	m_RoomList.clear();    //应该上锁的,否则画图的时候取出对于的room_info会有问题
	if(RoomNum > 0)
	{
		uint32_t size;
		kvdata->GetValue(KEY_Array, NumArray, size);
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
	m_RoomListCtrl.SetRedraw(FALSE);
	m_RoomListCtrl.DeleteAllItems();
	for(int i=0; i<m_RoomList.size(); ++i)
	{
		CString temp;
		temp.Format(_T("房间%d(%d人)"),i+1, m_RoomList[i].ClientNum);
		m_RoomListCtrl.InsertItem(i, temp);
	}
	m_RoomListCtrl.SetRedraw(TRUE);


	if(RoomNum>0 && m_SelectRoomIndex==-1)
	{
		m_SelectRoomIndex = 0;
		m_SelectRoom = m_RoomList[0];
	}

	SetTimer(1, 3000, NULL);

	//请求房间地址
	if(m_CurStatus==Status_PrintTableList && !m_RoomSocket.IsConnected &&  m_SelectRoomIndex>=0)
		GetRoomAddrReq();
	return true;
}

bool CTractorGameDlg::GetRoomAddrReq()
{
	assert(m_SelectRoomIndex>=0 &&  m_SelectRoomIndex<m_RoomList.size());

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_GetRoomAddr);
	kvdata.SetValue(KEY_RoomID, m_SelectRoom.RoomID);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	//序列化数据
	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "GetRoomAddrReq";
	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	SendProtocol(context, &m_InterfaceSocket);

	return true;
}

bool CTractorGameDlg::OnGetRoomAddrRsp(KVData *kvdata)
{
	AppendMsg(_T("OnGetRoomAddrRsp\r\n"));
	int RoomID = -1;
	kvdata->GetValue(KEY_RoomID, RoomID);
	assert(RoomID>=0 && RoomID==m_SelectRoom.RoomID);
	kvdata->GetValue(KEY_RoomIP, m_SelectRoom.IP);
	kvdata->GetValue(KEY_RoomPort, m_SelectRoom.Port);

	if(m_CurStatus == Status_PrintTableList)
		IntoRoomReq();
	return true;
}

bool CTractorGameDlg::IntoRoomReq()
{
	AppendMsg(_T("send IntoRoom request\r\n"));
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_IntoRoom);
	kvdata.SetValue(KEY_RoomID, m_SelectRoom.RoomID);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "IntoRoom";

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	CString ip(m_SelectRoom.IP.c_str());
	m_RoomSocket.IP = ip;
	m_RoomSocket.Port = m_SelectRoom.Port;
	SendProtocol(context, &m_RoomSocket);

	return true;
}

bool CTractorGameDlg::LeaveRoomReq()
{
	AppendMsg(_T("LeaveRoomReq\r\n"));
	if(!m_RoomSocket.IsConnected)
		return  true;
	//KillTimer(2);
	AppendMsg(_T("send LeaveRoomReq request\r\n"));
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_LeaveRoom);
	kvdata.SetValue(KEY_RoomID, m_SelectRoom.RoomID);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "LeavRoom";

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	m_RoomSocket.m_SendContext = context;
	m_RoomSocket.AsyncSelect(FD_WRITE);
	return true;
}

bool CTractorGameDlg::OnRoomInfoBroadCast(KVData *kvdata)
{
	int RoomID;
	int TableNum;
	unsigned int size;
	char *NumArray;

	kvdata->GetValue(KEY_RoomID, RoomID);
	assert(m_SelectRoom.RoomID == RoomID);

	kvdata->GetValue(KEY_ClientNum, m_SelectRoom.ClientNum);
	kvdata->GetValue(KEY_TableNum, TableNum);
	kvdata->GetValue(KEY_NeedNum, m_SelectRoom.NeedNum);
	kvdata->GetValue(KEY_Array, NumArray, size);
	assert(size == sizeof(int)*TableNum);

	CString msg;
	msg.Format(_T("OnRoomInfoBroadCast. RoomID=%d, NeedNum=%d, CurStat=%d\r\n"), RoomID, m_SelectRoom.NeedNum, m_CurStatus);
	AppendMsg(msg);

	m_SelectRoom.TableArray.clear();
	for(int i=0; i<TableNum; ++i)
	{
		m_SelectRoom.TableArray.push_back(ntohl(*(int*)NumArray));
		NumArray += sizeof(int);
	}

	if(m_CurStatus == Status_PrintTableList)
	{
		assert(m_SelectRoomIndex == RoomID);
		CString temp;
		temp.Format(_T("房间[%02d] %d人"), m_SelectRoom.RoomID, m_SelectRoom.ClientNum);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
		InvalidateRect(m_TableRect);
	}

	return true;
}

void CTractorGameDlg::OnAddGame()
{
	if(m_CurStatus != Status_Playing)
		return ;
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		IntoRoomReq();
		return;
	}

	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_AddGame);
	kvdata.SetValue(KEY_RoomID, m_SelectRoom.RoomID);
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
	
	SendProtocol(context, &m_RoomSocket);
}

void CTractorGameDlg::OnTableInfoBroadCast(KVData *kvdata)
{
	if(m_CurStatus != Status_Playing)
		return ;

	string Message;
	int    NeedNum;
	int    PlayerNum;
	int    AudienceNum;
	char   *NumArray;

	kvdata->GetValue(KEY_Message, Message);
	kvdata->GetValue(KEY_NeedNum, NeedNum);
	kvdata->GetValue(KEY_PlayerNum, PlayerNum);
	kvdata->GetValue(KEY_AudienceNum, AudienceNum);
	unsigned int size=0;
	kvdata->GetValue(KEY_Array, NumArray, size);
	assert(size == sizeof(int)*2*(PlayerNum+AudienceNum));

	CString temp(Message.c_str());
	CString msg(_T("TableInfo message:"));
	msg += temp + _T("\r\n");
	AppendMsg(msg);

	int *temp_buf = (int*)NumArray, i=0;
	m_PlayerStatus.clear();
	m_AudienceStatus.clear();
	for(i=0; i<PlayerNum; ++i)
	{
		TableStatus player_status;
		player_status.client_id = ntohl(*temp_buf++);
		player_status.status = ntohl(*temp_buf++);
		m_PlayerStatus.push_back(player_status);
		if(player_status.client_id == m_UID)
			m_MyStatus = player_status.status;
	}
	for(int i=0; i<AudienceNum; ++i)
	{
		TableStatus audience_status;
		audience_status.client_id = ntohl(*temp_buf++);
		audience_status.status = ntohl(*temp_buf++);
		m_AudienceStatus.push_back(audience_status);
		if(audience_status.client_id == m_UID)
			m_MyStatus = audience_status.status;
	}

	if(m_MyStatus == 2)  //等待状态
	{
		GetDlgItem(IDC_STARTGAME)->ShowWindow(SW_NORMAL);
		GetDlgItem(IDC_ADDGAME)->ShowWindow(SW_HIDE);
	}
	else if(m_MyStatus == 1)  //旁观者
	{
		GetDlgItem(IDC_STARTGAME)->ShowWindow(SW_HIDE);
		if(PlayerNum < NeedNum)
			GetDlgItem(IDC_ADDGAME)->ShowWindow(SW_NORMAL);
		else
			GetDlgItem(IDC_ADDGAME)->ShowWindow(SW_HIDE);
	}
	else
	{
		GetDlgItem(IDC_STARTGAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ADDGAME)->ShowWindow(SW_HIDE);
	}

	InvalidateRect(&m_TableRect);
}

void CTractorGameDlg::QuitGameReq()
{
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		IntoRoomReq();
		return;
	}

	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_QuitGame);
	kvdata.SetValue(KEY_RoomID, m_SelectRoom.RoomID);
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

void CTractorGameDlg::OnDealPoker(KVData *kvdata)
{
	int CardFlag = -1;
	char *Array = NULL;
	uint32_t size;
	int poker[20];

	kvdata->GetValue(KEY_CardFlag, CardFlag);
	bool IsSetArray  = kvdata->GetValue(KEY_Array, Array, size);
	assert((m_MyStatus==1 && IsSetArray==true) || (m_MyStatus==3 && IsSetArray==true));
	
	int CardNum = 0;
	if(IsSetArray == true)
	{
		CardNum = CardFlag==0?1:CardFlag;
		int *ppoker = (int*)Array;
		for(int i=0; i<CardNum; ++i)
			poker[i] = ntohl(ppoker[i]);
	}

	int my_index = 0;
	for(int i=0; i<m_PlayerStatus.size(); ++i)
	{
		if(m_PlayerStatus[i].client_id==m_UID)
		{
			my_index = i;
			for(int j=0; j<CardNum; j++)
			{
				m_PlayerStatus[i].poker.push_back(poker[j]);
				int pos = m_PlayerStatus[i].poker.size()-1;
				while(pos > 0)
				{
					if( poker[j] <= m_PlayerStatus[i].poker[pos-1])
						break;
					m_PlayerStatus[i].poker[pos] = m_PlayerStatus[i].poker[pos-1];
					--pos;
				}	
				m_PlayerStatus[i].poker[pos] = poker[j];
			}
		}
		else
			m_PlayerStatus[i].poker.push_back(-1);
	}

	InvalidateRect(&m_TableRect);

	CString msg;
	msg.Format(_T("OnDealPoker:cur Poker=%u\r\n"), m_PlayerStatus[my_index].poker.size());
	AppendMsg(msg);
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

	//请求房间列表
	GetRoomListReq();
}

void CTractorGameDlg::OnStaticRoomClick()
{
	if(m_CurStatus == Status_Playing)
	{
		GetDlgItem(IDC_STATIC_TABLE)->ShowWindow(SW_HIDE);

		CString temp;

		temp.Format(_T("->房间[%02d] %d 人"), m_SelectRoomIndex+1, m_RoomList[m_SelectRoomIndex].ClientNum);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
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
		AppendMsg(_T("Timer:GetRoomListReq\r\n"));
		GetRoomListReq();
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

		temp.Format(_T("房间[%02d]"), index+1);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);

		if(m_SelectRoomIndex != index)  //选了另外的一个房间
		{
			CString msg;
			msg.Format(_T("select another room=%d\r\n"), index);
			AppendMsg(msg);

			//LeaveRoomReq();
			if(m_RoomSocket.IsConnected)  //直接关掉让服务器处理对应的事件
				m_RoomSocket.Close();
			m_CurStatus = Status_PrintTableList;
			m_SelectRoomIndex = index;
			GetRoomAddrReq();
		}
	}

	*pResult = 0;
}

void CTractorGameDlg::OnPaint_TableList(CRect &client_rect)
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	CDC MemDc;
	MemDc.CreateCompatibleDC(&dc);
	CBitmap *old_bitmap = MemDc.SelectObject(&m_TableBmp);

	CPen *old_pen;
	CRect rect, draw_rect;

	//画背景
	CBrush gb_brush(&m_BgBmp);
	CBrush *old_brush = dc.SelectObject(&gb_brush);
	dc.Rectangle(&client_rect);
	 dc.SelectObject(old_brush);

	//画滚动条外框
	draw_rect.left = client_rect.right-VSCROLL_WIDTH;
	draw_rect.right = client_rect.right;
	draw_rect.top = client_rect.top;
	draw_rect.bottom = client_rect.bottom;
	CPen pen(PS_SOLID, 1, RGB(200,200,200));
	old_pen = dc.SelectObject(&pen);
	dc.Rectangle(&draw_rect);

	rect.left = client_rect.left+2;
	rect.right = client_rect.right-VSCROLL_WIDTH-2;
	rect.top = client_rect.top+2;
	rect.bottom = client_rect.bottom-2;

	if(m_SelectRoomIndex<0)
		return ;

	//画滚动条
	int RowNum = m_SelectRoom.TableArray.size()/TABLE_NUM+(m_SelectRoom.TableArray.size()%TABLE_NUM>0?1:0);  //行数
	int ViewHeight = RowNum*HEIGHT+(RowNum-1)*PADDING;  //总的高度
	if(ViewHeight > rect.Height())  //有滚动条
	{
		//滚动条高度
		m_VScrollBarHeight = client_rect.Height()*rect.Height()/(ViewHeight);
		//滚动条位置
		draw_rect.top += m_VScrollBarYOffset;
		draw_rect.bottom = draw_rect.top+m_VScrollBarHeight;
		draw_rect.left++;
		draw_rect.right--;
		CPen pen(PS_SOLID, 2, RGB(100,100,100));
		dc.SelectObject(&pen);
		dc.Rectangle(&draw_rect);
	}

	//画桌子
	m_TableRectXOffset = (rect.Width()-(TABLE_NUM*WIDTH+(TABLE_NUM-1)*PADDING))/2;
	int y_offset = -1*(ViewHeight*m_VScrollBarYOffset/client_rect.Height());  //视图Y轴偏远位置

	draw_rect = rect;
	draw_rect.left += m_TableRectXOffset;
	draw_rect.right = draw_rect.left+WIDTH;
	draw_rect.top = rect.top+y_offset;
	draw_rect.bottom = draw_rect.top+HEIGHT;

	//画桌子
	dc.SelectStockObject(NULL_PEN);
	CBrush brush(RGB(255, 200, 0));
	dc.SelectObject(&brush);
	for(int i=0; i<m_SelectRoom.TableArray.size(); ++i)
	{
		CRect player_rect;
		dc.BitBlt(draw_rect.left, draw_rect.top, draw_rect.Width(), draw_rect.Width(), &MemDc, 0, 0, SRCCOPY);

		CString lable;
		lable.Format(_T("%d"), i+1);
		dc.TextOut((draw_rect.left+draw_rect.right)/2,(draw_rect.top+draw_rect.bottom)/2, lable, lable.GetLength());

		int PlayerArray = m_SelectRoom.TableArray[i];

		for(int j=0; j<m_SelectRoom.NeedNum; ++j)
		{
			if((PlayerArray & (1<<j)) == 0)
				continue;

			int x, y;
			if(j==0)
			{
				x = draw_rect.left+13;
				y = (draw_rect.top+draw_rect.bottom)/2;
			}
			else if(j==1)
			{
				x = (draw_rect.left+draw_rect.right)/2;
				y = draw_rect.top+13;
			}
			else if(j==2)
			{
				x = draw_rect.right-13;
				y = (draw_rect.top+draw_rect.bottom)/2;
			}
			else
			{
				x = (draw_rect.left+draw_rect.right)/2;
				y = draw_rect.bottom-13;
			}

			player_rect.left = x-10;
			player_rect.right = x+10;
			player_rect.top = y-10;
			player_rect.bottom = y+10;
			
			dc.Ellipse(&player_rect);
		}

		if((i+1) % TABLE_NUM == 0)
		{
			draw_rect.left = rect.left+m_TableRectXOffset;
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
	MemDc.SelectObject(old_bitmap);

	dc.SelectObject(old_pen);
	dc.SelectObject(GetStockObject(NULL_BRUSH));
	dc.Rectangle(&client_rect);
	dc.SelectObject(old_brush);

	
}


void CTractorGameDlg::OnPaint_Talbe(CRect &client_rect)
{
	CPaintDC dc(this);
	dc.SetBkMode(TRANSPARENT);

	CRect rect, draw_rect;

	//画外框
	CPen pen(PS_SOLID, 3, RGB(158, 44, 104));
	CBrush gb_brush(&m_BgBmp2);
	CPen *old_pen = dc.SelectObject(&pen);
	CBrush *old_brush = dc.SelectObject(&gb_brush);
	dc.Rectangle(&client_rect);

	//画分割线
	rect = client_rect;
	rect.left = rect.right-200;
	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.left, rect.bottom);
	dc.SelectStockObject(NULL_PEN);

	rect.right = rect.left-6;
	rect.left = client_rect.left+6;
	rect.top += 6;
	rect.bottom -= 6;

	//圆
	CBrush brush(RGB(255, 200, 0));	
	dc.SelectObject(&brush);

	int i, index = -1;
	for(i=0; i<m_PlayerStatus.size(); ++i)
	{
		if(m_PlayerStatus[i].client_id == m_UID)
		{
			index = i;   //自己在桌子中的位置
			break;
		}
	}
	int delta = 0;
	if(index != -1)
		delta = (m_SelectRoom.NeedNum-1)-index;   // 玩家自己的位置顺时针移动的个数,以便显示在下面的位置
	int x, y;

	CString msg;
	msg.Format(_T("my_index=%d, needNum=%d, delta=%d\r\n"), index, m_SelectRoom.NeedNum, delta);
	AppendMsg(msg);

	CString StatusStr[4]={_T("UnKnow"),_T("Audience"),_T("Wait"), _T("Playing")};

	for(i=0; i<m_PlayerStatus.size(); ++i)
	{
		int pos = (i+delta)%m_SelectRoom.NeedNum;
		if(pos == 0)
		{
			x = rect.left+20;
			y = (rect.bottom+rect.top)/2;
		}
		else if(pos == 1)
		{
			x = (rect.left+rect.right)/2;
			y = rect.top+20;
		}
		else if(pos == 2)
		{
			x = rect.right-20;
			y = (rect.bottom+rect.top)/2;
		}
		else if(pos == 3)
		{
			x = (rect.left+rect.right)/2;
			y = rect.bottom-20;
		}

		draw_rect.left = x-20;
		draw_rect.right = x+20;
		draw_rect.top = y-20;
		draw_rect.bottom = y+20;

		if(pos==3 && m_MyStatus>1)  //自己是玩家
		{
			CBrush temp_brush(RGB(34,177,76));
			dc.SelectObject(&temp_brush);
			dc.Ellipse(&draw_rect);
			dc.SelectObject(&brush);
		}
		else
			dc.Ellipse(&draw_rect);

		CString lable, poker;
		lable.Format(_T("uid[%d]:"), m_PlayerStatus[i].client_id);
		lable += StatusStr[m_PlayerStatus[i].status];
		if(pos==3 && m_MyStatus>1)
		{
			COLORREF old_color = dc.GetTextColor();
			dc.SetTextColor(RGB(255, 0, 0));
			dc.TextOut(x, y, lable, lable.GetLength());
			dc.SetTextColor(old_color);
		}
		else
			dc.TextOut(x, y, lable, lable.GetLength());

		//画扑克

		CRect poker_rect;
		if(m_PlayerStatus[i].poker.size() > 0)
		{
			int poker_width = 0;
			int poker_height = 0;
			if(pos == 0 || pos ==2)
			{
				poker_width = 140;
				poker_height = (m_PlayerStatus[i].poker.size()-1)*18+100;
				poker_rect.top = y-poker_height/2;
				poker_rect.left = pos==0?x+40:x-140;
				poker_rect.bottom = poker_rect.top+100;
				poker_rect.right = poker_rect.left+140;
			}
			else
			{
				poker_height = 140;
				poker_width = (m_PlayerStatus[i].poker.size()-1)*18+100;
				poker_rect.left = x-poker_width/2;
				poker_rect.top = pos==1?y+40:y-140;
				poker_rect.bottom = poker_rect.top+140;
				poker_rect.right = poker_rect.left+100;
			}

			/*
			if(pos == 3)
			{
				CDC MemDC_BgMask;
				MemDC_BgMask.CreateCompatibleDC(&dc);
				MemDC_BgMask.SelectObject(&m_BgMaskBmp);

				for(int npoker=0; npoker<m_PlayerStatus[i].poker.size(); ++npoker)
				{
					dc.BitBlt(poker_rect.left, poker_rect.top, poker_rect.Width(),poker_rect.Height(), &MemDC_BgMask, 0,0,SRCAND);

					CBitmap BgBmp;
					BgBmp.CreateCompatibleBitmap(&dc, 100, 140);

					CDC MemDC;
					MemDC.CreateCompatibleDC(&dc);
					MemDC.SelectObject(&BgBmp);

					CDC MemDC_FtMask;
					MemDC_FtMask.CreateCompatibleDC(&dc);
					MemDC_FtMask.SelectObject(&m_FtMaskBmp);
					MemDC.BitBlt(0,0,100,140, &MemDC_FtMask, 0, 0, SRCCOPY);

					CDC MemDC_Front;
					MemDC_Front.CreateCompatibleDC(&dc);
					MemDC_Front.SelectObject(&m_PokerBmp[m_PlayerStatus[i].poker[npoker]]);
					CString msg;
					msg.Format(_T("%d,"), m_PlayerStatus[i].poker[npoker]);
					AppendMsg(msg);
					MemDC.BitBlt(0,0,100,140, &MemDC_Front, 0, 0, SRCAND);

					dc.BitBlt(poker_rect.left, poker_rect.top, poker_rect.Width(),poker_rect.Height(), &MemDC, 0,0,SRCPAINT);

					poker_rect.left+=18;
					poker_rect.right += 18;
				}
			}*/
			if(pos == 3)
			{
				for(int npoker=0; npoker<m_PlayerStatus[i].poker.size(); ++npoker)
				{
					DrawPoker(&dc, poker_rect, &m_PokerBmp[m_PlayerStatus[i].poker[npoker]]);
					poker_rect.left+=16;
					poker_rect.right += 16;
				}
			}
		}
	}

	//画旁观者
	x = client_rect.right-190;
	y = rect.top+6;
	for(int i=0; i<m_AudienceStatus.size(); ++i)
	{
		CString lable;
		lable.Format(_T("%d"), m_AudienceStatus[i].client_id);
		if(m_AudienceStatus[i].client_id == m_UID)
		{
			COLORREF old_color = dc.GetTextColor();
			dc.SetTextColor(RGB(255, 0, 0));
			dc.TextOut(x, y, lable, lable.GetLength());
			dc.SetTextColor(old_color);
		}
		else
			dc.TextOut(x, y, lable, lable.GetLength());
		y+=25;
	}

	dc.SelectObject(old_brush);
	dc.SelectObject(old_pen);
}

void CTractorGameDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_CurStatus == Status_PrintTableList)
	{
		if(point.x>=m_TableRect.left&&point.x<m_TableRect.right
			&&point.y>=m_TableRect.top&&point.y<m_TableRect.bottom)
			m_LButtonDown = TRUE;
		m_PreMousePoint = point;
	}
	//CDialog::OnLButtonDown(nFlags, point);
}

void CTractorGameDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_LButtonDown = FALSE;
	CDialog::OnLButtonUp(nFlags, point);
}

void CTractorGameDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_LButtonDown == TRUE)
	{
		if(m_VScrollBarHeight > 0)  //有滚动条
		{
			int y_offset = m_VScrollBarYOffset+point.y-m_PreMousePoint.y;
			m_PreMousePoint = point;
			if(y_offset >= 0 && y_offset+m_VScrollBarHeight<=m_TableRect.Height())
			{
				m_VScrollBarYOffset = y_offset;				
				InvalidateRect(&m_TableRect);
			}
		}
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CTractorGameDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(m_CurStatus == Status_PrintTableList)
	{
		if(point.x>=m_TableRect.left+2&&point.x<m_TableRect.right-VSCROLL_WIDTH-2
			&&point.y>=m_TableRect.top+2&&point.y<m_TableRect.bottom-2)
		{
			//转换点在table中的逻辑坐标

			int y_offset = m_VScrollBarHeight>0?m_VScrollBarYOffset*(m_TableRect.Height()-4)/m_VScrollBarHeight:0;
			int y = point.y-(m_TableRect.top+2)+y_offset;
			int x =  point.x-(m_TableRect.left+2)-m_TableRectXOffset;

			int row = y/(HEIGHT+PADDING) + (y%(HEIGHT+PADDING)>0?1:0);
			int column = x/(WIDTH+PADDING) + (x%(WIDTH+PADDING)>0?1:0);

			if(y<row*(HEIGHT+PADDING) -PADDING && x<column*(WIDTH+PADDING) -PADDING)   //点中的是桌子(而不是间隔)
			{
				m_CurStatus = Status_Playing;
				m_SelectTableIndex = (row-1)*TABLE_NUM+(column-1);

				CString temp;
				temp.Format(_T("Into Table[%d]\r\n"), m_SelectTableIndex+1);
				AppendMsg(temp);

				//KillTimer(2);
				temp.Format(_T("->房间[%02d]"), m_SelectRoomIndex+1);
				GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
				GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
				temp.Format(_T("->桌子[%02d]"), m_SelectTableIndex+1);
				GetDlgItem(IDC_STATIC_TABLE)->SetWindowText(temp);
				GetDlgItem(IDC_STATIC_TABLE)->ShowWindow(SW_NORMAL);

				InvalidateRect(&m_TableRect);
				OnAddGame();
			}
			
		}
	}

	CDialog::OnLButtonDblClk(nFlags, point);
}

void CTractorGameDlg::OnBnClickedStartgame()
{
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		IntoRoomReq();
		return;
	}

	assert(m_SelectRoomIndex>=0 && m_SelectRoom.RoomID>=0);

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_StartGame);
	kvdata.SetValue(KEY_RoomID, m_SelectRoom.RoomID);
	kvdata.SetValue(KEY_TableID, m_SelectTableIndex);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	KVDataProtocolFactory factory;
	unsigned int header_size = factory.HeaderSize();
	unsigned int body_size = kvdata.Size();
	ProtocolContext *context = new ProtocolContext(header_size+body_size);
	context->type = DTYPE_BIN;
	context->Info = "StartGame";

	kvdata.Serialize(context->Buffer+header_size);
	context->Size = header_size+body_size;
	factory.EncodeHeader(context->Buffer, body_size);

	m_RoomSocket.m_SendContext = context;
	m_RoomSocket.AsyncSelect(FD_WRITE|FD_READ|FD_CLOSE);

	CString temp;
	temp.Format(_T("send StartGame request\r\n"));
	AppendMsg(temp);
}

void CTractorGameDlg::OnBnClickedAddgame()
{
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		IntoRoomReq();
	}
	else
		OnAddGame();
}

void CTractorGameDlg::DrawPoker(CDC *dc, CRect &rect, CBitmap *poker_bitmap)
{
	CBitmap *old_bitmap;
	CDC mem_dc;
	mem_dc.CreateCompatibleDC(dc);
	old_bitmap = mem_dc.SelectObject(poker_bitmap);

	//3. 绘图
	dc->TransparentBlt(rect.left, rect.top, rect.Width(), rect.Height(), &  mem_dc , 0, 0, 100, 140,  RGB(255,0,255) );

	//4. 恢复
	mem_dc.SelectObject( old_bitmap);
}