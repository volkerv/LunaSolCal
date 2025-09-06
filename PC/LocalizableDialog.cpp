#include "StdAfx.h"
#include "LocalizableDialog.h"

CLocalizableDialog::CLocalizableDialog(void)
{
}
CLocalizableDialog::CLocalizableDialog( UINT nIDTemplate, CWnd* pParentWnd ) : CDialog( nIDTemplate, pParentWnd )
{
}

CLocalizableDialog::~CLocalizableDialog(void)
{
}

void CLocalizableDialog::LocalizeString( CString& str, UINT nID )
{
    str.LoadString( nID );
    if ( str.IsEmpty( ) )
    {
        // fallback to english
        str.LoadString( NULL, nID, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US));
    }
}

void CLocalizableDialog::LocalizeButton( CButton& btn, UINT nID )
{
    CString str;
    LocalizeString( str, nID );

    btn.SetWindowText( str );
}

void CLocalizableDialog::LocalizeTitle( UINT nID )
{
    CString title;
    LocalizeString( title, nID );
    SetWindowText( title );
}