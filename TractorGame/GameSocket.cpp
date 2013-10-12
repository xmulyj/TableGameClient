// GameSocket.cpp : 实现文件
//

#include "stdafx.h"
#include "TractorGame.h"
#include "GameSocket.h"

#include "KeyDefine.h"
#include "TractorGameDlg.h"

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
	ProtocolContext context;
	if(RecvProtocol(context) < 0)
	{
		IsConnected = FALSE;
		Close();
		CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
		CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

		CString msg(_T("OnReceive error. close socket now."));
		pDlg->AppendMsg(msg);
	}
	else
	{
		OnReceiveProtocol((KVData*)context.protocol);
		KVDataProtocolFactory factory;
		factory.DeleteProtocol(0, context.protocol);

		AsyncSelect(FD_READ|FD_CLOSE);
	}
}

void CGameSocket::OnSend(int nErrorCode)
{
	if(m_SendContext == NULL)
		return ;

	uint32_t send_size = 0;
	while(send_size < m_SendContext->Size)
	{
		int ret = Send(m_SendContext->Buffer, m_SendContext->Size-send_size, 0);
		if(ret >= 0)
			send_size += ret;
		else if(WSAGetLastError() != WSAEWOULDBLOCK)
		{
			IsConnected = FALSE;
			Close();

			CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
			CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

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
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;
	pDlg->AppendMsg(_T("server close socket.\r\n"));

	IsConnected = FALSE;
	Close();
}

// CInterfaceSocket
void CInterfaceSocket::OnConnect(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	if(nErrorCode == 0)
	{
		IsConnected = TRUE;
		pDlg->AppendMsg(_T("connect game interface successful.\r\n"));
		if(pDlg->m_CurStatus == Status_PrintRoomList)
			pDlg->PrintRoomList();
		else if(pDlg->m_CurStatus == Status_PrintTableList)
			pDlg->GetRoomAddrReq();
		else
			pDlg->AppendMsg(_T("connect game interface succssful, but in error status.\r\n"));
	}
	else
		pDlg->AppendMsg(_T("connect game interface failed.\r\n"));
}

void CInterfaceSocket::OnReceiveProtocol( KVData *kvdata)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	int Protocol;
	kvdata->GetValue(KEY_Protocol, Protocol);
	if(Protocol == PRO_GetRoomListRsp)
		pDlg->OnGetRoomListRsp(kvdata);
	else if(Protocol == PRO_GetRoomAddrRsp)
		pDlg->OnGetRoomAddrRsp(kvdata);
	else
		assert(0);
}


// CRoomSocket
void CRoomSocket::OnConnect(int nErrorCode)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	if(nErrorCode == 0)
	{
		IsConnected = TRUE;
		if(pDlg->m_CurStatus == Status_PrintTableList)
			pDlg->IntoRoomReq();
		else
			pDlg->AppendMsg(_T("connect game room succssful, but in error status.\r\n"));
	}
	else
		pDlg->AppendMsg(_T("connect game interface failed.\r\n"));
}

void CRoomSocket::OnReceiveProtocol(KVData *kvdata)
{
	CTractorGameApp* pApp= (CTractorGameApp*)AfxGetApp();
	CTractorGameDlg* pDlg= (CTractorGameDlg*)pApp->m_pMainWnd;

	int Protocol;
	kvdata->GetValue(KEY_Protocol, Protocol);
	if(Protocol == PRO_RoomInfoBroadCast)
		pDlg->OnRoomInfoBroadCast(kvdata);
	else if(Protocol == PRO_TableInfoBroadCast)
		pDlg->OnTableInfoBroadCast(kvdata);
	else if(Protocol == PRO_DealPoker)
		pDlg->OnDealPoker(kvdata);
	else
		assert(0);
}