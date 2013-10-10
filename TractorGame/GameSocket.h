#pragma once

// CGameSocket 命令目标
#include "common/KVDataProtocolFactory.h"
using namespace easynet;

//成功返回0,失败返回错误码


class CGameSocket : public CAsyncSocket
{
public:
	uint32_t RecvProtocol(ProtocolContext &context);
public:
	CGameSocket():IsConnected(FALSE), m_SendContext(NULL){;}
	virtual ~CGameSocket(){}

	void OnReceive(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnClose(int nErrorCode);

	BOOL IsConnected;
	ProtocolContext *m_SendContext;
protected:
	virtual void OnReceiveProtocol(KVData *kvdata)=0;
};

class CInterfaceSocket: public CGameSocket
{
public:
	void OnConnect(int nErrorCode);
protected:
	void OnReceiveProtocol(KVData *kvdata);
};


class CRoomSocket: public CGameSocket
{
public:
	void OnConnect(int nErrorCode);
protected:
	void OnReceiveProtocol(KVData *kvdata);
};