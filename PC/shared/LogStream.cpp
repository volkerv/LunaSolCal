/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include "DbgLogStream.h"
#include "LogStream.h"
#include "XEventLog.h"

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#pragma warning( disable : 4710 )
#include <string.h>

const int CWinDebugStreamBuf::cNoFile = -1;

CString     g_szLevel1Msg;
CXEventLog* g_pEventLog;

// ======================================================================
// class CLogStream
// ======================================================================
CLogStream::~CLogStream()
{
}

void CLogStream::open(LPTSTR logFile, unsigned short level, long logFileLimit, LPTSTR szAppName, LPTSTR szMsgDLLPath)
{
    USES_CONVERSION;

    g_pEventLog = new CXEventLog();
    g_pEventLog->Init( szAppName, szMsgDLLPath );

    m_szLogFile = logFile;
    m_lLogFileLimit = logFileLimit;

    m_usLogLevel = level;
    
    if (m_logBuf.is_open())
        m_logBuf.close();

    CheckLogFile();

    try
    {
        m_logBuf.open( T2A(m_szLogFile.GetBuffer(MAX_PATH)), out | app | in); //, filebuf::sh_read | filebuf::sh_write);
    }
    catch(...)
    {}
}


// Create or truncate the file if necessary
void CLogStream::CheckLogFile()
{    
    USES_CONVERSION;

    CFileStatus stat;
    if (CFile::GetStatus( m_szLogFile, stat))
    {
        if (stat.m_size > m_lLogFileLimit)
        { 
            if (m_logBuf.is_open())
                m_logBuf.close();

            // rename log file
            CString szNewFileName = m_szLogFile + _T("_1");
            
            DeleteFile( szNewFileName );
            MoveFile( m_szLogFile, szNewFileName );
            TRACE0("file limit is exceeded, log-file is erased\n");
            //            test.open((char*)path, out | trunc ); //, filebuf::sh_read | filebuf::sh_write);
            //            test.close();

            try
            {
                m_logBuf.open( T2A(m_szLogFile.GetBuffer(MAX_PATH)), out | app | in); //, filebuf::sh_read | filebuf::sh_write);
            }
            catch(...)
            {}
        }
    }
}

// ----------------------------------------------------------------------

void CLogStream::LogFileVersion( LPTSTR pFilename )
{
    DWORD dwHandle = 0;
    DWORD dwLen = GetFileVersionInfoSize( pFilename, &dwHandle );
    if ( dwLen > 0 )
    {
        char* pBuffer = new char[dwLen];
        
        if ( pBuffer && GetFileVersionInfo( pFilename, NULL, dwLen, pBuffer) )
        {
            VS_FIXEDFILEINFO* pFileInfo = NULL;
            UINT len;
            
            if ( VerQueryValue( pBuffer, _T("\\"), (void**)&pFileInfo, &len) )
            {
                level(3) << header << pFilename << ": [" << (DWORD) HIWORD( pFileInfo->dwProductVersionMS )
                    << "." << (DWORD) LOWORD( pFileInfo->dwProductVersionMS )
                    << "." << (DWORD) HIWORD( pFileInfo->dwProductVersionLS )
                    << "." << (DWORD) LOWORD( pFileInfo->dwProductVersionLS )
                    << "]" << endl;
            }
            
            delete[] pBuffer;
        }
    }
}

// ----------------------------------------------------------------------

CLogStream& daytime(CLogStream& _outs)
{ 
    if (_outs.m_usCurLimit <= _outs.m_usLogLevel)
    {   
        struct _timeb tstruct;
        _ftime(&tstruct);
        wchar_t szMilliSecs[5];
        swprintf( szMilliSecs, 5, _T("%.3u"), tstruct.millitm );

        SYSTEMTIME curTime;
        GetLocalTime( &curTime );
        
        wchar_t szTime[20];
        wchar_t szDate[50];
        wchar_t szCurTime[100];
        GetTimeFormat(LOCALE_SYSTEM_DEFAULT, 0, &curTime, NULL, szTime, 20);
        GetDateFormat(LOCALE_SYSTEM_DEFAULT, DATE_SHORTDATE, &curTime, NULL, szDate, 50);
        _tcscpy( szCurTime, szDate );
        _tcscat( szCurTime, _T(" "));
        _tcscat( szCurTime, szTime);
        _tcscat( szCurTime, _T(" "));
        _tcscat( szCurTime, szMilliSecs);

#ifdef _UNICODE
//        CW2A pszCurTimeA( szCurTime );
USES_CONVERSION;
		CString pszCurTimeA;
        try
        {
		    pszCurTimeA = W2A( szCurTime );
        }
        catch(...)
        {}
        _outs << pszCurTimeA;
#else
        _outs << szCurTime;
#endif
        _outs << _T(" : ");
    }
    return _outs;
}



// ======================================================================
// class CLogStream
// ======================================================================

CWinDebugStreamBuf::CWinDebugStreamBuf()
{ 
    streambuf* p;
    p = setbuf((char*)buf, 254);
    setp((char*)buf, (char*)(buf+254));
}

// ----------------------------------------------------------------------

CWinDebugStreamBuf::~CWinDebugStreamBuf()
{ 
    if (f.is_open())
        f.close();
}

// ----------------------------------------------------------------------

BOOL CWinDebugStreamBuf::open (LPSTR  fileName, int nMode ) //, int nProt)
{ 
    USES_CONVERSION;

    if (f.is_open())
        f.close();

    ASSERT(fileName);
    f.open( fileName, nMode ); //, nProt);

    return f.is_open();
}

// ----------------------------------------------------------------------

void CWinDebugStreamBuf::close ()
{ 
    if (!f.is_open())
        f.close();
}

// ----------------------------------------------------------------------

int CWinDebugStreamBuf::is_open () const
{   
    return f.is_open();
}

// ----------------------------------------------------------------------

int CWinDebugStreamBuf::overflow (int nCh)
{ 
    if (nCh != EOF)
        *(pptr())=(char) nCh;

    WriteOutput((int)(pptr() - pbase() + 1));
    *(pptr()+1)='\0';
    DebugOutput();
    setp(pbase(), epptr());
    
    return 0;
}

// ----------------------------------------------------------------------

int CWinDebugStreamBuf::underflow ()
{ // only output is supported
    return EOF;
}

// ----------------------------------------------------------------------

int CWinDebugStreamBuf::sync()
{
    WriteOutput((int)(pptr() - pbase())); 
    *(pptr())='\0';    
    DebugOutput();
    setp(pbase(), epptr());
    return 0;
}

// ----------------------------------------------------------------------

void CWinDebugStreamBuf::WriteOutput(int len)
{
    if (f.is_open())
    { 
        f.write(pbase(), len);
        f.flush();
    }
}

// ----------------------------------------------------------------------

void CWinDebugStreamBuf::DebugOutput()
{
    try
    {
        USES_CONVERSION;
        /*
        int len = pptr()-pbase();
        if (pptr()-pbase() > 255)
        {   // limit output to 200 characters
        strcpy(pbase()+250, "...\n");
        }
        */
        ::OutputDebugString(A2T(pbase()));
    }
    catch(...)
    {
    }
}

// ----------------------------------------------------------------------

wchar_t CWinDebugStreamBuf::buf[];

// ===========================================================================
// end of ixxlogst.cpp
// ===========================================================================
