/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */


#pragma once
#include "afxwin.h"
#include "calc.h"
#include "celestialCalc.h"
#include "TimeZoneManager.h"
#include "UpdateManager.h"
#include "UpdateDownloadProgress.h"
#include "LocalizableDialog.h"

#define VERSIONCHECK_URL L"https://www.vvse.com/cur/versions/updatecheck.php?product="

// CLunaSolDlg dialog
class CLunaSolDlg : public CLocalizableDialog
{
// Construction
public:
	CLunaSolDlg(CWnd* pParent = NULL);	// standard constructor
    ~CLunaSolDlg( );

// Dialog Data
	enum { IDD = IDD_LUNASOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    BOOL PreTranslateMessage(MSG* pMsg);

    typedef struct
    {
        unsigned int nameResID;
        TCHAR *fileNamePrefix;
    } ContinentDef;

    typedef struct
    {
        bool hasStates;
        CString filename;
    } CountryDef;

    typedef struct
    {
        double latitude;
        double longitude;
    } CityDef;

// Implementation
protected:
	HICON m_hIcon;
	CBrush m_brush;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnClose();
	afx_msg void OnBnClickedButtonAbout();
    afx_msg void OnCbnSelchangeComboContinent();
    afx_msg void OnCbnSelchangeComboCountry();
    afx_msg void OnCbnSelchangeComboCity();
    afx_msg void OnCbnSelchangeComboState();
    afx_msg void OnCbnSelchangeComboTimeZone();
    afx_msg void OnBnClickedButtonUpdateCheck();
    afx_msg void OnTimer( UINT );
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	DECLARE_MESSAGE_MAP()

private:
    void SetLocale( );
	void UpdateInfo( );
    void FillCountryList( );
    void FillStateList( );
    void FillStateOrCity( );
    void FillCityListForCountry( );
    void FillCityListForState( );
    void FillCityList( const CString& filePath );
	CString FormatTime( double time2format );
    CString FormatDate( SYSTEMTIME& date );
    void SaveWindowPos( );
    void UpdatePosition( );
    void ShowStateList( bool show );
    void UpdateSelectedTimeZoneLabel( );
    TIME_ZONE_INFORMATION GetSelectedTimeZoneInformation( );
    bool IsDaylightSaving( const TIME_ZONE_INFORMATION& tzInfo );
    double GetUTCDiff( const TIME_ZONE_INFORMATION& tzInfo );
    // LONGLONG GetUTCDiff( const TIME_ZONE_INFORMATION& tzInfo );
    void NextPrevDate( bool next );
    void ClearCountryComboBox( CComboBox& comboBox );
    void ClearCityComboBox( CComboBox& comboBox );
    void GetLinesFromFile( const CString& filePath, CStringArray& lines );
    bool IsBefore( bool v1Valid, double v1, bool v2Valid, double v2 );
    void ArrangeMoonRiseMoonSet( bool moonRiseBeforeMoonSet );
    void MoveLabel( CWnd& label, RECT topLeft, RECT rect );
    void Localize( );
    CString GetDayLengthString( double dayLength );
	SYSTEMTIME DoubleTime2SystemTime( double time2convert );
    SYSTEMTIME DoubleTime2UTCSystemTime( double time2convert );
    CelestialCalculator::Date DoubleTime2UTCDate( double time2convert );
    SYSTEMTIME Date2SystemTimeLocal( const CelestialCalculator::Date& d );
    void ShowUrl( const UINT id );

	CString GetLocationFolderName();

	CString		m_Date;
	CString		m_SunRise;
	CString		m_SunSet;
    CString     m_SunNoon;
    CString     m_SunDeclination;
    CString     m_SunTimeEquation;
	CString     m_SunRiseAzimuth;
	CString     m_SunSetAzimuth;

	CString		m_DateLabelText;
    CString     m_TimeZoneLabelText;
	CString		m_SunRiseLabelText;
	CString		m_SunSetLabelText;
    CString     m_SunNoonLabelText;
    CString     m_SunDeclinationLabelText;
    CString     m_SunTimeEquationLabelText;
	CString     m_SunRiseAzimuthLabelText;
	CString     m_SunSetAzimuthLabelText;
	CString		m_MoonRiseLabelText;
	CString		m_MoonSetLabelText;
    CString     m_MoonNoonLabelText;
    CString     m_MoonDeclinationLabelText;
	CString     m_MoonRiseAzimuthLabelText;
	CString     m_MoonSetAzimuthLabelText;
    CString     m_DayLengthLabelText;
    CString     m_TwilightLabelText;
    CString     m_TwilightAstronomicalLabelText;
    CString     m_TwilightNauticalLabelText;
    CString     m_TwilightCivilLabelText;
    CString     m_MoonVisibilityLabelText;
    CString     m_MoonPhaseLabelText;
    CString     m_NextNewMoonLabelText;
    CString     m_NextFullMoonLabelText;
    CString     m_ContinentLabelText;
    CString     m_CountryLabelText;
    CString     m_CityLabelText;
    CString     m_StateLabelText;
    CString     m_LatitudeLabelText;
    CString     m_LongitudeLabelText;

