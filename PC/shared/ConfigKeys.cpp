/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

// Definition of the configuration keys and values

#include "stdafx.h"
#include "customer.h"
#include "TCHAR.h"

#pragma warning (disable: 4018 4100 4146 4244 4663 4786 4275 )
#include "regAccess.h"
#pragma warning (disable: 4018 4100 4146 4244 4663 4786 4275 )
#include "ConfigKeys.h"
#pragma warning (disable: 4018 4100 4146 4244 4663 4786 4275 )


//=================================================================================================
//  Definition of the hierarchy paths
//=================================================================================================

REG_CONFIG_HIERARCHY CRegAccess::Hierarchy_Table[] =
{
//-------------------------------------------------------------------------------------------------
// HIERARCHY ID             | UPPER HIERARCHY ID        | HIERARCHY DEFINITION
//-------------------------------------------------------------------------------------------------
{  KEY_COMPANY_ROOT,          KEY_NONE,                   CUSTOM_REG_COMPANY_ROOT },
{  KEY_LUNASOLCAL_APP,        KEY_COMPANY_ROOT,           CUSTOM_APPLICATION_NAME_REG },

{  KEY_LUNASOLCAL_LOGGING,    KEY_LUNASOLCAL_APP,         _T("Logging") },
{  KEY_LUNASOLCAL_GUI,        KEY_LUNASOLCAL_APP,         _T("GUI") },
};


//=================================================================================================
//  Definition of the keys
//=================================================================================================

REG_CONFIG_KEYDATA CRegAccess::KeyData_Table[] =
{
//-----------------------------------------------------------------------------------------------------------------------
// KEY ID                           | UPPER HIERARCHY      | KEY NAME                    | KEY TYPE | DEFAULTS    
//-----------------------------------------------------------------------------------------------------------------------

// app
{  KEY_LUNASOLCAL_SELECTED_CONTINENT, KEY_LUNASOLCAL_APP,   _T("Continent"),                KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_SELECTED_COUNTRY,   KEY_LUNASOLCAL_APP,   _T("Country"),                  KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_SELECTED_CITY,      KEY_LUNASOLCAL_APP,   _T("City"),                     KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_SELECTED_STATE,     KEY_LUNASOLCAL_APP,   _T("State"),                    KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_SELECTED_TIMEZONE,  KEY_LUNASOLCAL_APP,   _T("TimeZone"),                 KT_INT,   KDEF_NUM(-1) },
{  KEY_LUNASOLCAL_LATITUDE,           KEY_LUNASOLCAL_APP,   _T("Latitude"),                 KT_STR,   _T("52") },
{  KEY_LUNASOLCAL_LONGITUDE,          KEY_LUNASOLCAL_APP,   _T("Longitude"),                KT_STR,   _T("13") },

// logging
{  KEY_LUNASOLCAL_LOGFILENAME,        KEY_LUNASOLCAL_LOGGING,   _T("LogFileName"),          KT_STR,   CUSTOM_REG_LOGFILENAME },
{  KEY_LUNASOLCAL_LOGLEVEL,           KEY_LUNASOLCAL_LOGGING,   _T("LogLevel"),             KT_INT,   KDEF_NUM(1) },
{  KEY_LUNASOLCAL_MAXLOGFILESIZE,     KEY_LUNASOLCAL_LOGGING,   _T("MaxLogFileSize"),       KT_INT,   KDEF_NUM(1048576) },         // 1M
{  KEY_LUNASOLCAL_LISTLOADEDDLLS,     KEY_LUNASOLCAL_LOGGING,   _T("ListLoadedDLLs"),       KT_BOOL,  KDEF_TRUE },

// GUI
{  KEY_LUNASOLCAL_WIN_POS_LEFT,       KEY_LUNASOLCAL_GUI,       _T("WindowPosLeft"),        KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_WIN_POS_TOP,        KEY_LUNASOLCAL_GUI,       _T("WindowPosTop"),         KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_WIN_POS_RIGHT,      KEY_LUNASOLCAL_GUI,       _T("WindowPosRight"),       KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_WIN_POS_BOTTOM,     KEY_LUNASOLCAL_GUI,       _T("WindowPosBottom"),      KT_INT,   KDEF_NUM(0) },
{  KEY_LUNASOLCAL_LANGUAGE_MARKER,    KEY_LUNASOLCAL_GUI,       _T("GUILanguage"),          KT_STR,   _T("") },

};



REG_CONFIG_KEYDATA* CRegAccess::GetKeyData( int nID )
{
    for (unsigned int i=0; i<sizeof(KeyData_Table)/sizeof(REG_CONFIG_KEYDATA); i++)
    {
        if ( KeyData_Table[i].nKeyID == nID )
            return &KeyData_Table[i];
    }

    return NULL;
}

REG_CONFIG_HIERARCHY* CRegAccess::GetHierarchyKeyData( int nID )
{
    for (unsigned int i=0; i<sizeof(Hierarchy_Table)/sizeof(REG_CONFIG_HIERARCHY); i++)
    {
        if ( Hierarchy_Table[i].nKeyID == nID )
            return &Hierarchy_Table[i];
    }

    return NULL;
}

bool CRegAccess::GetHierarchyKeyIDFromKeyID( int nID, int& nHierarchyKeyID )
{
    bool bSuccess = false;

    for (unsigned int i=0; i<sizeof(KeyData_Table)/sizeof(REG_CONFIG_KEYDATA); i++)
    {
        if ( KeyData_Table[i].nKeyID == nID )
        {
            nHierarchyKeyID = KeyData_Table[i].nLinkID;
            bSuccess = true;
            break;
        }
    }

    return bSuccess;
}

bool CRegAccess::GetDefaultValue( int nID, bool& bDefault )
{
    bool bSuccess = false;

    for (unsigned int i=0; i<sizeof(KeyData_Table)/sizeof(REG_CONFIG_KEYDATA); i++)
    {
        if ( KeyData_Table[i].nKeyID == nID )
        {
            bDefault = ( KeyData_Table[i].pDefault == KDEF_TRUE );
            bSuccess = true;
            break;
        }
    }

    return bSuccess;
}

bool CRegAccess::GetDefaultValue( int nID, DWORD& dwDefault )
{
    bool bSuccess = false;

    for (unsigned int i=0; i<sizeof(KeyData_Table)/sizeof(REG_CONFIG_KEYDATA); i++)
    {
        if ( KeyData_Table[i].nKeyID == nID )
        {
            dwDefault = (ULONG64)(KeyData_Table[i].pDefault);
            bSuccess = true;
            break;
        }
    }

    return bSuccess;
}

bool CRegAccess::GetDefaultValue( int nID, CString& szDefault )
{
    USES_CONVERSION;
    bool bSuccess = false;

    for (unsigned int i=0; i<sizeof(KeyData_Table)/sizeof(REG_CONFIG_KEYDATA); i++)
    {
        if ( KeyData_Table[i].nKeyID == nID )
        {
            CT2A pA((TCHAR*)KeyData_Table[i].pDefault);
            szDefault = pA;
            bSuccess = true;
            break;
        }
    }

    return bSuccess;
}

bool CRegAccess::IsHierarchyKeyID( int nKeyID )
{
    return ( NULL != GetHierarchyKeyData( nKeyID ));
}
