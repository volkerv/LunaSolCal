/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#include "stdafx.h"
#include "lunasol.h"
#include "LunaSolDlg.h"
#include "AboutDlg.h"
#include "UpdateDlg.h"
#include <locale.h>

#include "LogStream.h"
#include "RegAccess.h"
#include "customer.h"

#include "celestialCalc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern CRegAccess g_Config;

CLunaSolDlg::ContinentDef CLunaSolDlg::m_Continents[] = { 
    { IDS_AFRICA, _T("africa") },
    { IDS_EUROPE, _T("europe") },
    { IDS_ASIA, _T("asia") },
    { IDS_NORTH_AMERICA, _T("north_america") },
    { IDS_SOUTH_AMERICA, _T("south_america") },
    { IDS_OCEANIA, _T("oceania") }
};

CLunaSolDlg::CLunaSolDlg(CWnd* pParent /*=NULL*/)
: CLocalizableDialog(CLunaSolDlg::IDD, pParent)
	, m_Date(_T(""))
	, m_SunRise(_T(""))
	, m_SunSet(_T(""))
	, m_MoonRise(_T(""))
	, m_MoonSet(_T(""))
	, m_Latitude(_T(""))
	, m_Longitude(_T(""))
	, m_LatitudeValue( 52.5161564 )
	, m_LongitudeValue( 13.3775096 )
    , m_SelectedTimeZone(_T(""))
    , m_pUpdateDownloadDlg( NULL )
    , m_DownloadTimerEventID( 123 )
    , m_nSelectedContinentIdx( -1 )
    , m_nSelectedCountryIdx( -1 )
    , m_nSelectedStateIdx( -1 )
    , m_nSelectedCityIdx( -1 )
    , m_DayLengthLabelText(_T(""))
    , m_TwilightLabelText(_T(""))
    , m_TwilightAstronomicalLabelText(_T(""))
    , m_TwilightNauticalLabelText(_T(""))
    , m_TwilightCivilLabelText(_T(""))
    , m_MoonVisibilityLabelText(_T(""))
    , m_MoonPhaseLabelText(_T(""))
    , m_NextNewMoonLabelText(_T(""))
    , m_NextFullMoonLabelText(_T(""))
    , m_ContinentLabelText(_T(""))
    , m_CountryLabelText(_T(""))
    , m_CityLabelText(_T(""))
    , m_StateLabelText(_T(""))
    , m_LatitudeLabelText(_T(""))
    , m_LongitudeLabelText(_T(""))
    , m_DayLengthValue(_T(""))
    , m_IsNextFullMoonSwapped( false )
    , m_IsMoonRiseSetSwapped( false )
    , m_SunTimeEquation(_T(""))
    , m_SunTimeEquationLabelText(_T(""))
    , m_SunDeclination(_T(""))
    , m_SunDeclinationLabelText(_T(""))
    , m_MoonDeclination(_T(""))
    , m_MoonDeclinationLabelText(_T(""))
    , m_LunaSolCal_Mobile_LabelText(_T(""))
{
	m_hIcon = AfxGetApp( )->LoadIcon(IDR_MAINFRAME);
}

CLunaSolDlg::~CLunaSolDlg( )
{
    delete m_pUpdateDownloadDlg;
}

void CLunaSolDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_DATETIMEPICKER, m_Date);
    DDX_Text(pDX, IDC_STATIC_DATE_LABEL, m_DateLabelText );
    DDX_Control(pDX, IDC_DATETIMEPICKER, m_DateTimeControl);
    DDX_Text(pDX, IDC_STATIC_SUNRISE_VALUE, m_SunRise);
    DDX_Text(pDX, IDC_STATIC_SUNSET_VALUE, m_SunSet);
    DDX_Text(pDX, IDC_VALUE_SUN_NOON, m_SunNoon);
    DDX_Text(pDX, IDC_STATIC_MOONRISE_VALUE, m_MoonRise);
    DDX_Text(pDX, IDC_STATIC_MOONSET_VALUE, m_MoonSet);
    DDX_Text(pDX, IDC_VALUE_MOON_NOON, m_MoonNoon);
    DDX_Text(pDX, IDC_LABEL_SUN_NOON, m_SunNoonLabelText);
    DDX_Text(pDX, IDC_LABEL_MOON_NOON, m_MoonNoonLabelText);
    DDX_Text(pDX, IDC_VALUE_MOON_PHASE, m_MoonPhaseName);
    DDX_Text(pDX, IDC_VALUE_MOON_VISIBILITY, m_MoonVisibility);
    DDX_Control(pDX, IDC_COMBO_CONTINENT, m_ContinentList);
    DDX_Control(pDX, IDC_COMBO_COUNTRY, m_CountryList);
    DDX_Control(pDX, IDC_COMBO_CITY, m_CityList);
    DDX_Control(pDX, IDC_COMBO_STATE, m_StateList);
    DDX_Text(pDX, IDC_EDIT_LAT, m_Latitude);
    DDX_Text(pDX, IDC_EDIT_LAT2, m_Longitude);
    DDX_Control(pDX, IDC_STATIC_PIC_MOON_PHASE, m_MoonPhase);
    DDX_Control(pDX, IDC_STATIC_STATE, m_StateLabel);
    DDX_Control(pDX, IDC_STATIC_COUNTRY, m_CountryLabel);
    DDX_Control(pDX, IDC_STATIC_CITY, m_CityLabel);
    DDX_Control(pDX, IDC_COMBO_TIME_ZONE, m_TimeZoneList);
    DDX_Text(pDX, IDC_STATIC_TIMEZONE_LABEL, m_TimeZoneLabelText);
    DDX_Text(pDX, IDC_STATIC_TIMEZONE, m_SelectedTimeZone);
    DDX_Text(pDX, IDC_VALUE_TWILIGHT_ASTRON_START, m_TwilightAstronStartValue);
    DDX_Text(pDX, IDC_VALUE_TWILIGHT_ASTRON_END, m_TwilightAstronEndValue);
    DDX_Text(pDX, IDC_VALUE_TWILIGHT_NAUT_START, m_TwilightNautStartValue);
    DDX_Text(pDX, IDC_VALUE_TWILIGHT_NAUT_END, m_TwilightNautEndValue);
    DDX_Text(pDX, IDC_VALUE_TWILIGHT_CIVIL_START, m_TwilightCivilStartValue);
    DDX_Text(pDX, IDC_VALUE_TWILIGHT_CIVIL_END, m_TwilightCivilEndValue);
    DDX_Text(pDX, IDC_VALUE_NEXT_NEW_MOON, m_NextNewMoonValueText);
    DDX_Text(pDX, IDC_VALUE_NEXT_FULL_MOON, m_NextFullMoonValueText);

    DDX_Control(pDX, IDC_STATIC_SUNRISE_LABEL, m_SunRiseLabel);
    DDX_Control(pDX, IDC_STATIC_SUNSET_LABEL, m_SunSetLabel);
    DDX_Control(pDX, IDC_STATIC_MOONRISE_LABEL, m_MoonRiseLabel);
    DDX_Control(pDX, IDC_STATIC_MOONSET_LABEL, m_MoonSetLabel);
    DDX_Control(pDX, IDC_LABEL_MOONRISE_AZIMUTH, m_MoonRiseAzimuthLabel);
    DDX_Control(pDX, IDC_LABEL_MOONSET_AZIMUTH, m_MoonSetAzimuthLabel);
    DDX_Control(pDX, IDC_STATIC_MOONRISE_VALUE, m_MoonRiseValue);
    DDX_Control(pDX, IDC_STATIC_MOONSET_VALUE, m_MoonSetValue);
    DDX_Control(pDX, IDC_VALUE_MOONRISE_AZIMUTH, m_MoonRiseAzimuthValue);
    DDX_Control(pDX, IDC_VALUE_MOONSET_AZIMUTH, m_MoonSetAzimuthValue);
    DDX_Control(pDX, IDC_STATIC_SUNRISE_VALUE, m_SunRiseValue);
    DDX_Control(pDX, IDC_STATIC_SUNSET_VALUE, m_SunSetValue);

    DDX_Control(pDX, IDC_LABEL_TWILIGHT, m_TwilightLabel);
    DDX_Control(pDX, IDC_LABEL_TWILIGHT_ASTRONOMICAL, m_TwilightAstronLabel);
    DDX_Control(pDX, IDC_LABEL_TWILIGHT_NAUT, m_TwilightNautLabel);
    DDX_Control(pDX, IDC_LABEL_TWILIGHT_CIVIL, m_TwilightCivilLabel);
    DDX_Control(pDX, IDC_LABEL_NEXT_NEW_MOON, m_NextNewMoonLabel);
    DDX_Control(pDX, IDC_LABEL_NEXT_FULL_MOON, m_NextFullMoonLabel);

    DDX_Text(pDX, IDC_STATIC_SUNRISE_LABEL, m_SunRiseLabelText);
    DDX_Text(pDX, IDC_STATIC_SUNSET_LABEL, m_SunSetLabelText);
    DDX_Text(pDX, IDC_STATIC_MOONRISE_LABEL, m_MoonRiseLabelText);
    DDX_Text(pDX, IDC_STATIC_MOONSET_LABEL, m_MoonSetLabelText);
    DDX_Text(pDX, IDC_VALUE_DAYLENGTH, m_DayLengthValue);
    DDX_Control(pDX, IDC_BUTTON_TODAY, m_TodayButton);
    DDX_Text(pDX, IDC_LABEL_DAYLENGTH, m_DayLengthLabelText);
    DDX_Text(pDX, IDC_LABEL_TWILIGHT, m_TwilightLabelText);
    DDX_Text(pDX, IDC_LABEL_TWILIGHT_ASTRONOMICAL, m_TwilightAstronomicalLabelText);
    DDX_Text(pDX, IDC_LABEL_TWILIGHT_NAUT, m_TwilightNauticalLabelText);
    DDX_Text(pDX, IDC_LABEL_TWILIGHT_CIVIL, m_TwilightCivilLabelText);
    DDX_Text(pDX, IDC_LABEL_MOON_VISIBILITY, m_MoonVisibilityLabelText);
    DDX_Text(pDX, IDC_LABEL_MOON_PHASE, m_MoonPhaseLabelText);
    DDX_Text(pDX, IDC_LABEL_NEXT_NEW_MOON, m_NextNewMoonLabelText);
    DDX_Text(pDX, IDC_LABEL_NEXT_FULL_MOON, m_NextFullMoonLabelText);
    DDX_Text(pDX, IDC_STATIC_CONTINENT, m_ContinentLabelText);
    DDX_Text(pDX, IDC_STATIC_COUNTRY, m_CountryLabelText);
    DDX_Text(pDX, IDC_STATIC_CITY, m_CityLabelText);
    DDX_Text(pDX, IDC_STATIC_STATE, m_StateLabelText);
    DDX_Text(pDX, IDC_STATIC_LATITUDE, m_LatitudeLabelText);
    DDX_Text(pDX, IDC_STATIC_LONGITUDE, m_LongitudeLabelText);
    DDX_Control(pDX, IDC_BUTTON_UPDATE_CHECK, m_UpdateButton);
    DDX_Control(pDX, IDC_BUTTON_ABOUT, m_AboutButton);
    DDX_Text(pDX, IDC_LABEL_SUNRISE_AZIMUTH, m_SunRiseAzimuthLabelText);
    DDX_Text(pDX, IDC_LABEL_SUNSET_AZIMUTH, m_SunSetAzimuthLabelText);
    DDX_Text(pDX, IDC_VALUE_SUNRISE_AZIMUTH, m_SunRiseAzimuth);
    DDX_Text(pDX, IDC_VALUE_SUNSET_AZIMUTH, m_SunSetAzimuth);
    DDX_Text(pDX, IDC_LABEL_MOONRISE_AZIMUTH, m_MoonRiseAzimuthLabelText);
    DDX_Text(pDX, IDC_LABEL_MOONSET_AZIMUTH, m_MoonSetAzimuthLabelText);
    DDX_Text(pDX, IDC_VALUE_MOONRISE_AZIMUTH, m_MoonRiseAzimuth);
    DDX_Text(pDX, IDC_VALUE_MOONSET_AZIMUTH, m_MoonSetAzimuth);
    DDX_Control(pDX, IDC_VALUE_NEXT_NEW_MOON, m_NextNewMoonValue);
    DDX_Control(pDX, IDC_VALUE_NEXT_FULL_MOON, m_NextFullMoonValue);
    DDX_Text(pDX, IDC_VALUE_TIME_EQUATION, m_SunTimeEquation);
    DDX_Text(pDX, IDC_LABEL_TIME_EQUATION, m_SunTimeEquationLabelText);
    DDX_Text(pDX, IDC_VALUE_SUN_DECLINATION, m_SunDeclination);
    DDX_Text(pDX, IDC_LABEL_SUN_DECLINATION, m_SunDeclinationLabelText);
    DDX_Text(pDX, IDC_VALUE_MOON_DECLINATION, m_MoonDeclination);
    DDX_Text(pDX, IDC_LABEL_MOON_DECLINATION, m_MoonDeclinationLabelText);
    DDX_Text(pDX, IDC_LABEL_LUNASOLCAL_MOBILE, m_LunaSolCal_Mobile_LabelText);
}

