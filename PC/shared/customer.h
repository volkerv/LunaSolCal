/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once

// All defines, that may be customer dependant, go here

const TCHAR CUSTOM_REG_COMPANY_ROOT[] =             _T("Software\\VVSE");
const TCHAR CUSTOM_REG_LOGFILENAME[] =              _T("LunaSolCal.log");
const TCHAR CUSTOM_APPLICATION_NAME[] =             _T("LunaSolCal");
const TCHAR CUSTOM_APPLICATION_NAME_REG[] =         _T("LunaSolCal2");
const TCHAR CUSTOM_COMPANY_HOMEPAGE[] =             _T("https://www.vvse.com");
const TCHAR CUSTOM_OPENSTREETMAP_LICENSE[] =        _T("https://www.openstreetmap.org/copyright");
const TCHAR CUSTOM_SHOW_LICENSE_INFO[] =            _T("N");
const TCHAR CUSTOM_COMMENT[] =                      _T("");
const TCHAR CUSTOM_BETA[] =                         _T("");

#define _CHECK_LICENSE_STATE_                       
#define _CHECK_LICENSE_INFO_                       

#define CUSTOM_COMPANY_NAME                         "Volker Voecking\0"                
#define CUSTOM_LEGAL_COPYRIGHT                      "© 2018 by Volker Voecking Software Engineering\0"
#define CUSTOM_LEGAL_TRADEMARKS                     "\0"
#define CUSTOM_PRODUCT_NAME                         "LunaSolCal"

#define CUSTOM_MAIN_DESCR 						    "LunaSolCal"
#define CUSTOM_MAIN_RESOURCES_INTERNAL_NAME 		"LunaSolCal"
#define CUSTOM_MAIN_RESOURCES_ORIGINAL_NAME 		"LunaSolCal.exe"

#define CRC_OFFSET_APP                              4440
#define NOISE_BYTES_APP                             5200

#define CRC_OFFSET_RES                              220
#define NOISE_BYTES_RES                             280
