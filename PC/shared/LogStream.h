/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + HauteCapture - (c) 2005 by Volker Voecking Software Engineering
   +
   + $Source: C:\\CVS_Repository/CybonTailTNG/shared/LogStream.h,v $
   + $Author: volker $
   + $Date: 2005/01/15 12:50:28 $
   + $Revision: 1.6 $
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/**
 * $Source: C:\\CVS_Repository/CybonTailTNG/shared/LogStream.h,v $
 *
 * @author  Volker Voecking
 * @version $Revision: 1.6 $
 *
 */

#ifndef __LOGSTRM_H__
#define __LOGSTRM_H__

#pragma warning( push, 3 )
#include "fstream"
#include "XEventLog.h"

#ifdef _DEBUG
    #include "DbgLogStream.h"
#endif
#pragma warning( pop )
#include <afxpriv.h>

using namespace std;
#pragma warning( disable : 4786 )

extern CString     g_szLevel1Msg;
extern CXEventLog* g_pEventLog;

/**
 * Class for writing information to a log stream
 *
 * This class overrides the functionality of an OSTREAM by providing
 * a loglevel mechanism. The constructor of the class expects as argument
 * a file name of the logfile and an optional default logelevel. 
 * The loglevel determines the amount of output. By calling the method
 * SetActualLogLevelLimit(), the user can say that the output of the 
 * following extraction operators (<<) is only valid if the current
 * log-level is greater than or equal to the argument. This limit remains
 * valid until another call to SetActualLogLevelLimit. 
 * The function SetLogLevel() sets the current log-level
 *
 * @author  Volker Voecking
 *
 */
#define OSTREAM basic_ostream<char, char_traits<char> >

class CLogStream : public OSTREAM
{ 
public:
    CLogStream () : OSTREAM(&m_logBuf), m_usLogLevel(0), m_usCurLimit(1), m_lLogFileLimit(500000)
    {} 

    CLogStream (LPCTSTR logFile, unsigned short level = 0, long logFileLimit = 50000)
        : OSTREAM( &m_logBuf ), m_usLogLevel(level), m_usCurLimit(1), m_szLogFile(logFile), m_lLogFileLimit( logFileLimit )
    {
        CheckLogFile();
        init(&m_logBuf);
    }

    ~CLogStream();
    
    void open(LPTSTR logFile, unsigned short level, long logFileLimit, LPTSTR szAppName, LPTSTR szMsgDLLPath);
    
    int is_open() {
        return m_logBuf.is_open(); }
    
    void close() {
        m_logBuf.close(); 
        delete g_pEventLog; 
    }

    void LogFileVersion( LPTSTR pFilename );
    
    void SetLogLevel (unsigned short newLevel) { 
        m_usLogLevel = newLevel; }
    
    unsigned short GetLogLevel() const { 
        return m_usLogLevel; }
    
    void SetActualLogLevelLimit (unsigned short newLevel) { 
        m_usCurLimit = newLevel; }
    
    unsigned short SetActualLogLevelLimit() const { 
        return m_usCurLimit; }
    
    inline CLogStream& level(unsigned short newLogLimit)
    { 
        m_usCurLimit = newLogLimit;
        return *this;
    }                                     
    
    inline CLogStream& operator<<(CLogStream& (*_f)(CLogStream&))
    { 
        if (m_usCurLimit <= m_usLogLevel)
            (*_f)(*this);

        return *this;
    }
    
    inline CLogStream& operator<<(ios& (*_f)(ios&))
    { 
        if (m_usCurLimit <= m_usLogLevel)
            (*_f)(*this);
        
        return *this;
    }
    
    inline CLogStream& operator<<(const char* psz)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::write(psz,(std::streamsize)strlen(psz));
         
        if ( m_usCurLimit == 1 )
            g_szLevel1Msg += (char *)psz;
       