BEGIN_MESSAGE_MAP(CLunaSolDlg, CDialog)
	ON_WM_SYSCOMMAND( )
	ON_WM_PAINT( )
	ON_WM_QUERYDRAGICON( )
	ON_WM_CTLCOLOR( )
	ON_WM_CLOSE( )
    ON_WM_TIMER( )
	ON_WM_WINDOWPOSCHANGED( )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CLunaSolDlg::OnBnClickedButtonAbout)
    ON_CBN_SELCHANGE(IDC_COMBO_CONTINENT, &CLunaSolDlg::OnCbnSelchangeComboContinent)
    ON_CBN_SELCHANGE(IDC_COMBO_COUNTRY, &CLunaSolDlg::OnCbnSelchangeComboCountry)
    ON_CBN_SELCHANGE(IDC_COMBO_CITY, &CLunaSolDlg::OnCbnSelchangeComboCity)
    ON_CBN_SELCHANGE(IDC_COMBO_STATE, &CLunaSolDlg::OnCbnSelchangeComboState)
    ON_CBN_SELCHANGE(IDC_COMBO_TIME_ZONE, &CLunaSolDlg::OnCbnSelchangeComboTimeZone)
    ON_BN_CLICKED(IDC_BUTTON_UPDATE_CHECK, &CLunaSolDlg::OnBnClickedButtonUpdateCheck)
	ON_EN_CHANGE(IDC_EDIT_LAT, &CLunaSolDlg::OnEnChangeEditLat)
	ON_EN_KILLFOCUS(IDC_EDIT_LAT, &CLunaSolDlg::OnEnKillFocusEditLat)
	ON_EN_CHANGE(IDC_EDIT_LAT2, &CLunaSolDlg::OnEnChangeEditLon)
	ON_EN_KILLFOCUS(IDC_EDIT_LAT2, &CLunaSolDlg::OnEnKillFocusEditLon)
    ON_NOTIFY(DTN_DATETIMECHANGE, IDC_DATETIMEPICKER, &CLunaSolDlg::OnDtnDatetimechangeDatetimepicker)
    ON_BN_CLICKED(IDC_BUTTON_PREV, &CLunaSolDlg::OnBnClickedButtonPrev)
    ON_BN_CLICKED(IDC_BUTTON_NEXT, &CLunaSolDlg::OnBnClickedButtonNext)
    ON_BN_CLICKED(IDC_BUTTON_TODAY, &CLunaSolDlg::OnBnClickedButtonToday)
    ON_BN_CLICKED(IDC_BUTTON_IPHONE, &CLunaSolDlg::OnBnClickedButtonIphone)
    ON_BN_CLICKED(IDC_BUTTON_ANDROID, &CLunaSolDlg::OnBnClickedButtonAndroid)
    ON_BN_CLICKED(IDC_BUTTON_WP, &CLunaSolDlg::OnBnClickedButtonWp)
END_MESSAGE_MAP( )


// CLunaSolDlg message handlers

