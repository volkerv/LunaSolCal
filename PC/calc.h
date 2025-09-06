#pragma once
#include "afxwin.h"
#include <math.h>
#include "celestialCalc.h"

class Calculator
{
public:
	Calculator( );

	bool calc( const double latitude, const double longitude, const SYSTEMTIME date, const double utcDiff );
	bool calcSunAltitudeAzimuth( const double latitude, const double longitude, 
		const SYSTEMTIME& date, double& altitude, double& azimuth );

    bool sunriseValid( ) { return mSun.riseValid; }
    double sunrise( ) { return mSun.rise; };
	bool sunsetValid( ) { return mSun.setValid; }
	double sunset( ) { return mSun.set; };
    bool suntransitValid( ) { return mSun.transitValid; }
    double suntransit( ) { return mSun.transit; };
    bool sundeclination( double& sd );
    bool moonriseValid( ) { return mMoon.riseValid; }
    double moonrise( ) { return mMoon.rise; };
	bool moonsetValid( ) { return mMoon.setValid; }
	double moonset( ) { return mMoon.set; };
    bool moontransitValid( ) { return mMoon.transitValid; }
    double moontransit( ) { return mMoon.transit; };
    bool moondeclination( double& md );
    unsigned short moonphaseIdx( ) { return (unsigned short)mMoon.phaseIdx; }
    double moonvisibility( ) { return mMoon.phase; }

    double dayLength();

    double astronomicalTwilightMorning() {return mSun.astronomicalTwilightMorning; };
    double astronomicalTwilightEvening() {return mSun.astronomicalTwilightEvening; };
    bool   astronomicalTwilightValid() {return mSun.astronomicalTwilightValid; };

    double nauticalTwilightMorning() {return mSun.nauticalTwilightMorning; };
    double nauticalTwilightEvening() {return mSun.nauticalTwilightEvening; };
    bool   nauticalTwilightValid() {return mSun.nauticalTwilightValid; };

    double civilTwilightMorning() {return mSun.civilTwilightMorning; };
    double civilTwilightEvening() {return mSun.civilTwilightEvening; };
    bool   civilTwilightValid() {return mSun.civilTwilightValid; };

    double timeEquation() { return 60 * mSun.timeEquation; }

	typedef struct {
        bool   valid;
		double rise;
        bool   riseValid;
		double transit;
        bool   transitValid;
        double timeEquation;
        double declination;
		double set;
        bool   setValid;
        int    phaseIdx;

        double phase;

        double astronomicalTwilightMorning;
        double astronomicalTwilightEvening;
        bool   astronomicalTwilightValid;

        double nauticalTwilightMorning;
        double nauticalTwilightEvening;
        bool   nauticalTwilightValid;

        double civilTwilightMorning;
        double civilTwilightEvening;
        bool   civilTwilightValid;

        CelestialCalculator::CelestialObjectRiseSet::RiseSetType type;

	} RiseSetInfo;

    typedef struct
    {
        double ra;      // Right Ascension
        double decl;    // Declination
        double r;       // Distance
    } OrbitPosition;

private:
	bool calculate( const SYSTEMTIME current_date,
					const double latitude, const double longitude,
					const int utc_diff, SYSTEMTIME& sunrise );

	// return angle in range [0..360[
    double rev( double x );

	double sind( double x );
	double cosd( double x );
	double tand( double x );
	double asind( double x );
	double acosd( double x );
	double atand( double x );
	double atan2d( double y, double x );

	RiseSetInfo GMSTRiseSet( const double ra, const double dec, double lon, double lat, double altitude );
	RiseSetInfo RiseSet( double jd_out, const double ra1, const double dec1,
		const double ra2, const double dec2, const double lon, const double lat, 
		const double diameter, const double parallax, const double timeinterval,
        const double altitude );
	RiseSetInfo SunRise( double jd, double deltaT, double lon, double lat, double zone, bool recursive );
	RiseSetInfo MoonRise( double jd, double deltaT, double lon, double lat, double zone, bool recursive );
    
	bool		mValid;
	RiseSetInfo mSun;
	RiseSetInfo mMoon;
};


