#pragma once

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
};


