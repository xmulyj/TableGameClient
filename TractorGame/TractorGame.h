
// TractorGame.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CTractorGameApp:
// �йش����ʵ�֣������ TractorGame.cpp
//

class CTractorGameApp : public CWinAppEx
{
public:
	CTractorGameApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CTractorGameApp theApp;