    CString		m_MoonRise;
	CString		m_MoonSet;
    CString     m_MoonNoon;
    CString     m_MoonDeclination;
    CString     m_MoonPhaseName;
    CString     m_MoonVisibility;
	CString     m_MoonRiseAzimuth;
	CString     m_MoonSetAzimuth;
    
    CString     m_DayLengthValue;
    CString     m_TwilightAstronStartValue;
    CString     m_TwilightAstronEndValue;
    CString     m_TwilightNautStartValue;
    CString     m_TwilightNautEndValue;
    CString     m_TwilightCivilStartValue;
    CString     m_TwilightCivilEndValue;

    CString     m_NextNewMoonValueText;
    CString     m_NextFullMoonValueText;

    CComboBox	m_ContinentList;
    CComboBox	m_CountryList;
	CComboBox	m_CityList;
	CComboBox	m_StateList;
	CComboBox   m_TimeZoneList;
    CString		m_Latitude;
	CString		m_Longitude;
    CString     m_SelectedTimeZone;

	Calculator	m_Calculator;
	Calculator	m_MoonCalculator;
	double		m_LatitudeValue;
	double		m_LongitudeValue;
	CTime		m_TheDate;
    CStatic     m_MoonPhase;
    CStatic     m_StateLabel;
    CStatic     m_CountryLabel;
    CStatic     m_CityLabel;

    RECT        m_CityLabelRect;
    RECT        m_CityListRect;
    RECT        m_StateLabelRect;
    RECT        m_StateListRect;
    RECT        m_MoonRiseLabelRect;
    RECT        m_MoonRiseValueRect;
    RECT        m_MoonRiseAzimuthLabelRect;
    RECT        m_MoonRiseAzimuthValueRect;
    RECT        m_MoonSetLabelRect;
    RECT        m_MoonSetValueRect;
    RECT        m_MoonSetAzimuthLabelRect;
    RECT        m_MoonSetAzimuthValueRect;
    RECT        m_NextFullMoonLabelRect;
    RECT        m_NextFullMoonValueRect;
    RECT        m_NextNewMoonLabelRect;
    RECT        m_NextNewMoonValueRect;

    CDateTimeCtrl m_DateTimeControl;
    CButton       m_TodayButton;

    CStatic     m_SunRiseLabel;
    CStatic     m_SunSetLabel;
    CStatic     m_MoonRiseLabel;
    CStatic     m_MoonRiseValue;
    CStatic     m_MoonRiseAzimuthLabel;
    CStatic     m_MoonRiseAzimuthValue;
    CStatic     m_MoonSetLabel;
    CStatic     m_MoonSetValue;
    CStatic     m_MoonSetAzimuthLabel;
    CStatic     m_MoonSetAzimuthValue;
    CStatic     m_SunRiseValue;
    CStatic     m_SunSetValue;

    CStatic     m_TwilightLabel;
    CStatic     m_TwilightAstronLabel;
    CStatic     m_TwilightNautLabel;
    CStatic     m_TwilightCivilLabel;

    CStatic     m_NextNewMoonLabel;
    CStatic     m_NextFullMoonLabel;

    CButton     m_UpdateButton;
    CButton     m_AboutButton;
    
    CPtrList                m_Cities;
    TimeZoneManager         m_TimeZoneManager;
    UpdateManager           m_UpdateManager;
    UpdateDownloadProgress *m_pUpdateDownloadDlg; 

    UINT_PTR    m_DownloadTimerEventID;
    UINT_PTR    m_DownloadTimer;

    CFont       m_BigLabelFont;

    static ContinentDef m_Continents[];

    int         m_nSelectedContinentIdx;
    int         m_nSelectedCountryIdx;
    int         m_nSelectedStateIdx;
    int         m_nSelectedCityIdx;

    CString     m_Language;
    CString     m_AppPath;

    bool        m_IsNextFullMoonSwapped;
    bool        m_IsMoonRiseSetSwapped;

public:
	afx_msg void OnEnChangeEditLat();
	afx_msg void OnEnKillFocusEditLat();
	afx_msg void OnEnChangeEditLon();
	afx_msg void OnEnKillFocusEditLon();
    afx_msg void OnDtnDatetimechangeDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedButtonPrev();
    afx_msg void OnBnClickedButtonNext();
    afx_msg void OnBnClickedButtonToday();

    CStatic m_NextNewMoonValue;
    CStatic m_NextFullMoonValue;
    CString m_LunaSolCal_Mobile_LabelText;
    afx_msg void OnBnClickedButtonIphone();
    afx_msg void OnBnClickedButtonAndroid();
    afx_msg void OnBnClickedButtonWp();
};
