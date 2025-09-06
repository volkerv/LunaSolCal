#include <stdafx.h>
#include <math.h>
#include "calc.h"

Calculator::Calculator( ) : mValid( false )
{
}


bool Calculator::sundeclination( double& sd )
{
	if ( mValid )
	{
        sd = mSun.declination;
	}
	
	return mValid;
}

bool Calculator::moondeclination( double& md )
{
	if ( mValid )
	{
        md = mMoon.declination;
	}
	
	return mValid;
}

double Calculator::dayLength( )
{
    double dayLength = 0.0;

    switch ( mSun.type )
    {
        case CelestialCalculator::CelestialObjectRiseSet::RiseSetType::AllDayAboveHorizon:
            dayLength = 24.0;
            break;
        case CelestialCalculator::CelestialObjectRiseSet::RiseSetType::AllDayBelowHorizon:
            dayLength = 0.0;
            break;
        default:
            dayLength = mSun.set - mSun.rise;
            if ( dayLength < 0 )
            {
                dayLength += 24;
            }
            break;
    }

    return dayLength;
}

// ----------------------------------------------------------------------------

const double pi = 3.1415926535897932384626433832795028841972;
const double DEG = pi/180.0;
const double RAD = 180./pi;

// ----------------------------------------------------------------------------

typedef enum {
    SIGN_WIDDER = 0,
    SIGN_STIER, 
    SIGN_ZWILLINGE, 
    SIGN_KREBS, 
    SIGN_LOEWE, 
    SIGN_JUNGFRAU, 
    SIGN_WAAGE, 
    SIGN_SKORPION, 
    SIGN_SCHUETZE, 
    SIGN_STEINBOCK, 
    SIGN_WASSERMANN, 
    SIGN_FISCHE
} Sign;

typedef struct {
    double  longitude;
    double  latitude;
    double  anomalyMean;
    double  distance;
    double  diameter;
    double  parallax;
    double  ra;
    double  dec;

} SunCoordinates;

typedef struct {
    double  longitude;
    double  latitude;
    double  orbitLongitude;
    double  anomalyMean;
    double  distance;
    double  diameter;
    double  parallax;
    double  moonAge;
    double  phase;
    int     moonPhase;  // index in moon_phase array
    double  ra;
    double  dec;

} MoonCoordinates;

// ----------------------------------------------------------------------------

double fract( double x ) 
{ 
    return ( x - floor( x ) ); 
}

// ----------------------------------------------------------------------------

double mod( double a, double b) 
{ 
    return ( a - floor( a / b ) * b ); 
}

// ----------------------------------------------------------------------------

// Modulo PI
double mod2pi( const double x )
{
    return mod( x, 2 * pi );
}

// ----------------------------------------------------------------------------

// Transform ecliptical SunCoordinates (lon/lat) to equatorial SunCoordinates (RA/dec)
void ecl2equ( const double longitude, const double latitude, const double tdt,
              double& ra, double& dec )
{
    const double t = ( tdt - 2451545.0 ) / 36525.; // Epoch 2000 January 1.5
    const double eps = ( 23. + ( 26. + 21.45 / 60. ) / 60. + t * ( -46.815 + t * ( -0.0006 + t * 0.00181 ) ) / 3600. ) * DEG;
    const double coseps = cos( eps );
    const double sineps = sin( eps );

    const double sinlon = sin( longitude );
    ra  = mod2pi( atan2( ( sinlon * coseps - tan( latitude ) * sineps ), cos( longitude ) ) );
    dec = asin( sin( latitude ) * coseps + cos( latitude ) * sineps*  sinlon );
}

// ----------------------------------------------------------------------------

// Calculate Julian date: valid only from 1.3.1901 to 28.2.2100
double calc_julian_date( int day, int month, int year )
{
  double jd = 2415020.5-64.; // 1.1.1900 - correction of algorithm
  if ( month <= 2 ) 
  { 
      year--; 
      month += 12; 
  }

  jd += (int) ( ( year - 1900. ) * 365.25 );
  jd += (int) ( 30.6001 * (1. + month ) );
  return ( jd + day );
}

