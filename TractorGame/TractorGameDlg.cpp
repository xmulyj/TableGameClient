
// TractorGameDlg.cpp : ʵ���ļ�
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

#define TABLE_NUM             6       //һ����ʾ������
#define HEIGHT                      100  //���Ӹ�
#define WIDTH                       100  //���ӿ�
#define PADDING                   40    //���Ӽ��
#define VSCROLL_WIDTH   16   //��������


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CTractorGameDlg �Ի���




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
	ON_BN_CLICKED(IDC_STATIC_LIST, &CTractorGameDlg::OnStaticListClick)
	ON_BN_CLICKED(IDC_STATIC_ROOM, &CTractorGameDlg::OnStaticRoomClick)
	ON_WM_TIMER()
	ON_NOTIFY(NM_DBLCLK, IDC_RoomList, &CTractorGameDlg::OnNMDblclkRoomlist)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_STARTGAME, &CTractorGameDlg::OnBnClickedStartgame)
END_MESSAGE_MAP()


// CTractorGameDlg ��Ϣ�������

BOOL CTractorGameDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_UID = -1;
	m_SelectRoomIndex = -1;
	m_SelectTableIndex = -1;


	HICON  hicon;
	hicon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ImageList.Create(64,64, ILC_COLORDDB, 1,1);
	m_ImageList.Add(hicon);

	//�����б�
	m_RoomListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_FLATSB |LVS_EX_SUBITEMIMAGES );
	m_RoomListCtrl.SetImageList(&m_ImageList,  LVSIL_NORMAL);

	//WSADATA wsadata;
	//WSAStartup(MAKEWORD(2,0), &wsadata);

	m_CurStatus = Status_PrintRoomList;

	//��ͼ
	m_VScrollBarHeight = 0;
	m_VScrollBarYOffset = 0;
	m_TableRectXOffset = 0;
	m_LButtonDown = FALSE;
	m_RoomListCtrl.GetWindowRect(&m_TableRect);
	ScreenToClient(&m_TableRect);

	m_BgBmp.LoadBitmap(IDB_BG);
	m_TableBmp.LoadBitmap(IDB_TABLE);
	m_BgBmp2.LoadBitmap(IDB_BG2);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CTractorGameDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		if(m_CurStatus == Status_PrintTableList)
		{
			OnPaint_TableList(m_TableRect);
		}
		else if(m_CurStatus == Status_Playing)
			OnPaint_Talbe(m_TableRect);
		else
			CDialog::OnPaint();
	}
}


//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
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

//��ӡ�����б�
void CTractorGameDlg::PrintRoomList()
{
	if(m_CurStatus != Status_PrintRoomList)
	{
		AppendMsg(_T("Error:current status is [PrintRoomList]\r\n"));
		return ;
	}

	GetRoomListReq();
}

bool CTractorGameDlg::GetRoomListReq()
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

	//����GetAllRoom����interface
	KVDataProtocolFactory factory;
	ProtocolContext *context_tmp = new ProtocolContext;
	ProtocolContext &context = *context_tmp;
	unsigned int header_size, body_size;

	context.type = DTYPE_BIN;
	context.Info = "GetRoomList";

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_GetRoomList);
	kvdata.SetValue(KEY_ClientID, m_UID);
	kvdata.SetValue(KEY_ClientName, m_UName);

	header_size = factory.HeaderSize();
	body_size = kvdata.Size();
	context.CheckSize(header_size+body_size);
	kvdata.Serialize(context.Buffer+header_size);
	context.Size = header_size+body_size;
	factory.EncodeHeader(context.Buffer, body_size);

	m_InterfaceSocket.m_SendContext = context_tmp;
	m_InterfaceSocket.AsyncSelect(FD_WRITE|FD_CLOSE);

	AppendMsg(_T("send GetRoomList request\r\n"));
	return true;
}

