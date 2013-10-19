
// TractorGameDlg.h : 头文件
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

	vector<int> TableArray;  //每桌玩家数量
}RoomInfo;

typedef struct __player__
{
	int   client_id;
	string client_name;
	int   index;
	int   status;
	vector<int> poker;
}Player;


// CTractorGameDlg 对话框
class CTractorGameDlg : public CDialog
{
// 构造
public:
	CTractorGameDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_TRACTORGAME_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CGameSocket m_InterfaceSocket;  //与interface链接的socket
	CGameSocket m_RoomSocket;      //与gameroom链接的socket

	int m_UID;                                     //用户id
	string m_UName;                          //用户名

	int m_SelectRoomIndex;                //选择的房间号
	int m_SelectTableIndex;                 //选择的桌子号
	vector<RoomInfo> m_RoomList;   //房间列表
	RoomInfo m_SelectRoom;             //选择的房间

	int m_MyStatus;                            //0(无效),1(旁观者),2(等待开始),3(已经开始)
	vector<Player> m_Player;             //玩家
	vector<Player> m_Audience;        //观众
public:
	void AppendMsg(LPCTSTR msg);
	void SendProtocol(ProtocolContext *context, CGameSocket *gamesocket);
	//显示房间列表
	bool GetRoomListReq();
	bool GetRoomAddrReq();
	bool IntoRoomReq();
	bool LeaveRoomReq();
	//开始游戏
	void AddGameReq();
	void QuitGameReq();
	void StartGameReq();
	//接收协议
	void OnReceiveProtocol(KVData *kvdata);
	bool OnGetRoomListRsp(KVData *kvdata);
	bool OnGetRoomAddrRsp(KVData *kvdata);
	bool OnRoomInfoBroadCast(KVData *kvdata);
	void OnDealPoker(KVData *kvdata);
	void OnAddGameRsp(KVData *kvdata);
	void OnAddGameBroadCast(KVData *kvdata);
	void OnStartGameBroadCast(KVData *kvdata);
	void OnQuitGameBroadCast(KVData *kvdata);

//绘图相关
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
	CBitmap m_HBackBmp;  //扑克背面(横)
	CBitmap m_VBackBmp;  //扑克背面(竖)

	ClientStatus m_CurStatus;
	void OnPaint_TableList(CRect &client_rect);
	void OnPaint_Talbe(CRect &client_rect);
	void DrawPoker(CDC *dc, CRect &rect, CBitmap *poker_bitmap);
//对话框事件
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