// ----------------------------------------------------------------------------

// Julian Date to Greenwich Mean Sidereal Time
double GMST( double jd )
{
    const double UT = fract( jd - 0.5 ) * 24; // UT in hours
    jd = floor( jd - 0.5 ) + 0.5;   // JD at 0 hours UT
    const double T = ( jd - 2451545.0 ) / 36525.0;
    const double T0 = 6.697374558 + T * ( 2400.051336 + T * 0.000025862 );
    return mod( T0 + UT * 1.002737909, 24 );
}

// ----------------------------------------------------------------------------

// Convert Greenweek mean sidereal time to UT
double GMST2UT( double jd, double gmst )
{
  jd = floor(jd - 0.5 ) + 0.5;   // JD at 0 hours UT
  const double T = ( jd - 2451545.0 ) / 36525.0;
  const double T0 = mod( 6.697374558 + T * ( 2400.051336 + T * 0.000025862 ), 24);
  return ( 0.9972695663 * ( ( gmst-T0 ) ) );
}

// ----------------------------------------------------------------------------

// Find GMST of rise/set of object from the two calculates 
// (start)points (day 1 and 2) and at midnight UT(0)
double InterpolateGMST( double gmst0, double gmst1, double gmst2, double timefactor )
{
  return( ( timefactor * 24.07 * gmst1 - gmst0 * ( gmst2-gmst1 ) ) / ( timefactor * 24.07 + gmst1 - gmst2 ) );
}

// ----------------------------------------------------------------------------

// returns Greenwich sidereal time (hours) of time of rise 
// and set of object with SunCoordinates coor.ra/coor.dec
// at geographic position lon/lat (all values in radians)
// for mathematical horizon, without refraction and body radius correction
Calculator::RiseSetInfo Calculator::GMSTRiseSet( const double ra, const double dec, double lon, double lat, double altitude )
{
    double tagbogen = acos( ( sin( altitude ) - sin( lat ) * sin( dec ) ) / ( cos( lat ) * cos( dec ) ) );

    RiseSetInfo riseset;
    riseset.valid = true;
    if ( tagbogen != tagbogen )  // test if NaN
    {
        riseset.valid = false;
        return riseset;
    }

    riseset.transit = RAD / 15 * ( ra - lon );
    riseset.rise    = 24. + RAD / 15 * ( -tagbogen + ra - lon ); // calculate GMST of rise of object
    riseset.set     = RAD / 15 * ( tagbogen + ra - lon ); // calculate GMST of set of object

    riseset.transit = mod( riseset.transit, 24 );
    riseset.rise    = mod( riseset.rise, 24 );
    riseset.set     = mod( riseset.set, 24 );

    return riseset;
}

// ----------------------------------------------------------------------------

// Calculate SunCoordinates for Sun
// SunCoordinates are accurate to about 10s (right ascension) 
// and a few minutes of arc (declination)

SunCoordinates SunPosition( double tdt )
{
    const double d = tdt - 2447891.5;

    const double eg = 279.403303 * DEG;
    const double wg = 282.768422 * DEG;
    const double e  = 0.016713;
    const double a  = 149598500.0; // km
    const double diameter0 = 0.533128 * DEG; // angular diameter of Moon at a distance

    const double MSun = 360.0 * DEG / 365.242191 * d + eg - wg;
    const double nu = MSun + 360.0 * DEG / pi * e * sin( MSun );

    SunCoordinates sun_coords;
    sun_coords.longitude =  mod2pi( nu + wg );
    sun_coords.latitude = 0;
    sun_coords.anomalyMean = MSun;

    sun_coords.distance = ( 1.0 - e * e ) / ( 1.0 + e * cos( nu ) );   // distance in astronomical units
    sun_coords.diameter = diameter0 / sun_coords.distance; // angular diameter in radians
    sun_coords.distance *= a;						  // distance in km
    sun_coords.parallax = 6378.137 / sun_coords.distance;  // horizonal parallax

    ecl2equ( sun_coords.longitude, sun_coords.latitude, tdt, sun_coords.ra, sun_coords.dec );

    return sun_coords;
}

