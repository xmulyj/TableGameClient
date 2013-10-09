
// TractorGameDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "GameSocket.h"
#include "RoomSocket.h"

#include <string>
#include <vector>
using namespace std;

typedef enum 
{
	Status_PrintRoomList,
	Status_PrintTableList,
}ClientStatus;

typedef struct _room_info
{
	int         RoomID;
	int         ClientNum;
	string      IP;
	int         Port;

	vector<int> TableArray;  //每桌玩家数量
}RoomInfo;

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
	CListCtrl m_TableListCtrl;

	CGameSocket m_GameSocket;
	CRoomSocket m_RoomSocket;

	ClientStatus m_CurStatus;
	
	void PrintRoomList();
	bool OnInterfaceRsp();

	bool GetAllRoomReq();
	bool OnGetAllRoomRsp(KVData *kvdata);
	bool GetRoomAddrReq();
	bool OnGetRoomAddrRsp(KVData *kvdata);

	void PrintTableList();
	void OnRoomRsp();

	bool GetRoomInfoReq();
	bool OnGetRoomInfoRsp(KVData *kvdata);
public:
	int m_UID;
	string m_UName;
	int m_SelectRoomIndex;

	vector<RoomInfo> m_RoomList;

	afx_msg void OnBnClickedLoad();
	afx_msg void OnStaticListClick();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
};