BOOL CLunaSolDlg::OnInitDialog( )
{
	CDialog::OnInitDialog( );
    SetLocale( );

    TCHAR appPath[MAX_PATH];
    GetModuleFileName( NULL, appPath, MAX_PATH );
    CString path = appPath;
    int lastBackSlash = path.ReverseFind( '\\' );
    m_AppPath = path.Left( lastBackSlash );
    
    const CString szAppName = CUSTOM_APPLICATION_NAME;
    const CString szVersionNumber = theApp.GetVersionString( );
    CString szVersion;
    szVersion.LoadString( IDS_VERSION );

    CString szWindowTitle = szAppName + _T(" - ") + szVersion + _T(" ") + szVersionNumber + _T(" ") + CUSTOM_BETA;
    SetWindowTextW( szWindowTitle );

    const long lWindowPosLeft   = g_Config.GetDWORDValue( KEY_LUNASOLCAL_WIN_POS_LEFT );
    const long lWindowPosTop    = g_Config.GetDWORDValue( KEY_LUNASOLCAL_WIN_POS_TOP );
    const long lWindowPosRight  = g_Config.GetDWORDValue( KEY_LUNASOLCAL_WIN_POS_RIGHT );
    const long lWindowPosBottom = g_Config.GetDWORDValue( KEY_LUNASOLCAL_WIN_POS_BOTTOM );

    if ( !(( lWindowPosLeft == 0 ) && ( lWindowPosTop == 0 ) &&
           ( lWindowPosRight == 0 ) && ( lWindowPosBottom == 0 )))
    {
        SetWindowPos( &wndTop, lWindowPosLeft, lWindowPosTop, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE ); 
    }

    Localize( );

    m_BigLabelFont.CreateFont(20,0,0,0,FW_BOLD,FALSE,FALSE,0,
        DEFAULT_CHARSET, OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH,NULL);

    m_SunRiseLabel.SetFont( &m_BigLabelFont, TRUE );  
    m_SunSetLabel.SetFont( &m_BigLabelFont, TRUE );  
    m_MoonRiseLabel.SetFont( &m_BigLabelFont, TRUE );  
    m_MoonSetLabel.SetFont( &m_BigLabelFont, TRUE );  
    m_SunRiseValue.SetFont( &m_BigLabelFont, TRUE );  
    m_SunSetValue.SetFont( &m_BigLabelFont, TRUE );  
    m_MoonRiseValue.SetFont( &m_BigLabelFont, TRUE );  
    m_MoonSetValue.SetFont( &m_BigLabelFont, TRUE );  

	m_CityLabel.GetWindowRect( &m_CityLabelRect );
    m_CityList.GetWindowRect( &m_CityListRect );
    m_StateLabel.GetWindowRect( &m_StateLabelRect );
    m_StateList.GetWindowRect( &m_StateListRect );

    m_MoonRiseLabel.GetWindowRect( &m_MoonRiseLabelRect );
    m_MoonRiseValue.GetWindowRect( &m_MoonRiseValueRect );
    m_MoonRiseAzimuthLabel.GetWindowRect( &m_MoonRiseAzimuthLabelRect );
    m_MoonRiseAzimuthValue.GetWindowRect( &m_MoonRiseAzimuthValueRect );
    m_MoonSetLabel.GetWindowRect( &m_MoonSetLabelRect );
    m_MoonSetValue.GetWindowRect( &m_MoonSetValueRect );
    m_MoonSetAzimuthLabel.GetWindowRect( &m_MoonSetAzimuthLabelRect );
    m_MoonSetAzimuthValue.GetWindowRect( &m_MoonSetAzimuthValueRect );
    m_NextFullMoonLabel.GetWindowRect( &m_NextFullMoonLabelRect );
    m_NextFullMoonValue.GetWindowRect( &m_NextFullMoonValueRect );
    m_NextNewMoonLabel.GetWindowRect( &m_NextNewMoonLabelRect );
    m_NextNewMoonValue.GetWindowRect( &m_NextNewMoonValueRect );

    m_UpdateManager.Initialize( VERSIONCHECK_URL, L"lunasolcal", std::wstring( szVersionNumber ) );
    m_TimeZoneManager.Initialize( );

	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // color white brush

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_TheDate = CTime::GetCurrentTime( );

    // Fill time zone list
    TIME_ZONE_INFORMATION tz_info;
    GetTimeZoneInformation( &tz_info );
    CString currentTZStandard( tz_info.StandardName );
    CString currentTZDaylight( tz_info.DaylightName );

    int currentTimeZoneIdx = -1;

    CPtrList *timeZones = m_TimeZoneManager.GetTimeZones( );
    POSITION pos = timeZones->GetHeadPosition( );

    while ( pos != NULL )
    {
        TimeZoneManager::TimeZoneDescription *c = 
            (TimeZoneManager::TimeZoneDescription *) timeZones->GetNext( pos );

        const int idx = m_TimeZoneList.AddString( c->szDisplay );
        m_TimeZoneList.SetItemData( idx, (DWORD_PTR)c );

        if ( c->szStandard == currentTZStandard )
        {
            currentTimeZoneIdx = idx;
        }
    }

    int nSelectedTimeZoneIdx = g_Config.GetDWORDValue( KEY_LUNASOLCAL_SELECTED_TIMEZONE );
    if ( nSelectedTimeZoneIdx == -1 )
    {
        nSelectedTimeZoneIdx = currentTimeZoneIdx;
        if ( nSelectedTimeZoneIdx == -1 )
        {
            nSelectedTimeZoneIdx = 0;
        }
    }

    m_TimeZoneList.SetCurSel( nSelectedTimeZoneIdx );

    m_nSelectedContinentIdx = g_Config.GetDWORDValue( KEY_LUNASOLCAL_SELECTED_CONTINENT );
    m_nSelectedCountryIdx = g_Config.GetDWORDValue( KEY_LUNASOLCAL_SELECTED_COUNTRY );
    m_nSelectedStateIdx = g_Config.GetDWORDValue( KEY_LUNASOLCAL_SELECTED_STATE );
    m_nSelectedCityIdx = g_Config.GetDWORDValue( KEY_LUNASOLCAL_SELECTED_CITY );

    if ( m_nSelectedContinentIdx < 0 ) m_nSelectedContinentIdx = 0;
    if ( m_nSelectedCountryIdx < 0 ) m_nSelectedCountryIdx = 0;
    if ( m_nSelectedStateIdx < 0 ) m_nSelectedStateIdx = 0;
    if ( m_nSelectedCityIdx < 0 ) m_nSelectedCityIdx = 0;

    // Fill continent list
    CString szContinent;
    const unsigned int nContinents = sizeof( m_Continents ) / sizeof( ContinentDef );
    for ( unsigned int i = 0; i < nContinents; i++ )
    {
         szContinent.LoadStringW( m_Continents[ i ].nameResID );
         const int idx = m_ContinentList.AddString( szContinent );
         m_ContinentList.SetItemData( idx, (DWORD_PTR)&m_Continents[ i ] );
    }

    m_ContinentList.SetCurSel( m_nSelectedContinentIdx );
    
    FillCountryList( );

    m_CityList.SetCurSel( m_nSelectedCityIdx );
    UpdatePosition( );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

HBRUSH CLunaSolDlg::OnCtlColor(CDC* /* pDC */, CWnd* /* pWnd */, UINT /* nCtlColor */)
{
	return m_brush;
}

void CLunaSolDlg::OnWindowPosChanged( WINDOWPOS* /* lpwndpos */)
{
	m_CityLabel.GetWindowRect( &m_CityLabelRect );
    m_CityList.GetWindowRect( &m_CityListRect );
    m_StateLabel.GetWindowRect( &m_StateLabelRect );
    m_StateList.GetWindowRect( &m_StateListRect );

    if ( m_IsMoonRiseSetSwapped )
    {
        m_MoonRiseLabel.GetWindowRect( &m_MoonSetLabelRect );
        m_MoonRiseValue.GetWindowRect( &m_MoonSetValueRect );
        m_MoonRiseAzimuthLabel.GetWindowRect( &m_MoonSetAzimuthLabelRect );
        m_MoonRiseAzimuthValue.GetWindowRect( &m_MoonSetAzimuthValueRect );
        m_MoonSetLabel.GetWindowRect( &m_MoonRiseLabelRect );
        m_MoonSetValue.GetWindowRect( &m_MoonRiseValueRect );
        m_MoonSetAzimuthLabel.GetWindowRect( &m_MoonRiseAzimuthLabelRect );
        m_MoonSetAzimuthValue.GetWindowRect( &m_MoonRiseAzimuthValueRect );
    }
    else
    {
        m_MoonRiseLabel.GetWindowRect( &m_MoonRiseLabelRect );
        m_MoonRiseValue.GetWindowRect( &m_MoonRiseValueRect );
        m_MoonRiseAzimuthLabel.GetWindowRect( &m_MoonRiseAzimuthLabelRect );
        m_MoonRiseAzimuthValue.GetWindowRect( &m_MoonRiseAzimuthValueRect );
        m_MoonSetLabel.GetWindowRect( &m_MoonSetLabelRect );
        m_MoonSetValue.GetWindowRect( &m_MoonSetValueRect );
        m_MoonSetAzimuthLabel.GetWindowRect( &m_MoonSetAzimuthLabelRect );
        m_MoonSetAzimuthValue.GetWindowRect( &m_MoonSetAzimuthValueRect );
    }

    if ( m_IsNextFullMoonSwapped )
    {
        m_NextFullMoonLabel.GetWindowRect( &m_NextNewMoonLabelRect );
        m_NextFullMoonValue.GetWindowRect( &m_NextNewMoonValueRect );
        m_NextNewMoonLabel.GetWindowRect( &m_NextFullMoonLabelRect );
        m_NextNewMoonValue.GetWindowRect( &m_NextFullMoonValueRect );
    }
    else
    {
        m_NextFullMoonLabel.GetWindowRect( &m_NextFullMoonLabelRect );
        m_NextFullMoonValue.GetWindowRect( &m_NextFullMoonValueRect );
        m_NextNewMoonLabel.GetWindowRect( &m_NextNewMoonLabelRect );
        m_NextNewMoonValue.GetWindowRect( &m_NextNewMoonValueRect );
    }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLunaSolDlg::OnPaint( )
{
	if (IsIconic( ))
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc( )), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width( ) - cxIcon + 1) / 2;
		int y = (rect.Height( ) - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint( );
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLunaSolDlg::OnQueryDragIcon( )
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLunaSolDlg::SetLocale( )
{
	const int STR_SZ = 1024;
	TCHAR szBuf[STR_SZ] = { 0 };
	TCHAR szLocale[STR_SZ] = { 0 };

	LANGID uiLangId = GetUserDefaultUILanguage();
	
	::GetLocaleInfo(uiLangId, LOCALE_SENGLANGUAGE, szBuf, STR_SZ);
	::_tcscpy(szLocale, szBuf);
	::GetLocaleInfo(uiLangId, LOCALE_SENGCOUNTRY, szBuf, STR_SZ);
	if (::_tcsclen(szBuf) != 0){
		::_tcscat(szLocale, _T("_"));
		::_tcscat(szLocale, szBuf);
	}
	::GetLocaleInfo(uiLangId, LOCALE_IDEFAULTANSICODEPAGE, szBuf, STR_SZ);
	if (::_tcsclen(szBuf) != 0){
		::_tcscat(szLocale, _T("."));
		::_tcscat(szLocale, szBuf);
	}

	_tsetlocale(LC_ALL, szLocale);


    ::GetLocaleInfo(uiLangId, LOCALE_SISO639LANGNAME, szBuf, STR_SZ);
    m_Language = szBuf;

    if (( m_Language != _T("en")) && (m_Language != _T("de" )) && 
        ( m_Language != _T("es")) && (m_Language != _T("fr" )) && 
		(m_Language != _T("el")))
    {
        m_Language = _T("en");
    }
}

void CLunaSolDlg::UpdateInfo( )
{
	m_Date = m_TheDate.Format("%x");

	// Update calculated dates...
	SYSTEMTIME sysDate;
	
	if ( m_TheDate.GetAsSystemTime( sysDate ) )
	{
        sysDate.wMilliseconds = 0;
		sysDate.wSecond = 0;
        sysDate.wMinute = 0;
		sysDate.wHour = 0;

        double utcDiff = GetUTCDiff(GetSelectedTimeZoneInformation( ));

        if ( m_Calculator.calc( m_LatitudeValue, m_LongitudeValue, sysDate, utcDiff ) )
        {
            m_SunRise = m_Calculator.sunriseValid( ) ? FormatTime( m_Calculator.sunrise( ) ) : _T("-");
			
            CelestialCalculator::Date sunRiseDate = DoubleTime2UTCDate( m_Calculator.sunrise( ) );
            CelestialCalculator::CelestialObjectPosition sunRisePos;
            CelestialCalculator::calcSunCelestialObjectPosition( sunRiseDate, m_LatitudeValue, m_LongitudeValue, sunRisePos );
            m_SunRiseAzimuth.Format( _T("%.0f°"), sunRisePos.azimuth );

            m_SunSet = m_Calculator.sunsetValid( ) ? FormatTime( m_Calculator.sunset( ) ) : _T("-");
			
            CelestialCalculator::Date sunSetDate =  DoubleTime2UTCDate( m_Calculator.sunset( ) );
            CelestialCalculator::CelestialObjectPosition sunSetPos;
            CelestialCalculator::calcSunCelestialObjectPosition( sunSetDate, m_LatitudeValue, m_LongitudeValue, sunSetPos );
            m_SunSetAzimuth.Format( _T("%.0f°"), sunSetPos.azimuth );
            
			double sunNoon = m_Calculator.suntransit( );
			m_SunNoon = m_Calculator.suntransitValid( ) ? FormatTime( sunNoon ) : _T("-");
			
            CelestialCalculator::Date sunNoonDate = DoubleTime2UTCDate( sunNoon );
            CelestialCalculator::CelestialObjectPosition sunNoonPos;
            CelestialCalculator::calcSunCelestialObjectPosition( sunNoonDate, m_LatitudeValue, m_LongitudeValue, sunNoonPos );
            m_SunDeclination.Format( _T("%.1f°"), sunNoonPos.decl );
            
            // Zeitgleichung
            m_SunTimeEquation.Format( _T("%.2f min"), m_Calculator.timeEquation() );

            CString sunNoonAltitude;
            sunNoonAltitude. Format(_T(" (%.1f°)"), sunNoonPos.altitude );
            m_SunNoon += sunNoonAltitude;

            m_MoonRise = m_Calculator.moonriseValid( ) ? FormatTime( m_Calculator.moonrise( ) ) : _T("-");

            CelestialCalculator::Date moonRiseDate = DoubleTime2UTCDate( m_Calculator.moonrise( ) );
            CelestialCalculator::CelestialObjectPosition moonRisePos;
            CelestialCalculator::calcMoonCelestialObjectPosition( moonRiseDate, m_LatitudeValue, m_LongitudeValue, moonRisePos );
            if ( m_Calculator.moonriseValid( ) )
            {
                m_MoonRiseAzimuth.Format( _T("%.0f°"), moonRisePos.azimuth );
            }
            else
            {
                m_MoonRiseAzimuth = _T("-");
            }

            m_MoonSet = m_Calculator.moonsetValid( ) ? FormatTime( m_Calculator.moonset( ) ) : _T("-");

            CelestialCalculator::Date moonSetDate =  DoubleTime2UTCDate( m_Calculator.moonset( ) );
            CelestialCalculator::CelestialObjectPosition moonSetPos;
            CelestialCalculator::calcMoonCelestialObjectPosition( moonSetDate, m_LatitudeValue, m_LongitudeValue, moonSetPos );
            if ( m_Calculator.moonsetValid( ) )
            {
                m_MoonSetAzimuth.Format( _T("%.0f°"), moonSetPos.azimuth );
            }
            else
            {
                m_MoonSetAzimuth = _T("-");
            }

            m_MoonNoon = m_Calculator.moontransitValid( ) ? FormatTime( m_Calculator.moontransit( ) ) : _T("-");

            if ( m_Calculator.moontransitValid( ) )
            {
                CelestialCalculator::Date moonNoonDate = DoubleTime2UTCDate( m_Calculator.moontransit( ) );
                CelestialCalculator::CelestialObjectPosition moonNoonPos;
                CelestialCalculator::calcMoonCelestialObjectPosition( moonNoonDate, m_LatitudeValue, m_LongitudeValue, moonNoonPos );
                CString moonNoonAltitude;
                moonNoonAltitude. Format(_T(" (%.1f°)"), moonNoonPos.altitude );
                m_MoonNoon += moonNoonAltitude;

                m_MoonDeclination.Format( _T("%.1f°"), moonNoonPos.decl );
            }
            else
            {
                m_MoonDeclination = _T("-");
            }

            bool moonRiseBeforeMoonSet = IsBefore( 
                m_Calculator.moonriseValid( ), m_Calculator.moonrise( ),
                m_Calculator.moonsetValid( ), m_Calculator.moonset( ) );
            ArrangeMoonRiseMoonSet( moonRiseBeforeMoonSet );
            
            double moonVisibility = m_Calculator.moonvisibility( );
            m_MoonVisibility.Format( _T("%d%%"), (int)((moonVisibility * 100.0) + 0.5));

            unsigned short phaseIdx = m_Calculator.moonphaseIdx( );

            if ( phaseIdx == 8 ) phaseIdx = 0;

            int bitmapId = IDB_BITMAP_MPH1 + phaseIdx;
            if (( phaseIdx > 0 ) && ( m_LatitudeValue < 0 ))
            {
                // southern hemisphere
                bitmapId = IDB_BITMAP_MPH_SH2 - 1 + phaseIdx;
            }

            HBITMAP hBitmap = LoadBitmap( AfxGetInstanceHandle( ),MAKEINTRESOURCE( bitmapId ));
            HBITMAP prevBitmap = m_MoonPhase.SetBitmap( hBitmap );
            if ( prevBitmap )
            {
                DeleteObject( prevBitmap );
            }

            m_MoonPhaseName.LoadString( IDS_MOONPHASE_NEWMOON + phaseIdx );

            m_DayLengthValue = GetDayLengthString( m_Calculator.dayLength( ) );
			
            m_TwilightAstronStartValue = m_Calculator.astronomicalTwilightValid( ) ? FormatTime( m_Calculator.astronomicalTwilightMorning( ) ) : _T("-");
            m_TwilightAstronEndValue = m_Calculator.astronomicalTwilightValid( ) ? FormatTime( m_Calculator.astronomicalTwilightEvening( ) ) : _T("-");
            m_TwilightNautStartValue = m_Calculator.nauticalTwilightValid( ) ? FormatTime( m_Calculator.nauticalTwilightMorning( ) ) : _T("-");
            m_TwilightNautEndValue = m_Calculator.nauticalTwilightValid( ) ? FormatTime( m_Calculator.nauticalTwilightEvening( ) ) : _T("-");
            m_TwilightCivilStartValue = m_Calculator.civilTwilightValid( ) ? FormatTime( m_Calculator.civilTwilightMorning( ) ) : _T("-");
            m_TwilightCivilEndValue = m_Calculator.civilTwilightValid( ) ? FormatTime( m_Calculator.civilTwilightEvening( ) ) : _T("-");
        }

        CelestialCalculator::Date d = { sysDate.wYear, sysDate.wMonth, sysDate.wDay };
        CelestialCalculator::Date nextFullMoon;
        CelestialCalculator::Date nextNewMoon;
        
        calcNextFullNewMoonDates( d, nextFullMoon, nextNewMoon );
        SYSTEMTIME sysNextFullMoon = Date2SystemTimeLocal( nextFullMoon);
        SYSTEMTIME sysNextNewMoon = Date2SystemTimeLocal( nextNewMoon);

        m_NextNewMoonValueText = FormatDate( sysNextNewMoon );
        m_NextFullMoonValueText = FormatDate( sysNextFullMoon );

        if ( CelestialCalculator::isLater( nextFullMoon, nextNewMoon ))
        {
            MoveLabel( m_NextFullMoonLabel, m_NextNewMoonLabelRect, m_NextFullMoonLabelRect );
            MoveLabel( m_NextFullMoonValue, m_NextNewMoonValueRect, m_NextFullMoonValueRect );

            MoveLabel( m_NextNewMoonLabel, m_NextFullMoonLabelRect, m_NextNewMoonLabelRect );
            MoveLabel( m_NextNewMoonValue, m_NextFullMoonValueRect, m_NextNewMoonValueRect );

            m_IsNextFullMoonSwapped = true;
        }
        else
        {
            MoveLabel( m_NextFullMoonLabel, m_NextFullMoonLabelRect, m_NextFullMoonLabelRect );
            MoveLabel( m_NextFullMoonValue, m_NextFullMoonValueRect, m_NextFullMoonValueRect );

            MoveLabel( m_NextNewMoonLabel, m_NextNewMoonLabelRect, m_NextNewMoonLabelRect );
            MoveLabel( m_NextNewMoonValue, m_NextNewMoonValueRect, m_NextNewMoonValueRect );

            m_IsNextFullMoonSwapped = false;
        }
    }

    UpdateSelectedTimeZoneLabel( );
	UpdateData( FALSE );
}

CString CLunaSolDlg::FormatTime( double time2format )
{
    SYSTEMTIME localTime = DoubleTime2SystemTime( time2format );

	const int STR_SZ = 10;
	TCHAR szBuf[STR_SZ] = { 0 };
    ::GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_ITIME, szBuf, STR_SZ);

    CString fmtString = ( CString(szBuf) == _T("0") ) ? _T("%#I:%M %p") : _T("%#H:%M");
    return CTime( localTime ).Format(fmtString);
}

SYSTEMTIME CLunaSolDlg::DoubleTime2SystemTime( double time2convert )
{
    unsigned int h = abs((int)time2convert);
    double min = ( fabs(time2convert) - h ) * 60;

    if ( min >= 59.5 )
    {
        h++;
        min -= 60;
    }

    min += 0.5;    

    unsigned int d = h / 24;
    CTime ct = m_TheDate;

    if ( d > 0 )
    {
        ct += CTimeSpan( d, 0, 0, 0 );
        h = h % 24;
    }

	SYSTEMTIME localTime;
	ct.GetAsSystemTime( localTime );
    localTime.wMinute = (unsigned short)min;
    localTime.wHour = (unsigned short)h;
    localTime.wSecond = 0;
    localTime.wMilliseconds = 0;

    return localTime;
}

SYSTEMTIME CLunaSolDlg::DoubleTime2UTCSystemTime( double time2convert )
{
    unsigned short h = (unsigned short)time2convert;
    unsigned short min = (unsigned short)(( time2convert - h ) * 60.0);
    unsigned short sec = (unsigned short)(( time2convert - h ) * 3600.0 - min * 60.0);

	SYSTEMTIME localTime;
	m_TheDate.GetAsSystemTime( localTime );
    localTime.wHour = h;
    localTime.wMinute = min;
    localTime.wSecond = sec;
    localTime.wMilliseconds = 0;

    
    TIME_ZONE_INFORMATION tzInfo = GetSelectedTimeZoneInformation( );

    SYSTEMTIME utcTime;
    TzSpecificLocalTimeToSystemTime( &tzInfo, &localTime, &utcTime );

    return utcTime;
}

CelestialCalculator::Date CLunaSolDlg::DoubleTime2UTCDate( double time2convert )
{
    SYSTEMTIME utcTime = DoubleTime2UTCSystemTime( time2convert );
    CelestialCalculator::Date d = { utcTime.wYear, utcTime.wMonth, utcTime.wDay, utcTime.wHour, utcTime.wMinute, utcTime.wSecond };

    return d;
}

CString CLunaSolDlg::FormatDate( SYSTEMTIME& date )
{
    return CTime( date ).Format(_T("%x - %X" ));
}

void CLunaSolDlg::OnBnClickedButtonAbout( )
{
    const CString szAppName = CUSTOM_APPLICATION_NAME;
    const CString szVersionNumber = theApp.GetVersionString( );
    const CString szProductName = szAppName + _T(" - ") + szVersionNumber;

    CAboutDlg about;
    about.SetProductName( szProductName );
	about.DoModal( );
}

void CLunaSolDlg::OnClose( )
{
    SaveWindowPos( );

    g_Config.SetDWORDValue( KEY_LUNASOLCAL_SELECTED_CONTINENT, m_ContinentList.GetCurSel( ) );
    g_Config.SetDWORDValue( KEY_LUNASOLCAL_SELECTED_COUNTRY, m_CountryList.GetCurSel( ) );
    g_Config.SetDWORDValue( KEY_LUNASOLCAL_SELECTED_CITY, m_CityList.GetCurSel( ) );
    g_Config.SetDWORDValue( KEY_LUNASOLCAL_SELECTED_STATE, m_StateList.GetCurSel( ) );
    g_Config.SetDWORDValue( KEY_LUNASOLCAL_SELECTED_TIMEZONE, m_TimeZoneList.GetCurSel( ) );

    g_Config.SetStringValue( KEY_LUNASOLCAL_LATITUDE, m_Latitude );
    g_Config.SetStringValue( KEY_LUNASOLCAL_LONGITUDE, m_Longitude );

    ClearCountryComboBox( m_CountryList );
    ClearCountryComboBox( m_StateList );
    ClearCityComboBox( m_CityList );

    CDialog::OnClose( );
}

void CLunaSolDlg::SaveWindowPos( )
{
    if ( !IsIconic( ))
    {
        RECT rect;
        GetWindowRect( &rect );

        g_Config.SetDWORDValue( KEY_LUNASOLCAL_WIN_POS_LEFT, rect.left );
        g_Config.SetDWORDValue( KEY_LUNASOLCAL_WIN_POS_TOP, rect.top );
        g_Config.SetDWORDValue( KEY_LUNASOLCAL_WIN_POS_RIGHT, rect.right );
        g_Config.SetDWORDValue( KEY_LUNASOLCAL_WIN_POS_BOTTOM, rect.bottom );
    }
}

void CLunaSolDlg::OnCbnSelchangeComboContinent( )
{
    int idx = m_ContinentList.GetCurSel( );
    if ( idx != m_nSelectedContinentIdx )
    {
        m_nSelectedContinentIdx = idx;
        m_nSelectedCountryIdx = 0;
        m_nSelectedStateIdx = 0;
        m_nSelectedCityIdx = 0;

        FillCountryList( );
    }
}

void CLunaSolDlg::OnCbnSelchangeComboCountry( )
{
    m_nSelectedCountryIdx = m_CountryList.GetCurSel( );
    m_nSelectedStateIdx = 0;
    m_nSelectedCityIdx = 0;

    FillStateOrCity();
}

void CLunaSolDlg::FillStateOrCity( )
{
	DWORD_PTR country =  m_CountryList.GetItemData( m_nSelectedCountryIdx );
    if ( ( country != NULL ) && ( country != CB_ERR) )
    {
        if (((CountryDef *)country)->hasStates )
        {
            ShowStateList( true );
            FillStateList( );
        }
        else
        {
            ShowStateList( false );
            FillCityListForCountry( );
        }
    }
}

void CLunaSolDlg::OnCbnSelchangeComboState( )
{
    m_nSelectedStateIdx = m_StateList.GetCurSel( );
    m_nSelectedCityIdx = 0;

    FillCityListForState( );
}

void CLunaSolDlg::OnCbnSelchangeComboCity( )
{
    m_nSelectedCityIdx = m_CityList.GetCurSel( );
    UpdatePosition( );
}

void CLunaSolDlg::OnCbnSelchangeComboTimeZone( )
{
    UpdateSelectedTimeZoneLabel( );    
    UpdateInfo( );
}

void CLunaSolDlg::UpdatePosition( )
{
    const int nSel = m_CityList.GetCurSel( );
    if ( nSel >= 0 )
    {
        const CityDef *selectedCity = (CityDef *) m_CityList.GetItemData( nSel );

        if ( selectedCity )
        {
            m_Latitude.Format( L"%f", selectedCity->latitude );
            m_Longitude.Format( L"%f", selectedCity->longitude );

            m_LatitudeValue = selectedCity->latitude;
            m_LongitudeValue = selectedCity->longitude;
        }
        else
        {
            m_Latitude = g_Config.GetStringValue( KEY_LUNASOLCAL_LATITUDE );
            m_Longitude = g_Config.GetStringValue( KEY_LUNASOLCAL_LONGITUDE );

         	m_LatitudeValue = _wtof( m_Latitude );
            m_LongitudeValue = _wtof( m_Longitude );
        }

        UpdateInfo( );
    }

    UpdateData( FALSE );
}

void CLunaSolDlg::ShowStateList( bool show )
{
    m_StateLabel.ShowWindow( show ? SW_SHOW : SW_HIDE );
    m_StateList.ShowWindow( show ? SW_SHOW : SW_HIDE );

    if ( show )
    {
        MoveLabel( m_CityLabel, m_StateLabelRect, m_StateLabelRect );
        MoveLabel( m_CityList, m_StateListRect, m_StateListRect );
        MoveLabel( m_StateLabel, m_CityLabelRect, m_CityLabelRect );
        MoveLabel( m_StateList, m_CityListRect, m_CityListRect );
    }
    else
    {
        MoveLabel( m_CityLabel, m_CityLabelRect, m_CityLabelRect );
        MoveLabel( m_CityList, m_CityListRect, m_CityListRect );
        MoveLabel( m_StateLabel, m_StateLabelRect, m_StateLabelRect );
        MoveLabel( m_StateList, m_StateListRect, m_StateListRect );
     }
}

void CLunaSolDlg::UpdateSelectedTimeZoneLabel( )
{
    const int nSelectedTimeZoneIdx = m_TimeZoneList.GetCurSel( );
    const TimeZoneManager::TimeZoneDescription* tz = 
        (TimeZoneManager::TimeZoneDescription *) m_TimeZoneList.GetItemData( nSelectedTimeZoneIdx );

    if ( tz )
    {
        m_SelectedTimeZone = IsDaylightSaving( tz->tzInfo ) ? tz->szDaylight : tz->szStandard;
        UpdateData( FALSE );
    }
}

TIME_ZONE_INFORMATION CLunaSolDlg::GetSelectedTimeZoneInformation( )
{
    TIME_ZONE_INFORMATION tz_info;
    const int nSelectedTimeZoneIdx = m_TimeZoneList.GetCurSel( );
    const TimeZoneManager::TimeZoneDescription* tz = 
        (TimeZoneManager::TimeZoneDescription *) m_TimeZoneList.GetItemData( nSelectedTimeZoneIdx );

    if ( tz )
    {
        tz_info.Bias = tz->tzInfo.Bias;
        tz_info.DaylightBias = tz->tzInfo.DaylightBias;
        tz_info.StandardBias = tz->tzInfo.StandardBias;
        tz_info.DaylightDate = tz->tzInfo.DaylightDate;
        tz_info.StandardDate = tz->tzInfo.StandardDate;
    }
    else
    {
        GetTimeZoneInformation( &tz_info );
    }

    return tz_info;
}

double CLunaSolDlg::GetUTCDiff( const TIME_ZONE_INFORMATION& tzInfo )
{
    SYSTEMTIME utcTime;
    m_TheDate.GetAsSystemTime( utcTime );
    utcTime.wMilliseconds = 0;
    utcTime.wSecond = 0;
    utcTime.wMinute = 0;
    utcTime.wHour = 12;

    FILETIME ftUtcTime;
    SystemTimeToFileTime( &utcTime, &ftUtcTime );

    TIME_ZONE_INFORMATION sysTzInfo;
    sysTzInfo.Bias = tzInfo.Bias;
    sysTzInfo.DaylightBias = tzInfo.DaylightBias;
    sysTzInfo.StandardBias = tzInfo.StandardBias;
    sysTzInfo.DaylightDate = tzInfo.DaylightDate;
    sysTzInfo.StandardDate = tzInfo.StandardDate;

    SYSTEMTIME localTime;
    SystemTimeToTzSpecificLocalTime( &sysTzInfo, &utcTime, &localTime );

    FILETIME ftLocalTime;
    SystemTimeToFileTime( &localTime, &ftLocalTime );
  
    ULARGE_INTEGER ulLocalTime;
    ulLocalTime.LowPart = ftLocalTime.dwLowDateTime;
    ulLocalTime.HighPart = ftLocalTime.dwHighDateTime;

    ULARGE_INTEGER ulUtcTime;
    ulUtcTime.LowPart = ftUtcTime.dwLowDateTime;
    ulUtcTime.HighPart = ftUtcTime.dwHighDateTime;

    long double s1 = ulLocalTime.QuadPart / 3600.0 / 10000000.0;
    long double s2 = ulUtcTime.QuadPart / 3600.0 / 10000000.0;
    return (double)( s1 - s2 );
}

bool CLunaSolDlg::IsDaylightSaving( const TIME_ZONE_INFORMATION& tzInfo )
{
    return ( abs( GetUTCDiff( tzInfo ) ) == ( tzInfo.DaylightBias + tzInfo.Bias ) );
}

void CLunaSolDlg::OnBnClickedButtonUpdateCheck( )
{
    bool updateAvailable = false;
    std::wstring latestVersion;
    m_UpdateManager.DoUpdateCheck( updateAvailable, latestVersion );

    if ( updateAvailable )
    {
		UpdateDlg dlgUpdate;
        dlgUpdate.SetVersions( theApp.GetVersionString( ), 
                               CString( latestVersion.c_str( ) ) );
		if ( dlgUpdate.DoModal( ) == IDOK )
        {
            m_pUpdateDownloadDlg = new UpdateDownloadProgress( this );
            m_pUpdateDownloadDlg->Create( UpdateDownloadProgress::IDD, this );
            m_pUpdateDownloadDlg->ShowWindow( TRUE );

            if ( !m_UpdateManager.DoUpdate( m_pUpdateDownloadDlg ) )
            {
                CString errorMsg;
                errorMsg.FormatMessage( IDS_UPDATE_FAILED, m_UpdateManager.GetLastError( ) );
                MessageBox( errorMsg, CUSTOM_APPLICATION_NAME );
            }
            else
            {
                m_DownloadTimer = ::SetTimer( this->m_hWnd, m_DownloadTimerEventID, 100, NULL );
            }
        }
    }
    else
    {
        CString szMsg;
        szMsg.LoadString( IDS_UP_TO_DATE );

        MessageBox( szMsg, CUSTOM_APPLICATION_NAME, MB_OK );
    }
}

void CLunaSolDlg::OnTimer( UINT evt )
{
    if ( evt == m_DownloadTimerEventID )
    {
        if ( m_UpdateManager.IsInstallerAvailable( ) )
        {
            m_UpdateManager.RunInstaller( );
            PostMessage( WM_CLOSE );
        }
    }
}

void CLunaSolDlg::OnEnChangeEditLat( )
{
	m_CountryList.SetCurSel( m_CountryList.GetCount( )-1 );
	m_CityList.ResetContent( );
    m_CityList.InsertString( -1, L"-" );
    m_CityList.SetCurSel(0);
	m_StateList.ResetContent( );
    m_StateList.InsertString( -1, L"-" );
    m_StateList.SetCurSel(0);
}

void CLunaSolDlg::OnEnKillFocusEditLat( )
{
	UpdateData( );
	m_LatitudeValue = _wtof( m_Latitude );
    m_Latitude.Format( L"%g", m_LatitudeValue );
	UpdateInfo( );
}

void CLunaSolDlg::OnEnChangeEditLon( )
{
	m_CountryList.SetCurSel( m_CountryList.GetCount( )-1 );
	m_CityList.ResetContent( );
    m_CityList.InsertString( -1, L"-" );
    m_CityList.SetCurSel(0);
	m_StateList.ResetContent( );
    m_StateList.InsertString( -1, L"-" );
    m_StateList.SetCurSel(0);
}

void CLunaSolDlg::OnEnKillFocusEditLon( )
{
	UpdateData( );
	m_LongitudeValue = _wtof( m_Longitude );
    m_Longitude.Format( L"%g", m_LongitudeValue );
	UpdateInfo( );
}

BOOL CLunaSolDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if ((pMsg->wParam == VK_RETURN) || (pMsg->wParam == VK_ESCAPE))
        {
            pMsg->wParam = VK_TAB;
        }
    }
    return CDialog::PreTranslateMessage(pMsg);
}

