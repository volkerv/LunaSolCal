/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + HauteCapture - (c) 2005 by Volker Voecking Software Engineering
   +
   + $Source: C:\\CVS_Repository/CybonTailTNG/shared/XEventLog.cpp,v $
   + $Author: volker $
   + $Date: 2005/01/15 12:50:28 $
   + $Revision: 1.2 $
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include "shlwapi.h"

#include "regAccess.h"

#pragma warning( disable : 4786 )

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CRegAccess


/////////////////////////////////////////////////////////////////////////////
// The one and only CRegAccess object

BYTE key[56] = {
    0x23, 0x93, 0xdd, 0x32, 0x1f, 0xff, 0x59, 0x29,
    0x12, 0x8a, 0xff, 0x4a, 0x32, 0x2c, 0x39, 0x01,
    0x45, 0x20, 0x35, 0xa9, 0x99, 0x64, 0xd9, 0x32,
    0x21, 0x9f, 0x63, 0xaf, 0x12, 0x37, 0xa9, 0xef,
    0x98, 0xf3, 0x2e, 0xdd, 0x02, 0x77, 0x19, 0xbc,
    0xa5, 0x56, 0x10, 0x4d, 0x22, 0x77, 0x21, 0xa2,
    0x3f, 0xfd, 0x00, 0x1b, 0xa2, 0x01, 0x2c, 0xac
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CRegAccess::CRegAccess()
{
    m_BlowFish.Initialize( key, 56);
}


bool CRegAccess::GetBoolValue( int nKeyID )
{
    bool bValue = false;
    
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
    {
        bool bDefault = false;
        if ( GetDefaultValue( nKeyID, bDefault ))
            bValue = GetDWORDFromRegistry( pRegPath, pKeyName, (DWORD) bDefault ) > 0;
    }

    return bValue;
}


void CRegAccess::SetBoolValue( int nKeyID, bool bValue )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
        SetDWORDInRegistry( HKEY_CURRENT_USER, pRegPath, pKeyName, (DWORD) bValue );
}


DWORD CRegAccess::GetDWORDValue( int nKeyID )
{
    DWORD dwValue = 0;

    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
    {
        if ( GetDefaultValue( nKeyID, dwValue ))
            dwValue = GetDWORDFromRegistry( pRegPath, pKeyName, dwValue );
    }
    
    return dwValue;
}


void CRegAccess::SetDWORDValue( int nKeyID, DWORD dwValue )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
        SetDWORDInRegistry( HKEY_CURRENT_USER, pRegPath, pKeyName, dwValue );
}


CString CRegAccess::GetStringValue( int nKeyID )
{
    USES_CONVERSION;
    CString szValue;
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';
    
    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
    {
        if ( GetDefaultValue( nKeyID, szValue ))
            szValue = GetStringFromRegistry( pRegPath, pKeyName, szValue );
    }

    return szValue;
}


void CRegAccess::SetStringValue( int nKeyID, CString szValue )
{
    USES_CONVERSION;
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
        SetStringInRegistry( HKEY_CURRENT_USER, pRegPath, pKeyName, szValue );
}


CString CRegAccess::GetEncodedStringValue( int nKeyID )
{
    CString szValue;
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';
    
    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
    {
        CString strDefaultValue;
        if ( GetDefaultValue( nKeyID, strDefaultValue ))
        {
            BYTE* pBuffer = NULL;
            DWORD ulBufSize = 0;

            bool bSuccess = GetBinaryValueFromRegistry( pRegPath, pKeyName, &ulBufSize, &pBuffer );
            if ( bSuccess && (ulBufSize > 0))
            {
                m_BlowFish.Decode( pBuffer, pBuffer, ulBufSize );
                szValue = CString((char *)pBuffer, (unsigned int)strlen((char *)pBuffer));
            }
            else
                szValue = strDefaultValue;

            delete pBuffer;
        }
    }

    return szValue;
}


void CRegAccess::SetEncodedStringValue( int nKeyID, CString szValue )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
    {
        const int nValueLen = szValue.GetLength();
        BYTE* pBuffer = new BYTE[100];
        memset( pBuffer, 0, 100 );
        _tcsncpy((TCHAR *)pBuffer, szValue.GetBuffer(100), nValueLen );
        DWORD dwOutLen = m_BlowFish.Encode( pBuffer, pBuffer, nValueLen );
        pBuffer[dwOutLen] = '\0';
        SetBinaryValueInRegistry( HKEY_CURRENT_USER, pRegPath, pKeyName, dwOutLen, pBuffer );
        delete[] pBuffer;
    }
}


bool CRegAccess::GetBinaryValue( int nKeyID, CString szValueName, ULONG* pulBufSize, BYTE** ppBuffer )
{
    USES_CONVERSION;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegKeyPath( nKeyID, pRegPath, MAX_PATH ))
        return GetBinaryValueFromRegistry( pRegPath, szValueName, pulBufSize, ppBuffer );

    return false;
}


