// GameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "TractorGame.h"
#include "GameSocket.h"

#include "TractorGameDlg.h"

// CGameSocket

CGameSocket::CGameSocket():IsConnected(FALSE),m_SendContext(NULL)
{
}

CGameSocket::~CGameSocket()
{
}


// CGameSocket 成员函数
void CGameSocket::OnReceive(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	pDlg->OnInterfaceRsp();

	CAsyncSocket::OnReceive(nErrorCode);
}

void CGameSocket::OnSend(int nErrorCode)
{
	if(m_SendContext == NULL)
		return ;

	uint32_t send_size = 0;
	while(send_size < m_SendContext->Size)
	{
		int ret = Send(m_SendContext->Buffer, m_SendContext->Size-send_size, 0);
		if(ret != SOCKET_ERROR)
			send_size += ret;
	}

	delete m_SendContext;
	m_SendContext = NULL;

	AsyncSelect(FD_READ);
	CAsyncSocket::OnSend(nErrorCode);
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

		pDlg->AppendMsg(_T("connect game_interface successful.\r\n"));
		pDlg->PrintRoomList();
	}
	else
	{
		pDlg->AppendMsg(_T("connect game_interface failed.\r\n"));
	}

	CAsyncSocket::OnConnect(nErrorCode);
}

void CGameSocket::OnClose(int nErrorCode)
{
	IsConnected = FALSE;
}