// ----------------------------------------------------------------------------

// Calculate data and SunCoordinates for the Moon
// SunCoordinates are accurate to about 1/5 degree (in ecliptic SunCoordinates)
MoonCoordinates MoonPosition( const SunCoordinates& sun_coords, double tdt )
{
    const double D = tdt - 2447891.5;

    // Mean Moon orbit elements as of 1990.0
    const double l0 = 318.351648 * DEG;
    const double P0 =  36.340410 * DEG;
    const double N0 = 318.510107 * DEG;
    const double i  = 5.145396 * DEG;
    const double e  = 0.054900;
    const double a  = 384401; // km
    const double diameter0 = 0.5181 * DEG; // angular diameter of Moon at a distance
    const double parallax0 = 0.9507 * DEG; // parallax at distance a

    const double l = 13.1763966 * DEG * D + l0;
    const double MMoon = l - 0.1114041 * DEG * D - P0; // Moon's mean anomaly M
    const double N = N0 - 0.0529539*DEG*D;       // Moon's mean ascending node longitude
    const double C = l - sun_coords.longitude;
    const double Ev = 1.2739 * DEG * sin( 2 * C - MMoon );
    const double Ae = 0.1858 * DEG * sin( sun_coords.anomalyMean );
    const double A3 = 0.37 * DEG * sin( sun_coords.anomalyMean );
    const double MMoon2 = MMoon + Ev - Ae - A3;  // corrected Moon anomaly
    const double Ec = 6.2886 * DEG * sin( MMoon2 );  // equation of centre
    const double A4 = 0.214 * DEG * sin( 2 * MMoon2 );
    const double l2 = l + Ev + Ec - Ae + A4; // corrected Moon's longitude
    const double V = 0.6583 * DEG * sin( 2 * ( l2 - sun_coords.longitude ) );
    const double l3 = l2 + V; // true orbital longitude;

    const double N2 = N - 0.16 * DEG * sin( sun_coords.anomalyMean );

    MoonCoordinates moon_coords;
    moon_coords.longitude = mod2pi( N2 + atan2( sin( l3 - N2 ) * cos( i ), cos( l3 - N2 ) ) );
    moon_coords.latitude = asin( sin( l3 - N2 ) * sin( i ) );
    moon_coords.orbitLongitude = l3;

    ecl2equ( moon_coords.longitude, moon_coords.latitude, tdt, 
        moon_coords.ra, moon_coords.dec );

    // relative distance to semi mayor axis of lunar oribt
    moon_coords.distance = ( 1.0 - e * e ) / ( 1.0 + e * cos( MMoon2 + Ec ) );
    moon_coords.diameter = diameter0 / moon_coords.distance; // angular diameter in radians
    moon_coords.parallax = parallax0 / moon_coords.distance; // horizontal parallax in radians
    moon_coords.distance *= a;	// distance in km

    // Age of Moon in radians since New Moon (0) - Full Moon (pi)
    moon_coords.moonAge = mod2pi( l3 - sun_coords.longitude );   
    moon_coords.phase   = 0.5 * ( 1.0 - cos( moon_coords.moonAge ) ); // Moon phase, 0-1

    const double  mainPhase = 1.0 / 29.53 * 360 * DEG; // show 'Newmoon, 'Quarter' for +/-1 day arond the actual event
    double p = mod( moon_coords.moonAge, 90.0 * DEG );

    if ( ( p < mainPhase ) || 
        ( p > 90.0 * DEG - mainPhase ) )
    {
        p = 2.0 * (double)(int)( ( moon_coords.moonAge / ( 90.0 * DEG ) ) + 0.5);
    }
    else 
    {
        p = 2.0 * floor( moon_coords.moonAge / (90.0 * DEG ) ) + 1.0;
    }

    moon_coords.moonPhase = (int)p;

    return moon_coords;
}

