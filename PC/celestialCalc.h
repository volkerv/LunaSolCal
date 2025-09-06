#pragma once

namespace CelestialCalculator
{
    class CelestialObjectRiseSet
    {
    public: 
        typedef enum 
        {
            Normal,
            RiseInvalid,
            SetInvalid,
            AllDayAboveHorizon,
            AllDayBelowHorizon
        } RiseSetType;

        double rise;
        double set;
        RiseSetType type;
    };

    // constants
    const double pi = 3.14159265358979323846;
    const double RADEG    =   (180.0 / pi);
    const double DEGRAD   =   (pi / 180.0);
    
    const double rads = 0.0174532925;
    const double p2 = 6.283185307;
    const double arc = 206264.8062;
    const double coseps = 0.91748;
    const double sineps = 0.39778;

    // functions
    double sind( double x );
    double cosd( double x );
    double tand( double x );
    double asind( double x );
    double acosd( double x );
    double atand( double x );
    double atan2d( double y, double x );

    // types
    typedef enum
    {
        SUN = 1,
        MOON
    } Object;

    typedef struct
    {
        unsigned short year;    // [0..
        unsigned short month;   // [1..12]
        unsigned short day;     // [1..31]
        unsigned short hour;    // [0..23]
        unsigned short minute;  // [0..59]
        unsigned short second;  // [0..59]
    } Date;

    typedef struct
    {
        double ra;      // Right Ascension
        double decl;    // Declination
        double r;       // Distance
        double L;       // Mean longitude
        double M;       // Mean anomaly
        double altitude;
        double azimuth;
    } CelestialObjectPosition;


    /**************************************************************************
     *
     * calcCelestialObjectPosition
     *
     *************************************************************************/

    bool calcCelestialObjectPosition( Object obj, const Date& date,
                            const double latitude, const double longitude, 
                            CelestialObjectPosition& objectPos );

    bool calcSunCelestialObjectPosition( const Date& date, 
                                         const double latitude, 
                                         const double longitude, 
                                         CelestialObjectPosition& objectPos );

    bool calcMoonCelestialObjectPosition( const Date& date, 
                                          const double latitude, 
                                          const double longitude, 
                                          CelestialObjectPosition& objectPos );

    double convertDateToDayNumber( const Date& date );


    /**************************************************************************
     *
     * Next full/new moon
     *
     *************************************************************************/
    bool calcNextFullNewMoonDates( const Date& baseDate, Date& nextFullMoonDate, Date& nextNewMoonDate );

    CelestialObjectRiseSet calcSunRiseSet(const Date& date, double lat, double lon);
    CelestialObjectRiseSet calcMoonRiseSet(const Date& date, double utcdiff, double lat, double lon);

    // ------------------------------------------------------------------------
    // Helpers  
    // ------------------------------------------------------------------------

    // return angle in range [0..360[
    double rev( double x );

    // Convert internal GMT date to Julian day and fraction.
    int jdate( int year, int month, int day );
 
    // Convert Julian date to year, month, day
    void jyear( double td, Date& d );

    // Convert Julian time to hour, minutes, and seconds
    void jhms( double j, Date& d );

    // Given a K value used to determine the mean phase of the new moon, and a phase
    // selector (0.0, 0.25, 0.5, 0.75), obtain the true, corrected phase time.
    double truephase( double k, double phase );
    
    // Calculates time of the mean new Moon for a given base date
    double meanphase( double sdate, double k );

    // Find time of new/full phases of the moon which surround the current date.  
    void phasehunt( double sdate, double phases[3] );

    // Returns true if d2 is later than d1
    bool isLater( const Date& d1, const Date& d2 );

    void findNextNewFullMoonDates( const Date& baseDate, double jdate_base, 
        bool& foundNewMoonDate, Date& nextNewMoonDate,
        bool& foundFullMoonDate, Date& nextFullMoonDate );

}