bool CRegAccess::GetBinaryValue( int nKeyID, ULONG* pulBufSize, BYTE** ppBuffer )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
        return GetBinaryValueFromRegistry( pRegPath, pKeyName, pulBufSize, ppBuffer );

    return false;
}


void CRegAccess::SetBinaryValue( int nKeyID, CString szValueName, ULONG ulBufSize, BYTE* pBuffer )
{
    USES_CONVERSION;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegKeyPath( nKeyID, pRegPath, MAX_PATH ))
        SetBinaryValueInRegistry( HKEY_CURRENT_USER, pRegPath, szValueName, ulBufSize, pBuffer );
}


void CRegAccess::SetBinaryValue( int nKeyID, ULONG ulBufSize, BYTE* pBuffer )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
        SetBinaryValueInRegistry( HKEY_CURRENT_USER, pRegPath, pKeyName, ulBufSize, pBuffer );
}


LOGFONT* CRegAccess::GetFont( int nKeyID )
{
    LOGFONT* pLogFont = NULL;
    ULONG ulBufSize = sizeof( LOGFONT );

    GetBinaryValue( nKeyID, &ulBufSize, (BYTE **)&pLogFont );

    return pLogFont;
}


void CRegAccess::SetFont( int nKeyID, LOGFONT lf )
{
    SetBinaryValue( nKeyID, sizeof( LOGFONT ), (BYTE *)&lf );
}


bool CRegAccess::DeleteValue( int nKeyID )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName ))
        return DeleteValueFromRegistry( HKEY_CURRENT_USER, pRegPath, pKeyName );

    return false;
}


bool CRegAccess::DeleteValue( int nKeyID, CString szValueName )
{
    USES_CONVERSION;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';

    if ( GetRegKeyPath( nKeyID, pRegPath, MAX_PATH ))
        return DeleteValueFromRegistry( HKEY_CURRENT_USER, pRegPath, szValueName );

    return false;
}

void CRegAccess::GetListOfSubKeys( int nKeyID, CStringList& subKeys )
{
    const TCHAR* pKeyName = NULL;
    TCHAR pRegPath[MAX_PATH*sizeof(TCHAR)];
    pRegPath[0] = '\0';
    
    GetRegPathAndKeyName( nKeyID, pRegPath, MAX_PATH, &pKeyName );

    if ( pRegPath[0] != '\0' )
        GetListOfSubKeys( HKEY_CURRENT_USER, pRegPath, subKeys );
}


// helper function for registry access

// get a string value from the specified registry key
// returns lpDefault if key is not set
CString CRegAccess::GetStringFromRegistry( LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpDefault )
{
    USES_CONVERSION;
    CString szValue;
    bool bFound = GetStringFromRegistry( HKEY_CURRENT_USER, lpSubKey, lpValueName, szValue );

    if ( !bFound )
    {
        if (!GetStringFromRegistry( HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, szValue ))
        {
            try
            {
                szValue = T2A((TCHAR*)lpDefault);
            }
            catch(...)
            {
            }
        }
    }

    return szValue;
}

CString CRegAccess::GetStringFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpDefault )
{
    USES_CONVERSION;
    CString szValue;
    if (!GetStringFromRegistry( hKey, lpSubKey, lpValueName, szValue ))
    {
        try
        {
            szValue = T2A((TCHAR*)lpDefault);
        }
        catch(...)
        {
        }
    }

    return szValue;
}

bool CRegAccess::GetStringFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, CString& szValue )
{
    USES_CONVERSION;
    bool bFound = false;
    HKEY hOpenKey = 0;

	if (RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hOpenKey) == ERROR_SUCCESS)
	{				
        DWORD dwBufSize = MAX_PATH;
		DWORD dwType = 0;
        BYTE* pBuffer = new BYTE[dwBufSize];
        memset( pBuffer, 0, dwBufSize );

		if ((RegQueryValueEx( hOpenKey, lpValueName, NULL, &dwType, pBuffer, &dwBufSize) == ERROR_SUCCESS )
            && (dwType == REG_SZ))
        {
            // dwBufSize might includes terminating \0
            // test if the last character is the null char
            if ( (TCHAR)pBuffer[dwBufSize - sizeof(TCHAR)] == (TCHAR)'\0' )
                dwBufSize = dwBufSize - sizeof(TCHAR);

            try
            {
                szValue = CString( T2A((TCHAR*)pBuffer), dwBufSize/sizeof(TCHAR)); 
            }
            catch(...)
            {
            }

            bFound = true;
        }
        
        delete[] pBuffer;
        RegCloseKey( hOpenKey );
	}

    return bFound;
}