// ----------------------------------------------------------------------------

// JD is the Julian Date of 0h UTC time (midnight)
Calculator::RiseSetInfo Calculator::RiseSet( 
	double jd_out, const double ra1, const double dec1,
	const double ra2, const double dec2, const double lon, const double lat, 
	const double diameter, const double parallax, const double timeinterval,
    const double altitude)
{
    RiseSetInfo sun_rise;
    double alt = 0.0;

    if ( altitude == 0.0 )
    {
        alt = 0.5 * diameter - parallax + 34.0 / 60.0 * DEG;
    }

    RiseSetInfo rise1 = GMSTRiseSet( ra1, dec1, lon, lat, altitude );
    RiseSetInfo rise2 = GMSTRiseSet( ra2, dec2, lon, lat, altitude );

    if ( rise1.valid && rise2.valid )
    {
        // unwrap GMST in case we move across 24h -> 0h
        if ( rise1.transit > rise2.transit 
            && fabs( rise1.transit - rise2.transit ) > 18 ) 
        {
            rise2.transit += 24;
        }

        if ( rise1.rise > rise2.rise    
            && fabs( rise1.rise - rise2.rise) > 18 )   
        {
            rise2.rise += 24;
        }

        if ( rise1.set > rise2.set     
            && fabs( rise1.set - rise2.set ) > 18 )     
        {
            rise2.set  += 24;
        }

        double T0 = GMST( jd_out );

        // Greenwich sidereal time for 0h at selected longitude
        double T02 = T0 - lon * RAD / 15 * 1.002738; 
        if ( T02 < 0 ) 
        {
            T02 += 24; 
        }

        if ( rise1.transit < T02 ) 
        { 
            rise1.transit += 24; 
            rise2.transit += 24; 
        }

        if ( rise1.rise < T02 ) 
        { 
            rise1.rise += 24; 
            rise2.rise += 24; 
        }

        if ( rise1.set < T02 ) 
        { 
            rise1.set += 24; 
            rise2.set += 24; 
        }

        // Refraction and Parallax correction
        const double decMean = 0.5 * ( dec1 + dec2 );
        const double psi = acos( sin( lat ) / cos( decMean ) );

        // altitude of sun center: semi-diameter, horizontal parallax and (standard) refraction of 34'
        double y = asin( sin( alt ) / sin( psi ) );
        double dt = 240.0 * RAD * y / cos( decMean ) / 3600.0; // time correction due to refraction, parallax

        sun_rise.transit = GMST2UT( jd_out, InterpolateGMST( T0, rise1.transit, rise2.transit, timeinterval ) );
        sun_rise.rise    = GMST2UT( jd_out, InterpolateGMST( T0, rise1.rise, rise2.rise, timeinterval ) - dt );
        sun_rise.set     = GMST2UT( jd_out, InterpolateGMST( T0, rise1.set, rise2.set, timeinterval ) + dt );
        sun_rise.valid   = true;
    }
    else
    {
        sun_rise.valid = false;
    }

    return sun_rise;
}

// ----------------------------------------------------------------------------

