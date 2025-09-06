#pragma once
#include "afxwin.h"

class CLocalizableDialog : public CDialog
{
public:
    CLocalizableDialog( );
    CLocalizableDialog( UINT nIDTemplate, CWnd* pParentWnd = NULL );

    ~CLocalizableDialog( );

protected:
    void LocalizeString( CString& str, UINT nID );
    void LocalizeButton( CButton& btn, UINT nID );
    void LocalizeTitle( UINT nID );
};
