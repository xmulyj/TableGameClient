
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

typedef struct __player__
{
	int   client_id;
	string client_name;
	int   index;
	int   status;
	vector<int> poker;
}Player;


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
	CGameSocket m_InterfaceSocket;  //��interface���ӵ�socket
	CGameSocket m_RoomSocket;      //��gameroom���ӵ�socket

	int m_UID;                                     //�û�id
	string m_UName;                          //�û���

	int m_SelectRoomIndex;                //ѡ��ķ����
	int m_SelectTableIndex;                 //ѡ������Ӻ�
	vector<RoomInfo> m_RoomList;   //�����б�
	RoomInfo m_SelectRoom;             //ѡ��ķ���

	int m_MyStatus;                            //0(��Ч),1(�Թ���),2(�ȴ���ʼ),3(�Ѿ���ʼ)
	vector<Player> m_Player;             //���
	vector<Player> m_Audience;        //����
public:
	void AppendMsg(LPCTSTR msg);
	void SendProtocol(ProtocolContext *context, CGameSocket *gamesocket);
	//��ʾ�����б�
	bool GetRoomListReq();
	bool GetRoomAddrReq();
	bool IntoRoomReq();
	bool LeaveRoomReq();
	//��ʼ��Ϸ
	void AddGameReq();
	void QuitGameReq();
	void StartGameReq();
	//����Э��
	void OnReceiveProtocol(KVData *kvdata);
	bool OnGetRoomListRsp(KVData *kvdata);
	bool OnGetRoomAddrRsp(KVData *kvdata);
	bool OnRoomInfoBroadCast(KVData *kvdata);
	void OnDealPoker(KVData *kvdata);
	void OnAddGameRsp(KVData *kvdata);
	void OnAddGameBroadCast(KVData *kvdata);
	void OnStartGameBroadCast(KVData *kvdata);
	void OnQuitGameBroadCast(KVData *kvdata);

//��ͼ���
public:
	BOOL m_LButtonDown;
	CPoint  m_PreMousePoint;
	int m_VScrollBarHeight;
	int m_VScrollBarYOffset;
	int m_TableRectXOffset;
	CRect m_TableRect;

	CBitmap m_BgBmp;
	CBitmap m_BgBmp2;
	CBitmap m_TableBmp;
	//Poker Bitmap;
	CBitmap m_PokerBmp[54];
	CBitmap m_BgMaskBmp;
	CBitmap m_FtMaskBmp;
	CBitmap m_HBackBmp;  //�˿˱���(��)
	CBitmap m_VBackBmp;  //�˿˱���(��)

	ClientStatus m_CurStatus;
	void OnPaint_TableList(CRect &client_rect);
	void OnPaint_Talbe(CRect &client_rect);
	void DrawPoker(CDC *dc, CRect &rect, CBitmap *poker_bitmap);
//�Ի����¼�
public:
	CListCtrl m_RoomListCtrl;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedLoad();
	afx_msg void OnStaticRoomClick();
	afx_msg void OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedStartgame();
	afx_msg void OnBnClickedAddgame();
};