// Find (local) time of sunrise and sunset
// JD is the Julian Date of 0h local time (midnight)
// Accurate to about 1-2 minutes
// recursive: 1 - calculate rise/set in UTC
// recursive: 0 - find rise/set on the current local day (set could also be first)
Calculator::RiseSetInfo Calculator::SunRise( 
	double jd, double deltaT, double lon, double lat, double zone, bool recursive )
{
    double jd_out = floor( jd - 0.5 ) + 0.5;
    SunCoordinates coord1 = SunPosition( jd_out + 0 * deltaT / 24 / 3600 );
    SunCoordinates coord2 = SunPosition( jd_out + 1 + 0 * deltaT / 24 / 3600 ); // calculations for next day's UTC midnight

    RiseSetInfo sun_rise = RiseSet( jd_out, coord1.ra, coord1.dec, coord2.ra, coord2.dec, 
        lon, lat, coord1.diameter, coord1.parallax, 1, 0  ); // rise/set time in UTC;

    sun_rise.riseValid = sun_rise.setValid = sun_rise.transitValid = sun_rise.valid;

    if ( ! recursive )
    { 
        // check and adjust to have rise/set time on local calendar day
        if ( zone > 0 ) 
        {
            // rise time was yesterday local time -> calculate rise time for next UTC day
            if ( sun_rise.rise >= 24 - zone || sun_rise.transit >= 24 - zone || sun_rise.set >= 24 - zone ) 
            {
                RiseSetInfo risetemp = SunRise( jd+1, deltaT, lon, lat, zone, true );
            
                if ( sun_rise.rise >= 24 - zone ) 
                {
                    sun_rise.rise = risetemp.rise;
                }
                
                if ( sun_rise.transit >= 24 - zone ) 
                {
                    sun_rise.transit = risetemp.transit;
                }
                
                if ( sun_rise.set >= 24 - zone ) 
                {
                    sun_rise.set  = risetemp.set;
                }
            }
        }
        else 
        {
            if ( zone < 0 ) 
            {
                // rise time was yesterday local time -> calculate rise time for next UTC day
                if ( sun_rise.rise < -zone || sun_rise.transit < -zone || sun_rise.set < -zone) 
                {
                    RiseSetInfo risetemp = SunRise( jd-1, deltaT, lon, lat, zone, true );

                    if ( sun_rise.rise < -zone ) 
                    {
                        sun_rise.rise = risetemp.rise;
                    }

                    if ( sun_rise.transit < -zone ) 
                    {
                        sun_rise.transit = risetemp.transit;
                    }

                    if ( sun_rise.set <-zone ) 
                    {
                        sun_rise.set  = risetemp.set;
                    }
                }
            }
        }

        sun_rise.transit = mod( sun_rise.transit + zone, 24 );
        sun_rise.rise    = mod( sun_rise.rise    + zone, 24 );
        sun_rise.set     = mod( sun_rise.set     + zone, 24 );
    
        // Twilight calculation
        // civil twilight time in UTC.
        RiseSetInfo risetemp = RiseSet( jd_out, coord1.ra, coord1.dec, coord2.ra, coord2.dec,
                lon, lat, coord1.diameter, coord1.parallax, 1, -6. * DEG );
        if ( risetemp.valid )
        {
            sun_rise.civilTwilightMorning = mod( risetemp.rise + zone, 24. );
            sun_rise.civilTwilightEvening = mod( risetemp.set + zone, 24. );
            sun_rise.civilTwilightValid = true;
        }
        else
        {
            sun_rise.civilTwilightValid = false;
        }

        // nautical twilight time in UTC.
        risetemp = RiseSet( jd_out, coord1.ra, coord1.dec, coord2.ra, coord2.dec, lon, lat, coord1.diameter,
                coord1.parallax, 1, -12. * DEG );
        if ( risetemp.valid )
        {
            sun_rise.nauticalTwilightMorning = mod( risetemp.rise + zone, 24. );
            sun_rise.nauticalTwilightEvening = mod( risetemp.set + zone, 24. );
            sun_rise.nauticalTwilightValid = true;
        }
        else
        {
            sun_rise.nauticalTwilightValid = false;
        }

        // astronomical twilight time in UTC.
        risetemp = RiseSet( jd_out, coord1.ra, coord1.dec, coord2.ra, coord2.dec, lon, lat, coord1.diameter,
                coord1.parallax, 1, -18. * DEG );
        if ( risetemp.valid )
        {
            sun_rise.astronomicalTwilightMorning = mod( risetemp.rise + zone, 24. );
            sun_rise.astronomicalTwilightEvening = mod( risetemp.set + zone, 24. );
            sun_rise.astronomicalTwilightValid = true;
        }
        else
        {
            sun_rise.astronomicalTwilightValid = false;
        }
    }

    return sun_rise;
}

// ----------------------------------------------------------------------------

