// lunasol.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "lunasol.h"
#include "LunaSolDlg.h"

#include "customer.h"
#include "LogStream.h"
#include "RegAccess.h"
#include "ProductVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// global object for logging
CLogStream clogstream;

// global object for reading configuration information 
CRegAccess g_Config;
// CLunaSolApp

BEGIN_MESSAGE_MAP(CLunaSolApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CLunaSolApp construction

CLunaSolApp::CLunaSolApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CLunaSolApp object

CLunaSolApp theApp;


// CLunaSolApp initialization

BOOL CLunaSolApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

    // initialize logging
    CString szTempPath;
    if ( 0 == GetTempPath( MAX_PATH, szTempPath.GetBuffer(MAX_PATH)))
        szTempPath = _T("");  // use the current directory
    else
        szTempPath.ReleaseBuffer();

    CString szLogFileName = g_Config.GetStringValue( KEY_LUNASOLCAL_LOGFILENAME );
    if ( szLogFileName.IsEmpty() )
        szLogFileName = _T("Tail.log");

    szLogFileName = szTempPath + szLogFileName;
    const DWORD dwLogLevel = g_Config.GetDWORDValue( KEY_LUNASOLCAL_LOGLEVEL );
    const DWORD dwMaxLogFileSize = g_Config.GetDWORDValue( KEY_LUNASOLCAL_MAXLOGFILESIZE );

    m_szAppName = CUSTOM_APPLICATION_NAME;
    clogstream.open( szLogFileName.GetBuffer(256), (unsigned short) dwLogLevel, (long) dwMaxLogFileSize, NULL, NULL );
    clogstream.level(3) << _T("-----------------------------------------------------------------------------") << endl;
    clogstream.level(3) << header << m_szAppName << _T(" was started") << endl;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey( m_szAppName );

	CLunaSolDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CLunaSolApp::ExitInstance() 
{
    clogstream.level(3) << header << _T("ExitInstance") << endl;
    clogstream.close();

	return CWinApp::ExitInstance();
}

CString CLunaSolApp::GetVersionString() const
{
    DWORD dwVersionNumber = 0;
    DWORD dwBuildNumber = 0;
    ProductVersion versionInfo;
    versionInfo.GetFileVersionNumber( dwVersionNumber, dwBuildNumber );

    CString szVersioNumber;
    wsprintf( szVersioNumber.GetBuffer(128), _T("%ld.%ld.%ld.%04ld"),
        dwVersionNumber / 0x10000, dwVersionNumber % 0x10000,
        dwBuildNumber   / 0x10000, dwBuildNumber   % 0x10000 );
    szVersioNumber.ReleaseBuffer();

    return szVersioNumber;
}

