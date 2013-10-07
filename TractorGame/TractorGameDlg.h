
// TractorGameDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"

#include "GameSocket.h"
#include "RoomSocket.h"

typedef enum 
{
	Status_PrintRoomList,
}ClientStatus;

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
	CListCtrl m_RoomListCtrl;

	CGameSocket m_GameSocket;
	CGameSocket m_RoomSocket;

	ClientStatus m_CurStatus;
	void GetAllRoom();
	bool OnGetAllRoomRsp();
};
