/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   + LunaSolCal - (c) 2009 by Volker Voecking Software Engineering
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

#pragma once

class TimeZoneManager
{
public:
    TimeZoneManager( );
    ~TimeZoneManager( );

    bool Initialize( );

    typedef struct
    {
        LONG        Bias;
        LONG        StandardBias;
        LONG        DaylightBias;
        SYSTEMTIME  StandardDate;
        SYSTEMTIME  DaylightDate;
    } TimeZoneInfo;

    typedef struct
    {
        CString                 szDisplay;
        CString                 szDaylight;
        CString                 szStandard;
        TIME_ZONE_INFORMATION   tzInfo;
    } TimeZoneDescription;

    CPtrList* GetTimeZones( ) { return &m_TimeZones; }

private:
    CPtrList m_TimeZones;
};
