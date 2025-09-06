// lunasol.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CLunaSolApp:
// See lunasol.cpp for the implementation of this class
//

class CLunaSolApp : public CWinApp
{
public:
	CLunaSolApp();

// Overrides
	public:
	virtual BOOL InitInstance();
    virtual BOOL ExitInstance();

// Implementation
    CString GetVersionString() const;

	DECLARE_MESSAGE_MAP()


private:
    CString                 m_szAppName;
};

extern CLunaSolApp theApp;