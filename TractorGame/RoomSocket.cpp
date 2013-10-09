// RoomSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "TractorGame.h"
#include "RoomSocket.h"

#include "TractorGameDlg.h"

// CRoomSocket

CRoomSocket::CRoomSocket():IsConnected(FALSE),m_SendContext(NULL)
{
}

CRoomSocket::~CRoomSocket()
{
}


// CRoomSocket 成员函数
void CRoomSocket::OnReceive(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	pDlg->OnRoomRsp();

	CAsyncSocket::OnReceive(nErrorCode);
}

void CRoomSocket::OnSend(int nErrorCode)
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

void CRoomSocket::OnOutOfBandData(int nErrorCode)
{

}

void CRoomSocket::OnAccept(int nErrorCode)
{

}

void CRoomSocket::OnConnect(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	pDlg->AppendMsg(_T("OnConnect RoomSocket"));
	if(nErrorCode == 0)
	{
		IsConnected = TRUE;

		pDlg->AppendMsg(_T("connect game room successful.\r\n"));
		if(pDlg->m_CurStatus == Status_PrintTableList)
			pDlg->GetRoomInfoReq();
		else
			assert(0);
	}
	else
	{
		pDlg->AppendMsg(_T("connect game room failed.\r\n"));
	}

	//CAsyncSocket::OnConnect(nErrorCode);
}

void CRoomSocket::OnClose(int nErrorCode)
{
	IsConnected = FALSE;
}