
// TractorGameDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "GameSocket.h"

#include <string>
#include <vector>
using namespace std;

typedef enum
{
	Interface_GetRoomList,
	Interface_GetRoomAddr,
}InterfaceStatus;

typedef enum 
{
	Status_PrintTableList,
	Status_Playing,
}ClientStatus;

typedef struct _room_info
{
	int         RoomID;
	int         ClientNum;
	int         NeedNum;
	string    IP;
	int         Port;

	vector<int> TableArray;  //ÿ���������
}RoomInfo;

typedef struct _client_status_
{
	int   client_id;
	int	status;
	vector<int> poker;
}TableStatus;


// CTractorGameDlg �Ի���
class CTractorGameDlg : public CDialog
{
// ����
public:
	CTractorGameDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TRACTORGAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void AppendMsg(LPCTSTR msg);
	void SendProtocol(ProtocolContext *context, CGameSocket *gamesocket);
public:
	CListCtrl m_RoomListCtrl;

	CGameSocket m_InterfaceSocket;
	CGameSocket m_RoomSocket;

	ClientStatus m_CurStatus;
	
	void OnReceiveProtocol(KVData *kvdata);
	//��ʾ�����б�
	bool GetRoomListReq();
	bool OnGetRoomListRsp(KVData *kvdata);
	bool GetRoomAddrReq();
	bool OnGetRoomAddrRsp(KVData *kvdata);

	//��ʾ�����б�
	bool IntoRoomReq();
	bool LeaveRoomReq();
	bool OnRoomInfoBroadCast(KVData *kvdata);
	
	//��ʼ����Ϸ
	void OnAddGame();
	void QuitGameReq();
	void OnDealPoker(KVData *kvdata);
	void OnTableInfoBroadCast(KVData *kvdata);

public:
	int m_UID;
	string m_UName;

	int m_SelectRoomIndex;
	int m_SelectTableIndex;

	vector<RoomInfo> m_RoomList;
	RoomInfo m_SelectRoom;

	vector<TableStatus> m_PlayerStatus;
	vector<TableStatus> m_AudienceStatus;
	int m_MyStatus;   //0(��Ч),1(�Թ���),2(�ȴ���ʼ),3(�Ѿ���ʼ)

	afx_msg void OnBnClickedLoad();
	afx_msg void OnStaticRoomClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
public:
	BOOL m_LButtonDown;
	CPoint  m_PreMousePoint;

	int m_VScrollBarHeight;
	int m_VScrollBarYOffset;
	int m_TableRectXOffset;
	CRect m_TableRect;
	void OnPaint_TableList(CRect &client_rect);
	void OnPaint_Talbe(CRect &client_rect);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);

	CBitmap m_BgBmp;
	CBitmap m_BgBmp2;
	CBitmap m_TableBmp;
	afx_msg void OnBnClickedStartgame();
	afx_msg void OnBnClickedAddgame();

	//Poker Bitmap;
	CBitmap m_PokerBmp[54];
	CBitmap m_BgMaskBmp;
	CBitmap m_FtMaskBmp;
	CBitmap m_HBackBmp;  //�˿˱���(��)
	CBitmap m_VBackBmp;  //�˿˱���(��)

	void DrawPoker(CDC *dc, CRect &rect, CBitmap *poker_bitmap);
};
