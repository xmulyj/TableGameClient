#pragma once

#include "common/KVDataProtocolFactory.h"
using namespace easynet;
// CRoomSocket ÃüÁîÄ¿±ê

class CRoomSocket : public CAsyncSocket
{
public:
	CRoomSocket();
	virtual ~CRoomSocket();

	void OnReceive(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnOutOfBandData(int nErrorCode);
	void OnAccept(int nErrorCode);
	void OnConnect(int nErrorCode);
	void OnClose(int nErrorCode);

	BOOL IsConnected;
	ProtocolContext *m_SendContext;
};


