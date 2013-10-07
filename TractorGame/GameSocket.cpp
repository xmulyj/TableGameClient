// GameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "TractorGame.h"
#include "GameSocket.h"

#include "TractorGameDlg.h"

// CGameSocket

CGameSocket::CGameSocket():IsConnected(FALSE)
{
}

CGameSocket::~CGameSocket()
{
}


// CGameSocket 成员函数
void CGameSocket::OnReceive(int nErrorCode)
{

}
void CGameSocket::OnSend(int nErrorCode)
{

}
void CGameSocket::OnOutOfBandData(int nErrorCode)
{

}
void CGameSocket::OnAccept(int nErrorCode)
{

}
void CGameSocket::OnConnect(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	if(nErrorCode == 0)
	{
		IsConnected = TRUE;
		pDlg->GetAllRoom();
	}
	else
	{
		CRichEditCtrl* msg_ctrl = (CRichEditCtrl*)pDlg->GetDlgItem(IDC_MSG);
		CString str;
		msg_ctrl->GetWindowText(str);
		str += _T("connect to Interface failed.");
		msg_ctrl->SetWindowText(str);
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CGameSocket::OnClose(int nErrorCode)
{

}