bool CTractorGameDlg::OnGetRoomListRsp(KVData *kvdata)
{
	int RoomNum;
	char *NumArray;

	AppendMsg(_T("OnGetRoomListRsp\r\n"));
	kvdata->GetValue(KEY_RoomNum, RoomNum);
	m_RoomList.clear();
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

	if(m_CurStatus != Status_PrintRoomList)
		return true;

	m_RoomListCtrl.SetRedraw(FALSE);
	m_RoomListCtrl.DeleteAllItems();
	for(int i=0; i<m_RoomList.size(); ++i)
	{
		CString temp;
		temp.Format(_T("����[%2d] %d��"), i+1, m_RoomList[i].ClientNum);
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
	kvdata.SetValue(KEY_Protocol, PRO_GetRoomAddr);
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
		IntoRoomReq();
	return true;
}

bool CTractorGameDlg::IntoRoomReq()
{
	AppendMsg(_T("IntoRoomReq\r\n"));
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

	AppendMsg(_T("send IntoRoom request\r\n"));
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_IntoRoom);
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

bool CTractorGameDlg::LeaveRoomReq()
{
	AppendMsg(_T("LeaveRoomReq\r\n"));
	if(!m_RoomSocket.IsConnected)
		return  true;
	//KillTimer(2);
	AppendMsg(_T("send LeaveRoomReq request\r\n"));
	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_LeaveRoom);
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

void CTractorGameDlg::PrintTableList()
{
	if(m_CurStatus != Status_PrintTableList)
	{
		AppendMsg(_T("Error:current status is [PrintTableList]\r\n"));
		return ;
	}

	AppendMsg(_T("PrintTableList\r\n"));
	if(!m_RoomSocket.IsConnected)
		GetRoomAddrReq();
}

bool CTractorGameDlg::OnRoomInfoBroadCast(KVData *kvdata)
{
	int RoomID;
	int TableNum;
	unsigned int size;
	char *NumArray;

	AppendMsg(_T("OnRoomInfoBroadCast\r\n"));

	kvdata->GetValue(KEY_RoomID, RoomID);
	RoomInfo &room_info = m_RoomList[RoomID];
	assert(room_info.RoomID == RoomID);

	kvdata->GetValue(KEY_ClientNum, room_info.ClientNum);
	kvdata->GetValue(KEY_TableNum, TableNum);
	kvdata->GetValue(KEY_NeedNum, room_info.NeedNum);
	kvdata->GetValue(KEY_Array, NumArray, size);
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
		temp.Format(_T("->����[%02d] %d ��"), room_info.RoomID, room_info.ClientNum);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
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

	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_AddGame);
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

	if(m_MyStatus == 2)  //�ȴ�״̬
	{
		GetDlgItem(IDC_STARTGAME)->ShowWindow(SW_NORMAL);
	}
	else
	{
		GetDlgItem(IDC_STARTGAME)->ShowWindow(SW_HIDE);
	}

	InvalidateRect(&m_TableRect);
}

void CTractorGameDlg::QuitGameReq()
{
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		PrintTableList();
		return;
	}

	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_QuitGame);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_HIDE);
		m_RoomListCtrl.ShowWindow(SW_NORMAL);

		if(m_RoomSocket.IsConnected)
			LeaveRoomReq();
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

		temp.Format(_T("->����[%02d] %d ��"), m_SelectRoomIndex+1, m_RoomList[m_SelectRoomIndex].ClientNum);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		QuitGameReq();
		m_CurStatus = Status_PrintTableList;
		//PrintTableList();
	}
}

void CTractorGameDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(nIDEvent == 1)  //��ӡ�����б�
	{
		AppendMsg(_T("PrintRoomList\r\n"));
		PrintRoomList();
	}
	else if(nIDEvent == 2)  //��ӡ������Ϣ
	{
		AppendMsg(_T("PrintTableList\r\n"));
		PrintTableList();
	}
	CDialog::OnTimer(nIDEvent);
}

