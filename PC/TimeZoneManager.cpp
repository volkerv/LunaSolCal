/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "StdAfx.h"
#include "TimeZoneManager.h"
#include "RegAccess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SUBKEY_TIME_ZONES L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones"

extern CRegAccess g_Config;

TimeZoneManager::TimeZoneManager( )
{
}

TimeZoneManager::~TimeZoneManager( )
{
    POSITION pos = m_TimeZones.GetHeadPosition( );

    while ( pos != NULL )
    {
        TimeZoneDescription *c = (TimeZoneDescription *) m_TimeZones.GetNext( pos );
        delete c;
    }

    m_TimeZones.RemoveAll( );
}

bool TimeZoneManager::Initialize( )
{
    bool success = false;

    HKEY hOpenKey = 0;

	if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, SUBKEY_TIME_ZONES, 0, KEY_READ, &hOpenKey) == ERROR_SUCCESS)
	{				
        DWORD dwIndex = 0;
        FILETIME LastWriteTime;
        LONG lRes = 0;

        DWORD dwClassBufLen = MAX_PATH;  // number of characters !!!
        TCHAR* pKeyClass = new TCHAR[dwClassBufLen];
        memset( pKeyClass, 0, dwClassBufLen*sizeof(TCHAR));

        DWORD dwNameBufLen = MAX_PATH;   // number of characters !!!
        TCHAR* pKeyName = new TCHAR[dwNameBufLen];
        memset( pKeyName, 0, dwNameBufLen*sizeof(TCHAR));

        do
        {
            dwClassBufLen = MAX_PATH;
            dwNameBufLen = MAX_PATH;

            lRes = RegEnumKeyEx( hOpenKey, dwIndex, pKeyName, &dwNameBufLen, NULL, pKeyClass, &dwClassBufLen, &LastWriteTime );
            if ( lRes == ERROR_SUCCESS )
            {
                TimeZoneDescription *descr = new TimeZoneDescription;
                if ( descr )
                {
                    CString szTimeZoneKey = CString( SUBKEY_TIME_ZONES ) + L"\\" + CString( pKeyName );
                    g_Config.GetStringFromRegistry( HKEY_LOCAL_MACHINE, szTimeZoneKey, L"Display", descr->szDisplay );
                    g_Config.GetStringFromRegistry( HKEY_LOCAL_MACHINE, szTimeZoneKey, L"Dlt", descr->szDaylight );
                    g_Config.GetStringFromRegistry( HKEY_LOCAL_MACHINE, szTimeZoneKey, L"Std", descr->szStandard );
                    
                    ULONG n = sizeof( TimeZoneInfo );
                    TimeZoneInfo *tzi = 0;
                    g_Config.GetBinaryValueFromRegistry( HKEY_LOCAL_MACHINE, szTimeZoneKey, L"TZI", &n, (BYTE **)&tzi );
                    descr->tzInfo.Bias = tzi->Bias;
                    descr->tzInfo.DaylightBias = tzi->DaylightBias;
                    descr->tzInfo.DaylightDate = tzi->DaylightDate;
                    descr->tzInfo.StandardBias = tzi->StandardBias;
                    descr->tzInfo.StandardDate = tzi->StandardDate;
                    free( tzi );

                    m_TimeZones.AddTail( descr );
                }
                dwIndex++;
            }
        }   while (lRes == ERROR_SUCCESS );

        delete[] pKeyClass;
        delete[] pKeyName;

        RegCloseKey( hOpenKey );

        success = ( m_TimeZones.GetCount( ) > 0 );
    }

    return success;
}