void CLunaSolDlg::OnDtnDatetimechangeDatetimepicker(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
    *pResult = 0;

    if ( pDTChange )
	{
        m_TheDate = CTime( pDTChange->st );
		UpdateInfo( );
	}
}

void CLunaSolDlg::OnBnClickedButtonPrev( )
{
    NextPrevDate( false );
}

void CLunaSolDlg::OnBnClickedButtonNext( )
{
    NextPrevDate( true );
}

void CLunaSolDlg::NextPrevDate( bool next )
{
    CTimeSpan oneDay( 1, 0, 0, 0 );

    m_DateTimeControl.GetTime( m_TheDate );

    if ( next )
    {
        m_TheDate += oneDay;
    }
    else
    {
        m_TheDate -= oneDay;
    }

    m_DateTimeControl.SetTime( &m_TheDate );
    UpdateInfo( );
}

void CLunaSolDlg::OnBnClickedButtonToday( )
{
    SYSTEMTIME now;
    GetLocalTime( &now );

    m_DateTimeControl.SetTime( &now );

    m_TheDate = CTime( now );
    UpdateInfo( );
}

void CLunaSolDlg::FillCountryList( )
{
    ClearCountryComboBox( m_CountryList );

    ContinentDef *continent = (ContinentDef *) m_ContinentList.GetItemData( m_nSelectedContinentIdx );
    if ( continent )
    {
        TCHAR* fileNamePrefix = continent->fileNamePrefix;
        if ( fileNamePrefix )
        {
			CString filePath = CString(m_AppPath) + GetLocationFolderName( ) +
				fileNamePrefix + _T("_") + m_Language + _T(".txt");
            CStringArray lines;
            GetLinesFromFile( filePath, lines );

            const int n = (int) lines.GetCount( );
            for ( int i = 0; i < n; i++ )
            {
                CString line = lines.GetAt(i);

                int curPos = 0;
                CStringArray components;
                CString resToken= line.Tokenize( _T("|"), curPos );

                while (resToken != "")
                {
                    components.Add( resToken );
                    resToken= line.Tokenize( _T("|"), curPos );
                }

                CountryDef *country = new CountryDef;
                if ( components.GetCount( ) == 3 )
                {
                    CString countryName = components[0];
                    country->hasStates = ( components[1] == _T("1") );
                    country->filename = components[2];

                    const int idx = m_CountryList.AddString( countryName );
                    m_CountryList.SetItemData( idx, (DWORD_PTR)country );
                }
                else
                {
                    delete country;
                }
            }

            m_CountryList.SetCurSel( m_nSelectedCountryIdx );
            FillStateOrCity( );
        }
    }
}

