
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
	Status_PrintRoomList,
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

typedef struct _client_status_
{
	int   client_id;
	int	status;
}TableStatus;

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
	void AppendMsg(LPCTSTR msg);
public:
	CListCtrl m_RoomListCtrl;
	CImageList m_ImageList;

	CListCtrl m_TableListCtrl;

	CInterfaceSocket m_InterfaceSocket;
	CRoomSocket m_RoomSocket;

	ClientStatus m_CurStatus;
	
	//显示房间列表
	void PrintRoomList();
	bool GetRoomListReq();
	bool OnGetRoomListRsp(KVData *kvdata);
	bool GetRoomAddrReq();
	bool OnGetRoomAddrRsp(KVData *kvdata);

	//显示桌子列表
	void PrintTableList();
	bool IntoRoomReq();
	bool LeaveRoomReq();
	bool OnRoomInfoBroadCast(KVData *kvdata);
	
	//开始玩游戏
	void OnAddGame();
	void QuitGameReq();
	void OnTableInfoBroadCast(KVData *kvdata);

public:
	int m_UID;
	string m_UName;
	int m_SelectRoomIndex;
	int m_SelectTableIndex;

	vector<RoomInfo> m_RoomList;

	vector<TableStatus> m_PlayerStatus;
	vector<TableStatus> m_AudienceStatus;
	int m_MyStatus;   //0(无效),1(旁观者),2(等待开始),3(已经开始)

	afx_msg void OnBnClickedLoad();
	afx_msg void OnStaticListClick();
	afx_msg void OnStaticRoomClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkTablelist(NMHDR *pNMHDR, LRESULT *pResult);
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

	CBitmap m_Bitmap;
};
