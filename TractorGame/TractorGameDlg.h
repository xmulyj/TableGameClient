
// TractorGameDlg.h : ͷ�ļ�
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

	int         fd;  //����
	vector<int> TableArray;  //ÿ���������

}RoomInfo;

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
public:
	CListCtrl m_RoomListCtrl;
	CListCtrl m_TableListCtrl;

	CGameSocket m_GameSocket;
	CGameSocket m_RoomSocket;

	ClientStatus m_CurStatus;
	
	void PrintRoomList();
	bool GetAllRoomReq();
	bool OnGetAllRoomRsp();

	void PrintTableList();
	bool GetRoomInfoReq();
	bool OnGetRoomInfoRsp();
public:
	int m_UID;
	string m_UName;
	int m_SelectRoomIndex;

	vector<RoomInfo> m_RoomList;

	afx_msg void OnBnClickedLoad();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnLvnItemchangedRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
};