void CLunaSolDlg::FillStateList( )
{
    ClearCountryComboBox( m_StateList );

    CountryDef *country = (CountryDef *) m_CountryList.GetItemData( m_nSelectedCountryIdx );
    if ( country )
    {
        CString filePath = CString( m_AppPath ) + GetLocationFolderName( ) + country->filename;

        CStringArray lines;
        GetLinesFromFile( filePath.TrimRight( ), lines );

        const int n = (int) lines.GetCount( );
        for ( int i = 0; i < n; i++ )
        {
            CString line = lines.GetAt(i);

            int curPos = 0;
            CStringArray components;
            CString resToken= line.Tokenize( _T("|"), curPos );

            while (resToken != "")
            {
                components.Add( resToken );
                resToken= line.Tokenize( _T("|"), curPos );
            }

            CountryDef *state = new CountryDef;
            if ( components.GetCount( ) == 2 )
            {
                CString stateName = components[0];
                state->filename = components[1];
                state->hasStates = false;

                const int idx = m_StateList.AddString( stateName );
                m_StateList.SetItemData( idx, (DWORD_PTR)state );
            }
            else
            {
                delete state;
            }
        }

        UINT nIDState = IDS_STATE;
        if ( filePath.Find( _T("germany") ) != -1 )
        {
            nIDState = IDS_STATE_GERMANY;
        }
        else
        {
            if ( filePath.Find( _T("france") ) != -1 )
            {
                nIDState = IDS_STATE_FRANCE;
            }
            else
            {
                if ( filePath.Find( _T("usa") ) != -1 )
                {
                    nIDState = IDS_STATE_USA;
                }
            }
        }

        LocalizeString( m_StateLabelText, nIDState );

        m_StateList.SetCurSel( m_nSelectedStateIdx );
        FillCityListForState( );
    }
}