// set a registry key to the given string value
void CRegAccess::SetStringInRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, LPCTSTR lpValue )
{
    HKEY hOpenKey = 0;
    DWORD dwDisposition = 0;

	if (RegCreateKeyEx( hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hOpenKey, &dwDisposition ) == ERROR_SUCCESS)
	{				
		if (RegSetValueEx( hOpenKey, lpValueName, NULL, REG_SZ, (LPBYTE) lpValue, (lstrlen(lpValue) + 1) * sizeof(TCHAR)) != ERROR_SUCCESS)
		{
			// SET REG VALUE FAILED
		}

		RegCloseKey( hOpenKey );
	}
}

// get a DWORD value from the specified registry key
// returns dwDefault if key is not set
DWORD CRegAccess::GetDWORDFromRegistry( LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwDefault )
{
    DWORD dwValue = 0;
    bool bFound = GetDWORDFromRegistry( HKEY_CURRENT_USER, lpSubKey, lpValueName, dwValue );

    if ( !bFound )
    {
        if (!GetDWORDFromRegistry( HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, dwValue ))
            dwValue = dwDefault;
    }

    return dwValue;
}

bool CRegAccess::GetDWORDFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD& dwValue )
{
    DWORD dwBufSize = sizeof( DWORD );
    bool  bFound    = false;
    HKEY  hOpenKey  = 0;

	if (RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hOpenKey) == ERROR_SUCCESS)
	{				
		DWORD dwType = 0;

		bFound = (RegQueryValueEx( hOpenKey, lpValueName, NULL, &dwType, (LPBYTE)&dwValue, &dwBufSize) == ERROR_SUCCESS) 
                    && (dwType == REG_DWORD);

		RegCloseKey( hOpenKey );
	}

    return bFound;
}


// set a registry key to the given DWORD value
void CRegAccess::SetDWORDInRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, DWORD dwValue )
{
    HKEY hOpenKey = 0;
    DWORD dwDisposition = 0;

	if (RegCreateKeyEx( hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hOpenKey, &dwDisposition ) == ERROR_SUCCESS)
	{				
		if (RegSetValueEx( hOpenKey, lpValueName, NULL, REG_DWORD, (LPBYTE) &dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
		{
			// SET REG VALUE FAILED
		}

		RegCloseKey( hOpenKey );
	}
}


void CRegAccess::GetListOfSubKeys( HKEY hKey, LPCTSTR lpSubKey, CStringList& keyList )
{
    USES_CONVERSION;
    HKEY hOpenKey = 0;

	if (RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hOpenKey) == ERROR_SUCCESS)
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
                try
                {
                    CT2A pA(pKeyName);
				    CString keyName = CString( pA, dwNameBufLen );
                    keyList.AddTail( keyName );
                    dwIndex++;
                }
                catch(...)
                {
                }
            }
        }   while (lRes == ERROR_SUCCESS );

        delete[] pKeyClass;
        delete[] pKeyName;

        RegCloseKey( hOpenKey );
    }
}


void CRegAccess::SetBinaryValueInRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, ULONG ulBufSize, BYTE* pBuffer )
{
    HKEY hOpenKey = 0;
    DWORD dwDisposition = 0;

	if (RegCreateKeyEx( hKey, lpSubKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hOpenKey, &dwDisposition ) == ERROR_SUCCESS)
	{				
		if (RegSetValueEx( hOpenKey, lpValueName, NULL, REG_BINARY, (LPBYTE) pBuffer, ulBufSize) != ERROR_SUCCESS)
    {
			// SET REG VALUE FAILED
		}

		RegCloseKey( hOpenKey );
	}
}


bool CRegAccess::GetBinaryValueFromRegistry( LPCTSTR lpSubKey, LPCTSTR lpValueName, ULONG* pulBufSize, BYTE** ppBuffer )
{
    if (!GetBinaryValueFromRegistry( HKEY_CURRENT_USER, lpSubKey, lpValueName, pulBufSize, ppBuffer ))
        return GetBinaryValueFromRegistry( HKEY_LOCAL_MACHINE, lpSubKey, lpValueName, pulBufSize, ppBuffer );
    else
        return true;
}