// Find local time of moonrise and moonset
// JD is the Julian Date of 0h local time (midnight)
// Accurate to about 5 minutes or better
// recursive: 1 - calculate rise/set in UTC
// recursive: 0 - find rise/set on the current local day (set could also be first)
// returns '' for moonrise/set does not occur on selected day
Calculator::RiseSetInfo Calculator::MoonRise( 
	double jd, double deltaT, double lon, double lat, double zone, bool recursive )
{
  const double timeinterval = 0.5;
  
  const double jd0UT = floor( jd - 0.5 ) + 0.5;   // JD at 0 hours UT
  const SunCoordinates sun_coord1 = SunPosition( jd0UT + 0 * deltaT / 24 / 3600 );
  const MoonCoordinates moon_coord1 = MoonPosition( sun_coord1, jd0UT + 0 * deltaT / 24 / 3600 );

  const SunCoordinates sun_coord2 = SunPosition( jd0UT + timeinterval + 0 * deltaT / 24 / 3600 ); // calculations for noon
  // calculations for next day's midnight
  const MoonCoordinates moon_coord2 = MoonPosition( sun_coord2, jd0UT + timeinterval + 0 * deltaT / 24 / 3600 ); 
  
  // rise/set time in UTC, time zone corrected later
  RiseSetInfo risetemp;
  RiseSetInfo rise = RiseSet( jd0UT, moon_coord1.ra, moon_coord1.dec, moon_coord2.ra, moon_coord2.dec, 
      lon, lat, moon_coord1.diameter, moon_coord1.parallax, timeinterval, 0 ); 

  rise.riseValid = rise.setValid = rise.transitValid = rise.valid;

  if ( !recursive ) 
  { 
      // check and adjust to have rise/set time on local calendar day
      if ( zone > 0 ) 
      {
          // recursive call to MoonRise returns events in UTC
	      const RiseSetInfo riseprev = MoonRise( jd - 1, deltaT, lon, lat, zone, 1 ); 
	  
          if ( rise.transit >= 24 - zone || rise.transit < -zone) 
          { 
              // transit time is tomorrow local time
              rise.transitValid = !( riseprev.transit < 24 - zone );
              if ( rise.transitValid )
              {
                  rise.transit = riseprev.transit;
              }
          }

          if ( ( rise.rise >= 24 - zone ) || 
              ( rise.rise < -zone ) ) 
          {
              // transit time is tomorrow local time
              rise.riseValid = !(riseprev.rise < 24 - zone) ;
              if ( rise.riseValid )
              {
                  rise.rise  = riseprev.rise;
              }
          }

          if ( ( rise.set >= 24 - zone ) || 
              ( rise.set < -zone ) ) 
          { 
              // transit time is tomorrow local time
              rise.setValid = !(riseprev.set < 24 - zone ) ;
              if ( rise.setValid )
              {
                  rise.set  = riseprev.set;
              }
          }
      }
      else 
      {
          if ( zone < 0 ) 
          {
              // rise/set time was tomorrow local time -> calculate rise time for former UTC day
              if ( ( rise.rise < -zone ) || 
                  ( rise.set < -zone ) || 
                  ( rise.transit <- zone ) ) 
              { 
                  risetemp = MoonRise( jd+1, deltaT, lon, lat, zone, 1 );

                  if ( rise.rise < -zone )
                  {
                      rise.riseValid = !( risetemp.rise > -zone );
                      if (rise.riseValid)
                      {
                          rise.rise = risetemp.rise;
                      }
                  }

                  if ( rise.transit < -zone )
                  {
                      rise.transitValid = !( risetemp.transit > -zone );
                      if ( rise.transitValid )
                      {
                          rise.transit  = risetemp.transit;
                      }
                  }

                  if ( rise.set < -zone )
                  {
                      rise.setValid = !( risetemp.set > -zone );
                      if ( rise.setValid )
                      {
                          rise.set  = risetemp.set;
                      }
                  }
              }
          }
      }

      if ( rise.riseValid )
      {
          // correct for time zone, if time is valid
          rise.rise = mod( rise.rise + zone, 24 );
      }

      if ( rise.transitValid ) 
      {
          // correct for time zone, if time is valid
          rise.transit  = mod( rise.transit + zone, 24 ); 
      }

      if ( rise.setValid )
      {
          // correct for time zone, if time is valid
          rise.set  = mod( rise.set + zone, 24 );    
      }
  }
  return rise;  
}