void CLunaSolDlg::FillCityListForState( )
{
    CountryDef *country = (CountryDef *) m_StateList.GetItemData( m_nSelectedStateIdx );
    if ( country )
    {
        CString filePath = CString( m_AppPath ) + GetLocationFolderName( ) + country->filename;
        FillCityList( filePath.TrimRight( ));
    }
}

void CLunaSolDlg::FillCityListForCountry( )
{
    CountryDef *country = (CountryDef *) m_CountryList.GetItemData( m_nSelectedCountryIdx );
    if ( country )
    {
        CString filePath = CString( m_AppPath ) + GetLocationFolderName( ) + country->filename;
        FillCityList( filePath.TrimRight( ) );
    }
}

void CLunaSolDlg::FillCityList( const CString& filePath )
{
    ClearCityComboBox( m_CityList );

    CStringArray lines;
    GetLinesFromFile( filePath, lines );

    const int n = (int) lines.GetCount( );
    for ( int i = 0; i < n; i++ )
    {
        CString line = lines.GetAt(i);

        int curPos = 0;
        CStringArray components;
        CString resToken= line.Tokenize( _T("|"), curPos );

        while (resToken != "")
        {
            components.Add( resToken );
            resToken= line.Tokenize( _T("|"), curPos );
        }

        CityDef *city = new CityDef;
        if ( components.GetCount( ) == 3 )
        {
            _locale_t cLoc = _create_locale(LC_ALL, "C");

            CString cityName = components[0];
            city->latitude = _wtof_l( components[1], cLoc );
            city->longitude = _wtof_l( components[2], cLoc );

            const int idx = m_CityList.AddString( cityName );
            m_CityList.SetItemData( idx, (DWORD_PTR)city );
        }
    }

    m_CityList.SetCurSel( m_nSelectedCityIdx );
    OnCbnSelchangeComboCity( );
}