        return *this;
    }

    inline CLogStream& operator<<(const unsigned char *pusz)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::write((char*)pusz,(std::streamsize)strlen((char*)pusz));

        if ( m_usCurLimit == 1 )
            g_szLevel1Msg += (char *)pusz;
        
        return *this;
    }
    
    inline CLogStream& operator<<(const signed char *pssz)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::write((char*)pssz,(std::streamsize)strlen((char*)pssz));

        if ( m_usCurLimit == 1 )
            g_szLevel1Msg += (char *)pssz;
        
        return *this;
    }
    
    inline CLogStream& operator<<(char  rch)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::write(&rch,1);

        if ( m_usCurLimit == 1 )
            g_szLevel1Msg += rch;
        
        return *this;
    }
    
    inline CLogStream& operator<<(unsigned char ruch)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::write((char*)&ruch,1);
  
        if ( m_usCurLimit == 1 )
            g_szLevel1Msg += (char) ruch;

        return *this;
    }
    
    inline CLogStream& operator<<(signed char rch)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::write((char*)&rch,1);

        if ( m_usCurLimit == 1 )
            g_szLevel1Msg += (char) rch;

        return *this;
    }
    
    inline CLogStream& operator<<(const LPCWSTR psz)
        // note that wchar_t is a typedef to unsigned short
    { 
        try
        {
            USES_CONVERSION;
            char* pszA = W2A( psz );
            int nLen = (int) strlen( pszA );

            if (m_usCurLimit <= m_usLogLevel)
                OSTREAM::write(pszA, nLen);

            if ( m_usCurLimit == 1 )
                g_szLevel1Msg += CString( psz );
        }
        catch(...)
        {}

        return *this;
    }
    
    inline CLogStream& operator<<(unsigned short rch)
        // note that wchar_t is a typedef to unsigned short
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(rch);
        
        return *this;
    }
    
    inline CLogStream& operator<<(short  s)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(s);

        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%d"), s );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    
    inline CLogStream& operator<<(LONGLONG  n)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(n);

        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%lld"), n );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }

    inline CLogStream& operator<<(int  n)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(n);

        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%d"), n );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    
    inline CLogStream& operator<<(unsigned int  un)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(un);
      
        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%d"), un );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    
    inline CLogStream& operator<<(long  l)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(l);
        
        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%d"), l );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    
    inline CLogStream& operator<<(unsigned long  ul)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(ul);
 
        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%d"), ul );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    
    inline CLogStream& operator<<(float  f)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(f);

        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%f"), f );
            g_szLevel1Msg += szBuffer;
        }
        
        return *this;
    }
    
    inline CLogStream& operator<<(double  d)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(d);
 
        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%f"), d );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    
    inline CLogStream& operator<<(long double ld)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(ld);
        
        if ( m_usCurLimit == 1 )
        {
            wchar_t szBuffer[20];
            swprintf( szBuffer, 20, _T("%f"), ld );
            g_szLevel1Msg += szBuffer;
        }

        return *this;
    }
    inline CLogStream& operator<<(streambuf* psb)
    { 
        if (m_usCurLimit <= m_usLogLevel)
            OSTREAM::operator<<(psb);
        
        return *this;
    }

    // create or truncate the file if necessary
    void CheckLogFile();
    
  private:
      friend class CWinDebugStreamBuf;

      // members:
  public:
      unsigned short m_usLogLevel;
      unsigned short m_usCurLimit;
      long m_lLogFileLimit;
      CString m_szLogFile;

#ifdef _DEBUG
      CWinDebugStreamBuf m_logBuf;
#else 
      filebuf m_logBuf;
#endif
};

inline CLogStream& endl(CLogStream& _outs)
{ 
    if (_outs.m_usCurLimit <= _outs.m_usLogLevel)
        endl((OSTREAM&)_outs);

    return _outs;
}

inline CLogStream& flush(CLogStream& _outs)
{ 
    if (_outs.m_usCurLimit <= _outs.m_usLogLevel)
        _outs.m_logBuf.pubsync();
    
    if ( _outs.m_usCurLimit == 1 )
    {
        g_pEventLog->Write( EVENTLOG_ERROR_TYPE, g_szLevel1Msg );
        g_szLevel1Msg.Empty();
    }

    _outs.CheckLogFile();

    return _outs;
}

CLogStream& daytime(CLogStream& _outs);
inline CLogStream& header(CLogStream& _outs)
{ 
    if (_outs.m_usCurLimit <= _outs.m_usLogLevel)
        _outs << daytime;
//        _outs << module << _T(" ") << daytime;
    
    return _outs;
}

inline CLogStream& module(CLogStream& _outs)
{ 
    if (_outs.m_usCurLimit <= _outs.m_usLogLevel)
        _outs <<((AfxGetApp()->m_pszExeName));
    
    return _outs;
}

extern CLogStream clogstream;

#endif
