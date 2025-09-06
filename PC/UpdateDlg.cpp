/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include "lunasol.h"
#include "UpdateDlg.h"


// UpdateDlg dialog

IMPLEMENT_DYNAMIC(UpdateDlg, CDialog)

UpdateDlg::UpdateDlg(CWnd* pParent /*=NULL*/)
: CLocalizableDialog(UpdateDlg::IDD, pParent)
    , m_Installed(_T(""))
    , m_Latest(_T(""))
    , m_NewVersionAvailableLabelText(_T(""))
    , m_InstalledVersionLabelText(_T(""))
    , m_NewVersionLabelText(_T(""))
{

}

UpdateDlg::~UpdateDlg()
{
}

BOOL UpdateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    
    LocalizeString( m_NewVersionAvailableLabelText, IDS_UPDATE_NEW_VERSION_AVAILABLE );
    LocalizeString( m_InstalledVersionLabelText, IDS_UPDATE_INSTALLED_VERSION );
    LocalizeString( m_NewVersionLabelText, IDS_UPDATE_NEW_VERSION );
    LocalizeButton( m_OKButton, IDS_UPDATE_NOW );
    LocalizeTitle( IDS_UPDATE_TITLE );

    UpdateData( FALSE );

	return TRUE;
}

void UpdateDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_STATIC_INSTALLED_VERSION, m_Installed);
    DDX_Text(pDX, IDC_STATIC_LATEST_VERSION, m_Latest);
    DDX_Text(pDX, IDC_STATIC_PRODUCT_NAME, m_NewVersionAvailableLabelText);
    DDX_Text(pDX, IDC_STATIC_VERSION_INSTALLED, m_InstalledVersionLabelText);
    DDX_Text(pDX, IDC_STATIC_NEW_VERSION, m_NewVersionLabelText);
    DDX_Control(pDX, IDOK, m_OKButton);
}


BEGIN_MESSAGE_MAP(UpdateDlg, CDialog)
END_MESSAGE_MAP()


// UpdateDlg message handlers

void UpdateDlg::SetVersions( const CString& installed, const CString& latest )
{
    m_Installed = installed;
    m_Latest = latest;
}

