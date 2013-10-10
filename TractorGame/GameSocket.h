#pragma once

// CGameSocket ����Ŀ��
#include "common/KVDataProtocolFactory.h"
using namespace easynet;

//�ɹ�����0,ʧ�ܷ��ش�����


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