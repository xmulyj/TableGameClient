#pragma once

// CGameSocket ÃüÁîÄ¿±ê

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
};


