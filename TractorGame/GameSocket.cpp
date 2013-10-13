// GameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "TractorGame.h"
#include "GameSocket.h"

#include "KeyDefine.h"
#include "TractorGameDlg.h"

void CGameSocket::OnConnect(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	if(nErrorCode == 0)
	{
		pDlg->AppendMsg(_T("connect to server successfu.\r\n"));
		IsConnected = TRUE;
		AsyncSelect(FD_WRITE|FD_READ|FD_CLOSE);
	}
	else
	{
		pDlg->AppendMsg(_T("connect to server failed.\r\n"));
		Close();
	}
}

// CGameSocket
uint32_t CGameSocket::RecvProtocol(ProtocolContext &context)
{
	KVDataProtocolFactory factory;
	context.header_size = factory.HeaderSize();
	context.CheckSize(context.header_size);
	while(context.Size < context.header_size)
	{
		int recv_size = Receive(context.Buffer+context.Size, context.header_size-context.Size);
		if(recv_size > 0)
			context.Size += recv_size;
		else
			return -1;
	}

	if(DECODE_SUCC != factory.DecodeHeader(context.Buffer, context.type, context.body_size))
	{
		return -2;
	}

	context.CheckSize(context.body_size);
	while(context.Size < context.header_size+context.body_size)
	{
		int recv_size = Receive(context.Buffer+context.Size, context.header_size+context.body_size-context.Size);
		if(recv_size > 0)
			context.Size += recv_size;
		else
			return -1;
	}

	if(DECODE_SUCC != factory.DecodeBinBody(&context))
		return -3;
	return 0;
}

void CGameSocket::OnReceive(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	ProtocolContext context;
	if(RecvProtocol(context) < 0)
	{
		Close();
		IsConnected = FALSE;
		
		CString msg(_T("OnReceive error. close socket now."));
		pDlg->AppendMsg(msg);
	}
	else
	{
		pDlg->OnReceiveProtocol((KVData*)context.protocol);
		KVDataProtocolFactory factory;
		factory.DeleteProtocol(0, context.protocol);
		AsyncSelect(FD_READ|FD_CLOSE);
	}
}

void CGameSocket::OnSend(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	if(m_SendContext == NULL)
	{
		pDlg->AppendMsg(_T("Socket OnSend, but context is NULL.\r\n"));
		return ;
	}

	CString msg(m_SendContext->Info.c_str());
	msg += ": OnSend\r\n";
	pDlg->AppendMsg(msg);

	uint32_t send_size = m_SendContext->send_size;
	while(send_size < m_SendContext->Size)
	{
		int ret = Send(m_SendContext->Buffer, m_SendContext->Size-send_size, 0);
		if(ret >= 0)
			send_size += ret;
		else if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			Close();
			IsConnected = FALSE;

			CString msg(m_SendContext->Info.c_str());
			msg += _T(":OnSend error. close socket now");
			pDlg->AppendMsg(msg);
			break;
		}
	}
	delete m_SendContext;
	m_SendContext = NULL;

	if(IsConnected)
		AsyncSelect(FD_READ|FD_CLOSE);
}

void CGameSocket::OnClose(int nErrorCode)
{
	Close();
	IsConnected = FALSE;

	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;
	pDlg->AppendMsg(_T("server close socket.\r\n"));
}
