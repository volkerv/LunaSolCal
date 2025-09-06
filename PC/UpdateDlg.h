/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once
#include "afxwin.h"
#include "LocalizableDialog.h"

// UpdateDlg dialog

class UpdateDlg : public CLocalizableDialog
{
	DECLARE_DYNAMIC(UpdateDlg)

public:
	UpdateDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~UpdateDlg();

    void SetVersions( const CString& installed, const CString& latest );

// Dialog Data
	enum { IDD = IDD_UPDATE };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
    CString m_Installed;
    CString m_Latest;

    CString m_NewVersionAvailableLabelText;
    CString m_InstalledVersionLabelText;
    CString m_NewVersionLabelText;
    CButton m_OKButton;
};
