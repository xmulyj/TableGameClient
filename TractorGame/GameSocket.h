#pragma once

// CGameSocket ÃüÁîÄ¿±ê
#include "common/KVDataProtocolFactory.h"
using namespace easynet;

class CGameSocket : public CAsyncSocket
{
public:
	CGameSocket();
	virtual ~CGameSocket();

	void OnReceive(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnOutOfBandData(int nErrorCode);
	void OnAccept(int nErrorCode);
	void OnConnect(int nErrorCode);
	void OnClose(int nErrorCode);

	BOOL IsConnected;

	ProtocolContext *m_SendContext;
};


