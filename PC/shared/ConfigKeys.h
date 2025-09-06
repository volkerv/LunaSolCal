/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once

// type defs
typedef struct 
{
    int nKeyID;
    int nLinkID;
    LPCTSTR pPath;
}	REG_CONFIG_HIERARCHY;

typedef struct 
{
    int nKeyID;
    int nLinkID;
    LPCTSTR pKeyName;
    int nKeyTyp;
    LPCTSTR pDefault;
    LPCTSTR pInfo;
}	REG_CONFIG_KEYDATA;

typedef struct
{
    DWORD dwSize;
    REG_CONFIG_HIERARCHY *pHierarchy;
    int nHierarchyCount;
    int nHierarchySize;
    REG_CONFIG_KEYDATA *pKeydata;
    int nKeydataCount;
    int nKeydataSize;
}	REG_CONFIG_QUERYDATA;

// DEFAULT PARAMETERS -----------------------------------------------------------------------------

#define	KDEF_NUM(nID)		(LPTSTR)((DWORD)(nID))	// Default cast for numeric keydata
#define KDEF_TRUE			KDEF_NUM(1)				// Default for BOOL key
#define KDEF_FALSE			KDEF_NUM(0)				// Default for BOOL key
#define KDEF_NONE			KDEF_NUM(0)				// No default, 0 is always the numeric default

// INTERNAL SUBTYPES - DO NOT USE DIRECTLY !!! ----------------------------------------------------
#define _KT_REGSZ			0x00100					// Basic string type bit
#define _KT_REGDW			0x00200					// Basic DWORD type bit
#define	_KT_BIN				0x00400					// Binary data

#define	_KTS_MASKSUB		0x000FF
#define	_KTS_MASKTYPE		0x0FFFF

#define	_KTS_STR			0						// Subtypes for generic _KT_REGSZ
#define	_KTS_FONT			1						// Stringtype Font				
#define	_KTS_RGB			2						// Stringtype RGB Color
#define	_KTS_DIR			3						// Stringtype Directory			
#define	_KTS_ILIST			4						// Stringtype INT List ("123,456,1,44,...")
#define	_KTS_RECT			5						// 
#define	_KTS_POINT			6						// 

#define	_KTS_DWORD			0						// Subtypes for generic _KT_REGDW
#define	_KTS_INT			1
#define	_KTS_BOOL			2

#define	_KTS_BIN			0						// Subtypes for binary types
#define	_KTS_DOUBLE			1						// Double


// KEY TYPE PARAMETERS FOR DATA DEFINITION --------------------------------------------------------

#define	KT_DEFUNI			0x10000					// Set UNICODE flag for default string entrys
#define KT_EXP				0x20000					// Expand-Bit for strings e.g. "%TEMP%\LOGFILE.LOG"

#define	KT_STR				(_KT_REGSZ|_KTS_STR)	// String data
#define	KT_FONT				(_KT_REGSZ|_KTS_FONT)	// Font definition 
#define	KT_RGB				(_KT_REGSZ|_KTS_RGB)	// RGB color definition "R,G,B"
#define	KT_DIR				(_KT_REGSZ|_KTS_DIR)	// Directory path terminated with "\"
#define	KT_ILIST			(_KT_REGSZ|_KTS_ILIST)	// INT List ("123,456,1,44,...")
#define	KT_RECT				(_KT_REGSZ|_KTS_RECT)	// INT List for RECT structure
#define	KT_POINT			(_KT_REGSZ|_KTS_POINT)	// INT List for POINT structure

#define	KT_DWORD			(_KT_REGDW|_KTS_DWORD)	// DWORD value
#define	KT_INT				(_KT_REGDW|_KTS_INT)	// Signed INT value 
#define	KT_BOOL				(_KT_REGDW|_KTS_BOOL)	// BOOL value, TRUE or FALSE

#define	KT_BIN				(_KT_BIN|_KTS_BIN)		// Simple binary field
#define	KT_DOUBLE			(_KT_BIN|_KTS_DOUBLE)	// DOUBLE value



// defines
#define KEY_NONE                                  0
#define KEY_COMPANY_ROOT                          1
 
// app settings
#define KEY_LUNASOLCAL_APP                        100
#define KEY_LUNASOLCAL_LOGGING                    200
#define KEY_LUNASOLCAL_GUI                        300

// app
#define KEY_LUNASOLCAL_SELECTED_CONTINENT         101
#define KEY_LUNASOLCAL_SELECTED_COUNTRY           102
#define KEY_LUNASOLCAL_SELECTED_CITY              103
#define KEY_LUNASOLCAL_SELECTED_STATE             104
#define KEY_LUNASOLCAL_SELECTED_TIMEZONE          105
#define KEY_LUNASOLCAL_LATITUDE                   106
#define KEY_LUNASOLCAL_LONGITUDE                  107

// logging
#define KEY_LUNASOLCAL_LOGFILENAME                201
#define KEY_LUNASOLCAL_LOGLEVEL                   202
#define KEY_LUNASOLCAL_MAXLOGFILESIZE             203
#define KEY_LUNASOLCAL_LISTLOADEDDLLS             204

// GUI
#define KEY_LUNASOLCAL_WIN_POS_LEFT               301
#define KEY_LUNASOLCAL_WIN_POS_TOP                302
#define KEY_LUNASOLCAL_WIN_POS_RIGHT              303
#define KEY_LUNASOLCAL_WIN_POS_BOTTOM             304
#define KEY_LUNASOLCAL_LANGUAGE_MARKER            305