void CLunaSolDlg::ClearCountryComboBox( CComboBox& comboBox )
{
    const int n = comboBox.GetCount( );
    for ( int i = 0; i < n; i++ )
    {
        delete (CountryDef *)comboBox.GetItemData( i );
    }

    comboBox.ResetContent( );
}

void CLunaSolDlg::ClearCityComboBox( CComboBox& comboBox )
{
    const int n = comboBox.GetCount( );
    for ( int i = 0; i < n; i++ )
    {
        delete (CityDef *)comboBox.GetItemData( i );
    }

    comboBox.ResetContent( );
}

void CLunaSolDlg::GetLinesFromFile( const CString& filePath, CStringArray& lines )
{
	CFile file( filePath, CFile::modeRead );
    
    ULONGLONG  len = file.GetLength( );
    char *buf = (char *) malloc( (size_t)len );
    if ( buf )
    {
        file.Read( buf, (UINT)len );

        // file are encoded in UTF-8
        char *ptr = buf;

		// ignore BOM
		if ( ( buf[0] == (char)0xef ) &&
			 ( buf[1] == (char)0xbb ) &&
			 ( buf[2] == (char)0xbf ) )
		{
			ptr += 3;
			len -= 3;
		}

        char *lineStart = ptr;

		for ( int i = 0; i < len; i++, ptr++)
        {
            if ( *ptr == '\n' )
            {
                if ( ptr > lineStart )
                {
                    *ptr = '\0';

                    CString line = CA2W( lineStart, CP_UTF8 );

                    lines.Add( line );

                    lineStart = ptr + 1;
                }
            }
        }

        free( buf );
    }

    file.Close( );
}

