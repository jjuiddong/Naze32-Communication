
// Naze32UDP.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CNaze32UDPApp:
// See Naze32UDP.cpp for the implementation of this class
//

class CNaze32UDPApp : public CWinApp
{
public:
	CNaze32UDPApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CNaze32UDPApp theApp;