void CTractorGameDlg::OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int index = pNMItemActivate->iItem;
	if(index >=0 && index<m_RoomListCtrl.GetItemCount())
	{
		CString temp;
		temp.Format(_T("OnDBClick. Into Room[%d]\r\n"), index);
		AppendMsg(temp);

		KillTimer(1);
		m_SelectRoomIndex = index;
		m_RoomListCtrl.ShowWindow(SW_HIDE);

		temp.Format(_T("->����[%02d]"), index+1);
		GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
		GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
		//m_TableListCtrl.ShowWindow(SW_NORMAL);

		
		m_CurStatus = Status_PrintTableList;
		PrintTableList();
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
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	//�����
	CBrush gb_brush(&m_BgBmp);
	CBrush *old_brush = dc.SelectObject(&gb_brush);
	dc.Rectangle(&client_rect);
	 dc.SelectObject(old_brush);

	//�����������
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

	//��������
	int RowNum = room_info.TableArray.size()/TABLE_NUM+(room_info.TableArray.size()%TABLE_NUM>0?1:0);  //����
	int ViewHeight = RowNum*HEIGHT+(RowNum-1)*PADDING;  //�ܵĸ߶�
	if(ViewHeight > rect.Height())  //�й�����
	{
		//�������߶�
		m_VScrollBarHeight = client_rect.Height()*rect.Height()/(ViewHeight);
		//������λ��
		draw_rect.top += m_VScrollBarYOffset;
		draw_rect.bottom = draw_rect.top+m_VScrollBarHeight;
		draw_rect.left++;
		draw_rect.right--;
		CPen pen(PS_SOLID, 2, RGB(100,100,100));
		dc.SelectObject(&pen);
		dc.Rectangle(&draw_rect);
	}

	//������
	m_TableRectXOffset = (rect.Width()-(TABLE_NUM*WIDTH+(TABLE_NUM-1)*PADDING))/2;
	int y_offset = -1*(ViewHeight*m_VScrollBarYOffset/client_rect.Height());  //��ͼY��ƫԶλ��

	draw_rect = rect;
	draw_rect.left += m_TableRectXOffset;
	draw_rect.right = draw_rect.left+WIDTH;
	draw_rect.top = rect.top+y_offset;
	draw_rect.bottom = draw_rect.top+HEIGHT;

	//������
	dc.SelectStockObject(NULL_PEN);
	CBrush brush(RGB(255, 200, 0));
	dc.SelectObject(&brush);
	for(int i=0; i<room_info.TableArray.size(); ++i)
	{
		CRect player_rect;
		dc.BitBlt(draw_rect.left, draw_rect.top, draw_rect.Width(), draw_rect.Width(), &MemDc, 0, 0, SRCCOPY);

		CString lable;
		lable.Format(_T("%d"), i+1);
		dc.TextOut((draw_rect.left+draw_rect.right)/2,(draw_rect.top+draw_rect.bottom)/2, lable, lable.GetLength());

		int PlayerArray = room_info.TableArray[i];

		for(int j=0; j<room_info.NeedNum; ++j)
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

	//�����
	CPen pen(PS_SOLID, 3, RGB(158, 44, 104));
	CBrush gb_brush(&m_BgBmp2);
	CPen *old_pen = dc.SelectObject(&pen);
	CBrush *old_brush = dc.SelectObject(&gb_brush);
	dc.Rectangle(&client_rect);

	//���ָ���
	rect = client_rect;
	rect.left = rect.right-200;
	dc.MoveTo(rect.left, rect.top);
	dc.LineTo(rect.left, rect.bottom);
	dc.SelectStockObject(NULL_PEN);

	rect.right = rect.left-6;
	rect.left = client_rect.left+6;
	rect.top += 6;
	rect.bottom -= 6;

	//Բ
	CBrush brush(RGB(255, 200, 0));	
	dc.SelectObject(&brush);

	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];
	int i, index = -1;
	for(i=0; i<m_PlayerStatus.size(); ++i)
	{
		if(m_PlayerStatus[i].client_id == m_UID)
		{
			index = i;   //�Լ��������е�λ��
			break;
		}
	}
	int delta = 0;
	if(index != -1)
		delta = (room_info.NeedNum-1)-index;   // ����Լ���λ��˳ʱ���ƶ��ĸ���,�Ա���ʾ�������λ��
	int x, y;

	CString StatusStr[4]={_T("UnKnow"),_T("Audience"),_T("Wait"), _T("Playing")};

	for(i=0; i<m_PlayerStatus.size(); ++i)
	{
		int pos = (i+delta)%room_info.NeedNum;
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

		if(pos==3 && m_MyStatus>1)  //�Լ������
		{
			CBrush temp_brush(RGB(34,177,76));
			dc.SelectObject(&temp_brush);
			dc.Ellipse(&draw_rect);
			dc.SelectObject(&brush);
		}
		else
			dc.Ellipse(&draw_rect);

		if(pos == 0)
			x += 25;
		else if(pos == 1)
			y += 25;
		else if(pos == 2)
			x -= 50;
		else if(pos == 3)
			y -= 40;
		CString lable;

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
	}

	//���Թ���
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_LButtonDown = FALSE;
	CDialog::OnLButtonUp(nFlags, point);
}

void CTractorGameDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_LButtonDown == TRUE)
	{
		if(m_VScrollBarHeight > 0)  //�й�����
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if(m_CurStatus == Status_PrintTableList)
	{
		if(point.x>=m_TableRect.left+2&&point.x<m_TableRect.right-VSCROLL_WIDTH-2
			&&point.y>=m_TableRect.top+2&&point.y<m_TableRect.bottom-2)
		{
			//ת������table�е��߼�����
			int y_offset = m_VScrollBarYOffset*(m_TableRect.Height()-4)/m_VScrollBarHeight;
			int y = point.y-(m_TableRect.top+2)+y_offset;
			int x =  point.x-(m_TableRect.left+2)-m_TableRectXOffset;

			int row = y/(HEIGHT+PADDING) + (y%(HEIGHT+PADDING)>0?1:0);
			int column = x/(WIDTH+PADDING) + (x%(WIDTH+PADDING)>0?1:0);

			if(y<row*(HEIGHT+PADDING) -PADDING && x<column*(WIDTH+PADDING) -PADDING)   //���е�������(�����Ǽ��)
			{
				m_CurStatus = Status_Playing;
				m_SelectTableIndex = (row-1)*TABLE_NUM+(column-1);

				CString temp;
				temp.Format(_T("Into Table[%d]\r\n"), m_SelectTableIndex+1);
				AppendMsg(temp);

				//KillTimer(2);
				temp.Format(_T("->����[%02d]"), m_SelectRoomIndex+1);
				GetDlgItem(IDC_STATIC_ROOM)->SetWindowText(temp);
				GetDlgItem(IDC_STATIC_ROOM)->ShowWindow(SW_NORMAL);
				temp.Format(_T("->����[%02d]"), m_SelectTableIndex+1);
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(!m_RoomSocket.IsConnected || m_CurStatus!=Status_Playing)
		return ;
	if(m_CurStatus != Status_Playing)
		return ;
	if(!m_RoomSocket.IsConnected)
	{
		m_CurStatus = Status_PrintTableList;
		PrintTableList();
		return;
	}

	assert(m_SelectRoomIndex>=0 && m_SelectRoomIndex<m_RoomList.size());
	RoomInfo &room_info = m_RoomList[m_SelectRoomIndex];

	//Send AddGame request
	KVData kvdata(true);
	kvdata.SetValue(KEY_Protocol, PRO_StartGame);
	kvdata.SetValue(KEY_RoomID, room_info.RoomID);
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