/*
    Rise:        8  10  --  10  -- 
    Set:        10   8  10  --  --
    Before:      t   f   f   t   t
*/
bool CLunaSolDlg::IsBefore( bool riseValid, double rise, bool setValid, double set )
{
    bool before = true;
    
    if ( !riseValid )
    {
        before = !setValid;
    }
    else
    {
        if ( setValid )
        {
            before = ( set > rise ); 
        }
    }

    return before;
}

void CLunaSolDlg::MoveLabel( CWnd& label, RECT topLeft, RECT rect )
{
    POINT ptTopLeft = { topLeft.left, topLeft.top };
    ScreenToClient(&ptTopLeft);
    label.MoveWindow( ptTopLeft.x, ptTopLeft.y, 
        rect.right - rect.left, rect.bottom - rect.top );
}

void CLunaSolDlg::ArrangeMoonRiseMoonSet( bool moonRiseBeforeMoonSet )
{
    if ( moonRiseBeforeMoonSet )
    {
        MoveLabel( m_MoonRiseLabel, m_MoonRiseLabelRect, m_MoonRiseLabelRect );
        MoveLabel( m_MoonRiseValue, m_MoonRiseValueRect, m_MoonRiseValueRect );
        MoveLabel( m_MoonRiseAzimuthLabel, m_MoonRiseAzimuthLabelRect, m_MoonRiseAzimuthLabelRect );
        MoveLabel( m_MoonRiseAzimuthValue, m_MoonRiseAzimuthValueRect, m_MoonRiseAzimuthValueRect );
        MoveLabel( m_MoonSetLabel, m_MoonSetLabelRect, m_MoonSetLabelRect );
        MoveLabel( m_MoonSetValue, m_MoonSetValueRect, m_MoonSetValueRect );
        MoveLabel( m_MoonSetAzimuthLabel, m_MoonSetAzimuthLabelRect, m_MoonSetAzimuthLabelRect );
        MoveLabel( m_MoonSetAzimuthValue, m_MoonSetAzimuthValueRect, m_MoonSetAzimuthValueRect );
        
        m_IsMoonRiseSetSwapped = false;
    }
    else
    {
        MoveLabel( m_MoonRiseLabel, m_MoonSetLabelRect, m_MoonRiseLabelRect );
        MoveLabel( m_MoonRiseValue, m_MoonSetValueRect, m_MoonRiseValueRect );
        MoveLabel( m_MoonRiseAzimuthLabel, m_MoonSetAzimuthLabelRect, m_MoonRiseAzimuthLabelRect );
        MoveLabel( m_MoonRiseAzimuthValue, m_MoonSetAzimuthValueRect, m_MoonRiseAzimuthValueRect );
        MoveLabel( m_MoonSetLabel, m_MoonRiseLabelRect, m_MoonSetLabelRect );
        MoveLabel( m_MoonSetValue, m_MoonRiseValueRect, m_MoonSetValueRect );
        MoveLabel( m_MoonSetAzimuthLabel, m_MoonRiseAzimuthLabelRect, m_MoonSetAzimuthLabelRect );
        MoveLabel( m_MoonSetAzimuthValue, m_MoonRiseAzimuthValueRect, m_MoonSetAzimuthValueRect );
        
        m_IsMoonRiseSetSwapped = true;
    }
}

void CLunaSolDlg::Localize( )
{
	LocalizeString( m_DateLabelText, IDS_DATE_LABEL );
	LocalizeString( m_TimeZoneLabelText, IDS_TIMEZONE_LABEL );

    LocalizeButton( m_TodayButton, IDS_TODAY );

    LocalizeString( m_SunRiseLabelText, IDS_SUNRISE_LABEL );
	LocalizeString( m_SunSetLabelText, IDS_SUNSET_LABEL );
    LocalizeString( m_SunNoonLabelText, IDS_SUNNOON_LABEL );
    LocalizeString( m_SunTimeEquationLabelText, IDS_TIMEEQUATION_LABEL );
    LocalizeString( m_SunDeclinationLabelText, IDS_SUNDECLINATION_LABEL );
    LocalizeString( m_SunRiseAzimuthLabelText, IDS_AZIMUTH_LABEL );
    LocalizeString( m_SunSetAzimuthLabelText, IDS_AZIMUTH_LABEL );
    LocalizeString( m_MoonRiseLabelText, IDS_MOONRISE_LABEL );
	LocalizeString( m_MoonSetLabelText, IDS_MOONSET_LABEL );
    LocalizeString( m_MoonNoonLabelText, IDS_MOONNOON_LABEL );
    LocalizeString( m_MoonDeclinationLabelText, IDS_SUNDECLINATION_LABEL );
    LocalizeString( m_MoonRiseAzimuthLabelText, IDS_AZIMUTH_LABEL );
    LocalizeString( m_MoonSetAzimuthLabelText, IDS_AZIMUTH_LABEL );

    LocalizeString( m_DayLengthLabelText, IDS_DAYLENGTH );
    LocalizeString( m_TwilightLabelText, IDS_TWILIGHT );
    LocalizeString( m_TwilightAstronomicalLabelText, IDS_TWLIGHT_ASTRONOMICAL );
    LocalizeString( m_TwilightNauticalLabelText, IDS_TWLIGHT_NAUTICAL );
    LocalizeString( m_TwilightCivilLabelText, IDS_TWLIGHT_CIVIL );
    LocalizeString( m_MoonVisibilityLabelText, IDS_MOON_VISIBILITY );
    LocalizeString( m_MoonPhaseLabelText, IDS_MOON_PHASE );
    LocalizeString( m_NextNewMoonLabelText, IDS_NEXT_NEW_MOON );
    LocalizeString( m_NextFullMoonLabelText, IDS_NEXT_FULL_MOON );
    LocalizeString( m_ContinentLabelText, IDS_CONTINENT );
    LocalizeString( m_CountryLabelText, IDS_COUNTRY );
    LocalizeString( m_CityLabelText, IDS_CITY );
    LocalizeString( m_StateLabelText, IDS_STATE );
    LocalizeString( m_LatitudeLabelText, IDS_LATITUDE );
    LocalizeString( m_LongitudeLabelText, IDS_LONGITUDE );
    LocalizeString( m_LunaSolCal_Mobile_LabelText, IDS_LUNASOLCAL_MOBILE );

    LocalizeButton( m_UpdateButton, IDS_UPDATE_CHECK );
    LocalizeButton( m_AboutButton, IDS_ABOUT );
}

CString CLunaSolDlg::GetDayLengthString( double dayLength )
{
    int h = (int) dayLength;
    double min = ( dayLength - h ) * 60;
    if ( min >= 59.5 )
    {
        h++;
        min -= 59.5;
    }

    int sec = (int) ( ( min - (int) min ) * 60.0 + 0.5 );

    if ( sec >= 60 )
    {
        min++;
        sec--;
    }

    if ( min >= 60 )
    {
        h++;
        min--;
    }

    CString dayLengthStr;
    dayLengthStr.Format( _T("%2dh %02dm %02ds"), h, (int) min, sec );
    
    return dayLengthStr;
}


SYSTEMTIME CLunaSolDlg::Date2SystemTimeLocal( const CelestialCalculator::Date& d )
{
    SYSTEMTIME utcTime = { 0 };
    utcTime.wYear = d.year;
    utcTime.wMonth = d.month;
    utcTime.wDay = d.day;
    utcTime.wHour = d.hour;
    utcTime.wMinute = d.minute;
    utcTime.wSecond = d.second;

    SYSTEMTIME localTime = { 0 };
    TIME_ZONE_INFORMATION tzInfo = GetSelectedTimeZoneInformation( );
    SystemTimeToTzSpecificLocalTime( &tzInfo, &utcTime, &localTime );

    return localTime;
}


void CLunaSolDlg::OnBnClickedButtonIphone()
{
    ShowUrl( IDS_ITUNES_URL );
}

void CLunaSolDlg::OnBnClickedButtonAndroid()
{
    ShowUrl( IDS_ANDROID_MARKET_URL );
}

void CLunaSolDlg::OnBnClickedButtonWp()
{
    ShowUrl( IDS_WP_MARKET_URL );
}

void CLunaSolDlg::ShowUrl( const UINT urlId )
{
    CString url;
    url.LoadString( urlId );

    ShellExecute(NULL, _T("open"), url, NULL, NULL, SW_SHOWNORMAL);
}

CString CLunaSolDlg::GetLocationFolderName()
{
#ifdef DEBUG
	return _T("\\..\\Locations\\");
#else
	return _T("\\Location\\");
#endif

}
