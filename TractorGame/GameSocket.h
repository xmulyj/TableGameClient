#pragma once

// CGameSocket 命令目标
#include "common/KVDataProtocolFactory.h"
using namespace easynet;

//成功返回0,失败返回错误码
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