bool CRegAccess::GetBinaryValueFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName, ULONG* pulBufSize, BYTE** ppBuffer )
{
    bool bSuccess = false;

    DWORD dwBufSize = MAX_PATH;
    BYTE* pBuffer = (BYTE *) new BYTE[MAX_PATH * sizeof(BYTE)];
    memset( pBuffer, 0, MAX_PATH * sizeof(BYTE));

    HKEY hOpenKey = 0;

    if (RegOpenKeyEx( hKey, lpSubKey, 0, KEY_READ, &hOpenKey) == ERROR_SUCCESS)
    {				
        bool bContinue = true;
        unsigned int nRetryCount = 1;

        while (bContinue && (nRetryCount < 11))
        {
            DWORD dwType = 0;
            LONG hRes = RegQueryValueEx( hOpenKey, lpValueName, NULL, &dwType, pBuffer, &dwBufSize );
            if ( hRes == ERROR_SUCCESS )
            {
                bContinue = false;
                *ppBuffer = pBuffer;
                *pulBufSize = dwBufSize;
                bSuccess = true;
            }
            else
            {
                if (hRes == ERROR_MORE_DATA)
                {
                    // ups - the buffer is too small

                    //clogstream.level(3) << header << _T("Reading binary value from registry - Buffer too small: nRetryCount = ") << nRetryCount << endl;

                    nRetryCount++;
                    pBuffer = (BYTE *)realloc( pBuffer, nRetryCount * MAX_PATH * sizeof(BYTE) );
                }
                else
                {
                    delete pBuffer;
                    bContinue = false;
                }
            }
        }

        RegCloseKey( hOpenKey );
    }
    else
        delete pBuffer;

    return bSuccess;
}


bool CRegAccess::DeleteValueFromRegistry( HKEY hKey, LPCTSTR lpSubKey, LPCTSTR lpValueName )
{
    bool bSuccess = false;
    HKEY hOpenKey = 0;

    if (RegOpenKeyEx( hKey, lpSubKey, 0, KEY_SET_VALUE, &hOpenKey) == ERROR_SUCCESS)
    {				
        bSuccess = RegDeleteValue( hOpenKey, lpValueName ) == ERROR_SUCCESS;
        RegCloseKey( hOpenKey );
    }

    return bSuccess;
}

bool CRegAccess::DeleteKey( LPCTSTR lpSubKey )
{
    bool bSuccess = ( SHDeleteKey( HKEY_CURRENT_USER, lpSubKey ) == ERROR_SUCCESS );

    return bSuccess;
}


//=================================================================================================
//  Interface function for ConfigService
//=================================================================================================

// returns the complete path to a registry key
bool CRegAccess::GetRegKeyPath( int nKeyID, LPTSTR pBuffer, DWORD dwBufferLen )
{
    bool bSuccess = true;
    int nHierarchyKeyID = 0;

    if (IsHierarchyKeyID( nKeyID ))
        nHierarchyKeyID = nKeyID;
    else
        bSuccess = GetHierarchyKeyIDFromKeyID( nKeyID, nHierarchyKeyID );
    
    if ( bSuccess )
    {
        GetRegHierarchyKeyPath( nHierarchyKeyID, pBuffer, dwBufferLen );
        bSuccess = _tcslen( pBuffer ) != 0;
    }

    return bSuccess;
}

void CRegAccess::GetRegHierarchyKeyPath( int nKeyID, LPTSTR pBuffer, DWORD dwBufferLen )
{
    USES_CONVERSION;
    REG_CONFIG_HIERARCHY* pHierarchyKeyData = GetHierarchyKeyData( nKeyID );
    
    if ( pHierarchyKeyData )
    {
        TCHAR pPath[MAX_PATH*sizeof(TCHAR)];
        pPath[0] = '\0';

        GetRegHierarchyKeyPath( pHierarchyKeyData->nLinkID, pPath, MAX_PATH );
        DWORD dwRemainingBufLen = MAX_PATH;
        if ( _tcslen( pPath ) > 0 )
        {
            _tcsncpy( pBuffer, pPath, dwBufferLen );
            dwRemainingBufLen = dwBufferLen - _tcslen( pPath );
            try
            {
                TCHAR *p = A2T("\\");
                if ( p )
                {
                    _tcsncat( pBuffer, p, dwRemainingBufLen );
                }
            }
            catch(...)
            {
            }
    
            dwRemainingBufLen -= 1;
        }

        if ( pHierarchyKeyData )
            _tcsncat( pBuffer, pHierarchyKeyData->pPath, dwRemainingBufLen );
    }
}

bool CRegAccess::GetRegPathAndKeyName( int nKeyID, LPTSTR pRegPath, DWORD dwMaxLen, LPCTSTR* ppKeyName )
{
    bool bSuccess = false;

    if ( GetRegKeyPath( nKeyID, pRegPath, dwMaxLen ))
    {
        REG_CONFIG_KEYDATA* pKeyData = GetKeyData( nKeyID );

        if ( pKeyData )
        {
            bSuccess = true;
            *ppKeyName = pKeyData->pKeyName;
        }
    }

    return bSuccess;
}

CString CRegAccess::GetValueName( int nKeyID )
{
    CString szValueName;
    REG_CONFIG_KEYDATA* pKeyData = GetKeyData( nKeyID );

    if ( pKeyData )
        szValueName = pKeyData->pKeyName;

    return szValueName;
}

#pragma warning( default : 4786 )

