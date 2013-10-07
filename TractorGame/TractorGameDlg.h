
// TractorGameDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"

#include "GameSocket.h"
#include "RoomSocket.h"

typedef enum 
{
	Status_PrintRoomList,
}ClientStatus;

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
	CListCtrl m_RoomListCtrl;

	CGameSocket m_GameSocket;
	CGameSocket m_RoomSocket;

	ClientStatus m_CurStatus;
	void GetAllRoom();
	bool OnGetAllRoomRsp();
};