// ----------------------------------------------------------------------------
bool Calculator::calc( const double latitude, const double longitude, 
					   const SYSTEMTIME date, const double utcDiff )
{
    if ( ( date.wYear <= 1900 ) || 
         ( date.wYear >= 2100 ) )
    {
		mValid = false;
    }
	else
	{
    	double jdo = calc_julian_date( date.wDay, date.wMonth, date.wYear );
		const double min = date.wMinute / 60.;
		const double sec = date.wSecond / 3600.;
		double jd = jdo + ( date.wHour + min + sec ) / 24.;
		double tdt = jd + 65./24./3600.;

		SunCoordinates sun_coords = SunPosition( tdt );
		MoonCoordinates moon_coords = MoonPosition( sun_coords, tdt );

		const double deltaT = 65.;
		const double lat = latitude * DEG;
		const double lon = longitude * DEG;

		mSun = SunRise( jdo, deltaT, lon, lat, utcDiff, false );    
        mSun.declination = sun_coords.dec * RAD;
        int dayOfYear = (int) (jdo - calc_julian_date( 1, 1, date.wYear ) + 1);
        mSun.timeEquation = -0.170869921174742*sin(0.0336997028793971 * dayOfYear + 0.465419984181394) - 
            0.129890681040717*sin(0.0178674832556871*dayOfYear - 0.167936777524864);

		mMoon = MoonRise( jdo, deltaT, lon, lat, utcDiff, false ); 
        mMoon.declination = moon_coords.dec * RAD;
        mMoon.phaseIdx = moon_coords.moonPhase;
        mMoon.phase = moon_coords.phase;

        CelestialCalculator::Date d = { date.wYear, date.wMonth, date.wDay, date.wHour, date.wMinute, date.wSecond };
        CelestialCalculator::CelestialObjectRiseSet result = calcSunRiseSet( d, latitude, longitude );
        if ( ( result.type == CelestialCalculator::CelestialObjectRiseSet::RiseSetType::AllDayAboveHorizon ) ||
            ( result.type == CelestialCalculator::CelestialObjectRiseSet::RiseSetType::AllDayBelowHorizon ) )
        {
            mSun.riseValid = false;
            mSun.setValid = false;
        }
        else
        {
            mSun.riseValid = true;
            mSun.setValid = true;

            mSun.rise = result.rise + utcDiff;
            mSun.set = result.set + utcDiff;
            mSun.type = result.type;
        }

        result = calcMoonRiseSet( d, utcDiff, latitude, longitude );
        if ( ( result.type == CelestialCalculator::CelestialObjectRiseSet::RiseSetType::AllDayAboveHorizon ) ||
            ( result.type == CelestialCalculator::CelestialObjectRiseSet::RiseSetType::AllDayBelowHorizon ) )
        {
            mMoon.riseValid = false;
            mMoon.setValid = false;
        }
        else
        {
            mMoon.riseValid = ( result.type == CelestialCalculator::CelestialObjectRiseSet::RiseSetType::Normal ) ||
                ( result.type != CelestialCalculator::CelestialObjectRiseSet::RiseSetType::RiseInvalid );
            mMoon.setValid = ( result.type == CelestialCalculator::CelestialObjectRiseSet::RiseSetType::Normal ) ||
                ( result.type != CelestialCalculator::CelestialObjectRiseSet::RiseSetType::SetInvalid );

            mMoon.rise = result.rise;
            mMoon.set = result.set;
            mMoon.type = result.type;
        }

		mValid = true;

	}

    return mValid;
}

