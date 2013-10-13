#pragma once

// CGameSocket ����Ŀ��
#include "common/KVDataProtocolFactory.h"
using namespace easynet;

//�ɹ�����0,ʧ�ܷ��ش�����
#include <string>
using std::string;

class CGameSocket : public CAsyncSocket
{
public:
	uint32_t RecvProtocol(ProtocolContext &context);
public:
	CGameSocket():IsConnected(FALSE), m_SendContext(NULL){;}
	virtual ~CGameSocket(){}

	void OnConnect(int nErrorCode);
	void OnReceive(int nErrorCode);
	void OnSend(int nErrorCode);
	void OnClose(int nErrorCode);

	BOOL IsConnected;
	ProtocolContext *m_SendContext;

	CString IP;
	int      Port;
};
