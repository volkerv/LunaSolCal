#include "stdafx.h"
#include "celestialCalc.h"

namespace CelestialCalculator
{
    const double synmonth  = 29.53058868;  /* Synodic month (new Moon to new Moon) */

    bool calcCelestialObjectPosition( Object obj, 
                                      const Date& date, 
                                      const double latitude, 
                                      const double longitude, 
                                      CelestialObjectPosition& objectPos )
    {
        switch (obj)
        {
        case SUN:
            return calcSunCelestialObjectPosition( date, latitude, longitude, objectPos );
            break;
        case MOON:
            return calcMoonCelestialObjectPosition( date, latitude, longitude, objectPos );
            break;
        default:
            break;
        }

        return false;
    }

    bool calcSunCelestialObjectPosition( const Date& date, 
                                         const double latitude,
                                         const double longitude, 
                                         CelestialObjectPosition& objectPos )
    {
        const double d = convertDateToDayNumber( date );

        // longitude of perihelion
        const double w = 282.9404 + 4.70935E-05 * d;
        
        // mean distance, AU
        //const double a = 1.0;

        // eccentricity
        const double e = 0.016709 - 1.151E-9 * d;

        // mean anomaly
        const double M = rev( 356.0470 + 0.9856002585 * d );

        // obliquity of the ecliptic
        const double oblecl = 23.4393 - 3.563E-7 * d;

        // mean longitude of the sun
        const double L = rev( w + M );

        // Eccentricity of the sun's orbit
        const double E = M + ( 180.0 / pi ) * e * sind( M ) * ( 1.0 + e * cosd( M ) );

        // sun rectangular coordinates in the plane of the ecliptic
        const double x = cosd( E ) - e;
        const double y = sind( E ) * sqrt( 1.0 - e * e );

        // distance and true anomaly
        const double r = sqrt( x * x + y * y );
        const double v = atan2d( y, x );

        // longitude of the sun
        const double lon = rev( v + w );

        // sun's ecliptic rectangular coordinates
        const double xeclip = r * cosd( lon );
        const double yeclip = r * sind( lon );
        //const double zeclip = 0.0;

        // rotate using obdecl
        const double xequat = xeclip;
        const double yequat = yeclip * cosd( oblecl ) - 0.0 * sind( oblecl );
        const double zequat = yeclip * sind( oblecl ) + 0.0 * cosd( oblecl );

        // RA and Decl
        const double RA = rev( atan2d( yequat, xequat ) );
        const double Decl = asind( zequat / r );

        // sidereal time at Greenwich meridian at 00:00
        const double GMST0 = rev( L + 180.0 ) / 15.0; // hours

        // sidereal time at given terrestial longitude
        const double timeOffset = date.hour + date.minute / 60.0 + date.second / 3600.0;
        const double SIDTIME = GMST0 + timeOffset + longitude / 15.0; // hours

        // hour angle 
        const double HA = ( SIDTIME - RA / 15.0 ) * 15.0;  // degrees

        const double x2 = cosd( HA ) * cosd( Decl );
        const double y2 = sind( HA ) * cosd( Decl );
        const double z2 = sind( Decl );

        const double xhor = x2 * sind( latitude ) - z2 * cosd( latitude );
        const double yhor = y2;
        const double zhor = x2 * cosd( latitude ) + z2 * sind( latitude );

        const double azimuth = rev( atan2d( yhor, xhor ) + 180.0 );  // +180.0 because we relate it to North (0 deg)
        const double altitude = asind( zhor);

        objectPos.ra = RA;
        objectPos.decl = Decl;
        objectPos.r = r;
        objectPos.L = L;
        objectPos.M = M;
        objectPos.altitude = altitude;
        objectPos.azimuth = azimuth;

        return true;
    }

    bool calcMoonCelestialObjectPosition( const Date& date, 
                                          const double latitude, 
                                          const double longitude, 
                                          CelestialObjectPosition& objectPos )
    {
        const double d = convertDateToDayNumber( date );

        // obliquity of the ecliptic
        const double oblecl = 23.4393 - 3.563E-7 * d;

        // Long asc. node
        const double N = rev( 125.1228 - 0.0529538083 * d );

        // Inclination
        const double i = 5.1454;

        // Arg. of perigee
        const double w = rev( 318.0634 + 0.1643573223 * d );

        // Mean distance
        const double a = 60.2666;

        // Eccentricity
        const double e = 0.054900;

        // Mean anomaly
        const double M = rev( 115.3654 + 13.0649929509 * d );

        // Eccentric anomaly
        double E0 = M + (180.0 / pi ) * e * sind( M ) * ( 1.0 + e + cosd( M ) );
        double E1 = 0.0;
        bool bCont = true;

        while ( bCont )
        {
            E1 = E0 - ( E0 - ( 180.0 / pi ) * e * sind( E0 ) - M ) / ( 1.0 - e * cosd( E0 ) );
            
            bCont = ( fabs( E1 - E0 ) > 0.00005 );

            E0 = E1;
        }

        const double E = E0;

        const double x = a * ( cosd( E ) - e );
        const double y = a * sqrt( 1.0 - e*e ) * sind( E );

        const double r = sqrt( x*x + y*y );  // earth radii
        const double v = rev( atan2d( y, x ) );

        const double xeclip = r * ( cosd( N ) * cosd( v +w ) - sind( N ) * sind( v + w ) * cosd( i ) );
        const double yeclip = r * ( sind( N ) * cosd( v +w ) + cosd( N ) * sind( v + w ) * cosd( i ) );
        const double zeclip = r * sind( v + w ) * sind( i );

        // Pertubations caused by the sun
        CelestialObjectPosition sunPos;
        calcSunCelestialObjectPosition( date, latitude, longitude, sunPos );

        const double Ms = sunPos.M;
        const double Mm = M;
        const double Ls = sunPos.L;
        const double Lm = rev( N + w + Mm );

        // Moon's mean elongation
        const double D = rev( Lm - Ls );

        // Moon's argument of latitude
        const double F = Lm - N;

        const double lonPerturbations = 
            -1.274 * sind( Mm - 2*D ) +
            +0.658 * sind( 2*D ) +
            -0.186 * sind( Ms ) +
            -0.059 * sind( 2*Mm - 2*D ) +
            -0.057 * sind( Mm - 2*D + Ms ) +
            +0.053 * sind( Mm + 2*D ) +
            +0.046 * sind( 2*D - Ms ) +
            +0.041 * sind( Mm - Ms ) +
            -0.035 * sind( D ) +
            -0.031 * sind( Mm + Ms ) +
            -0.015 * sind( 2*F - 2*D ) +
            +0.011 *sind( Mm - 4*D );

        const double latPerturbations =
            -0.173 * sind( F - 2*D ) +
            -0.055 * sind( Mm - F - 2*D ) +
            -0.046 * sind( Mm + F - 2*D ) +
            +0.033 * sind( F + 2*D ) +
            +0.017 * sind( 2*Mm + F );

        const double distPerturbations =
            -0.58 * cosd( Mm - 2*D ) +
            -0.46 * cosd( 2*D );

        // Unclear: Do we have to compute r?
        double lon = rev( atan2d( yeclip, xeclip ) );
        double lat = atan2d( zeclip * sind( lon ),  yeclip );
        if ( lat > 90 ) 
        {
            lat -= 180.0;
        }

        if ( lat < -90 )
        {
            lat += 180.0;
        }

        double rr = zeclip / sind( lat );

        lon += lonPerturbations;
        lat += latPerturbations;
        rr += distPerturbations;

        // moon's ecliptic rectangular coordinates
        const double xeclip2 = cosd( lon ) * cosd( lat );
        const double yeclip2 = sind( lon ) * cosd( lat );
        const double zeclip2 = sind( lat );

        // rotate using obdecl
        const double xequat = xeclip2;
        const double yequat = yeclip2 * cosd( oblecl ) - zeclip2 * sind( oblecl );
        const double zequat = yeclip2 * sind( oblecl ) + zeclip2 * cosd( oblecl );

        // RA and Decl
        const double RA = rev( atan2d( yequat, xequat ) );
        const double Decl = asind( zequat );

        // Topocentric RA and Decl
        const double gclat = latitude - 0.1924 * sind( 2*latitude );
        const double rho = 0.99833 + 0.00167 * cosd( 2*latitude );

        // sidereal time at Greenwich meridian at 00:00
        const double GMST0 = rev( Ls + 180.0 ) / 15.0; // hours

        // sidereal time at given terrestial longitude
        const double timeOffset = date.hour + date.minute / 60.0 + date.second / 3600.0;
        const double SIDTIME = GMST0 + timeOffset + longitude / 15.0; // hours

        const double HA = rev( SIDTIME * 15.0 - RA );
        const double g = atand( tand( gclat ) / cosd( HA ) );
        const double mpar = asind( 1 / rr );
        
        const double topRA = RA - mpar * rho * cosd( gclat ) * sind( HA ) / cosd( Decl );
        const double topDecl = Decl - mpar * rho * sind( gclat ) * sind( g - Decl ) / sind( g );

        const double x2 = cosd( HA ) * cosd( topDecl );
        const double y2 = sind( HA ) * cosd( topDecl );
        const double z2 = sind( topDecl );

        const double xhor = x2 * sind( latitude ) - z2 * cosd( latitude );
        const double yhor = y2;
        const double zhor = x2 * cosd( latitude ) + z2 * sind( latitude );

        const double azimuth = rev( atan2d( yhor, xhor ) + 180.0);  // +180.0 because we relate it to North (0 deg)
        const double altitude = asind( zhor);

        objectPos.decl = topDecl;
        objectPos.L = Lm;
        objectPos.M = Mm;
        objectPos.ra = topRA;
        objectPos.r = rr;
        objectPos.altitude = altitude;
        objectPos.azimuth = azimuth;

        return true;
    }

    double convertDateToDayNumber( const Date& date )
    {
        const double timeOffset = date.hour + date.minute / 60.0 + date.second / 3600.0;

        return 367 * date.year - 
            (int)(( 7 * ( date.year + ((date.month + 9) / 12 ))) / 4) +
            (int)(( 275 * date.month ) / 9) + date.day - 730530 +
            timeOffset / 24.0;
    }

    bool calcNextFullNewMoonDates( const Date& baseDate, Date& nextFullMoonDate, Date& nextNewMoonDate )
    {
        bool foundNewMoonDate = false;
        bool foundFullMoonDate = false;

        const double jdate_base = jdate( baseDate.year, baseDate.month, baseDate.day );
    
        findNextNewFullMoonDates( baseDate, jdate_base, 
            foundNewMoonDate, nextNewMoonDate,
            foundFullMoonDate,  nextFullMoonDate );

        if ( !foundNewMoonDate || !foundFullMoonDate )
        {
            findNextNewFullMoonDates( baseDate, jdate_base + 29, 
                foundNewMoonDate, nextNewMoonDate,
                foundFullMoonDate,  nextFullMoonDate );
        }

        return true;
    }

    //
    // Helpers  
    //
    double rev( double x )
    {
        return x - floor( x / 360.0) * 360.0;
    }

    double sind( double x )  { return sin( x * DEGRAD ); };
    double cosd( double x )  { return cos( x * DEGRAD ); };
    double tand( double x )  { return tan( x * DEGRAD ); };
    double asind( double x ) { return ( RADEG * asin( x ) ); };
    double acosd( double x ) { return ( RADEG * acos( x ) ); };
    double atand( double x ) { return ( RADEG * atan( x ) ); };
    double atan2d( double y, double x ) { return ( RADEG * atan2( y, x ) ); };

    bool isLater( const Date& d1, const Date& d2 )
    {
        return
            (( d2.year > d1.year ) ||
            (( d2.year == d1.year ) && (d2.month > d1.month)) ||
            (( d2.year == d1.year ) && (d2.month == d1.month) && (d2.day > d1.day)));
    }

    //
    // JDATE  --  Convert internal GMT date to Julian day
    //
    int jdate( int year, int month, int day )
    {
        int y = year;
        int m = month;
        if ( m > 2 ) 
        {
            m = m - 3;
        }
        else 
        {
            m = m + 9;
            y--;
        }

        int c = y / 100L;		   /* Compute century */
        y -= 100L * c;

        return (day + (c * 146097L) / 4 + (y * 1461L) / 4 +
            (m * 153L + 2) / 5 + 1721119L);
    }

    //
    // JYEAR  --  Convert Julian date to year, month, day, which are
    //	       returned via integer pointers to integers.  
    //
    void jyear( double td, Date& date )
    {
        td += 0.5;				/* Astronomical to civil */

        double j = floor(td);
        j = j - 1721119.0;

        double y = floor(((4 * j) - 1) / 146097.0);
        j = (j * 4.0) - (1.0 + (146097.0 * y));

        double d = floor(j / 4.0);
        j = floor(((4.0 * d) + 3.0) / 1461.0);
        d = ((4.0 * d) + 3.0) - (1461.0 * j);
        d = floor((d + 4.0) / 4.0);

        double m = floor(((5.0 * d) - 3) / 153.0);
        d = (5.0 * d) - (3.0 + (153.0 * m));
        d = floor((d + 5.0) / 5.0);

        y = (100.0 * y) + j;

        if (m < 10.0)
        {
            m = m + 3;
        }
        else 
        {
            m = m - 9;
            y = y + 1;
        }

        date.year = (unsigned short)y;
        date.month = (unsigned short)m;
        date.day = (unsigned short)d;
    }


    //
    // JHMS  --  Convert Julian time to hour, minutes, and seconds.
    //
    void jhms( double j, Date& date )
    {
        j += 0.5;				/* Astronomical to civil */
        int ij = (int)(( j - floor( j ) ) * 86400.0);
        date.hour = (unsigned short)(ij / 3600L);
        date.minute = (unsigned short)(( ij / 60L ) % 60L);
        date.second = (unsigned short)(ij % 60L);
    }


    //
    // TRUEPHASE  --  Given a K value used to determine the
    //		mean phase of the new moon, and a phase
    //		selector (0.0, 0.25, 0.5, 0.75), obtain
    //		the true, corrected phase time.
    //
    double truephase( double k, double phase )
    {
        bool apcor = false;

        k += phase;		   /* Add phase to new moon time */

        double t = k / 1236.85;	   /* Time in Julian centuries from
                                   1900 January 0.5 */
        double t2 = t * t;		   /* Square for frequent use */
        double t3 = t2 * t;		   /* Cube for frequent use */

        double pt = 2415020.75933	   /* Mean time of phase */
            + synmonth * k
            + 0.0001178 * t2
            - 0.000000155 * t3
            + 0.00033 * sind(166.56 + 132.87 * t - 0.009173 * t2);

        double m = 359.2242               /* Sun's mean anomaly */
            + 29.10535608 * k
            - 0.0000333 * t2
            - 0.00000347 * t3;

        double mprime = 306.0253          /* Moon's mean anomaly */
            + 385.81691806 * k
            + 0.0107306 * t2
            + 0.00001236 * t3;

        double f = 21.2964                /* Moon's argument of latitude */
            + 390.67050646 * k
            - 0.0016528 * t2
            - 0.00000239 * t3;

        if ((phase < 0.01) || (fabs(phase - 0.5) < 0.01)) 
        {
            /* Corrections for New and Full Moon */
            pt += (0.1734 - 0.000393 * t) * sind(m)
                + 0.0021 * sind(2 * m)
                - 0.4068 * sind(mprime)
                + 0.0161 * sind(2 * mprime)
                - 0.0004 * sind(3 * mprime)
                + 0.0104 * sind(2 * f)
                - 0.0051 * sind(m + mprime)
                - 0.0074 * sind(m - mprime)
                + 0.0004 * sind(2 * f + m)
                - 0.0004 * sind(2 * f - m)
                - 0.0006 * sind(2 * f + mprime)
                + 0.0010 * sind(2 * f - mprime)
                + 0.0005 * sind(m + 2 * mprime);

            apcor = true;
        } 
        else
        {
            if ((fabs(phase - 0.25) < 0.01 || (fabs(phase - 0.75) < 0.01))) 
            {
                pt += (0.1721 - 0.0004 * t) * sind(m)
                    + 0.0021 * sind(2 * m)
                    - 0.6280 * sind(mprime)
                    + 0.0089 * sind(2 * mprime)
                    - 0.0004 * sind(3 * mprime)
                    + 0.0079 * sind(2 * f)
                    - 0.0119 * sind(m + mprime)
                    - 0.0047 * sind(m - mprime)
                    + 0.0003 * sind(2 * f + m)
                    - 0.0004 * sind(2 * f - m)
                    - 0.0006 * sind(2 * f + mprime)
                    + 0.0021 * sind(2 * f - mprime)
                    + 0.0003 * sind(m + 2 * mprime)
                    + 0.0004 * sind(m - 2 * mprime)
                    - 0.0003 * sind(2 * m + mprime);

                if (phase < 0.5)
                {
                    /* First quarter correction */
                    pt += 0.0028 - 0.0004 * cosd(m) + 0.0003 * cosd(mprime);
                }
                else
                {
                    /* Last quarter correction */
                    pt += -0.0028 + 0.0004 * cosd(m) - 0.0003 * cosd(mprime);
                }

                apcor = true;
            }
        }

        if (!apcor) 
        {
            fprintf (stderr,
                "TRUEPHASE called with invalid phase selector.\n");
        }

        return pt;
    }

    //
    // MEANPHASE  --  Calculates time of the mean new Moon for a given
    //		base date.  This argument K to this function is
    //		the precomputed synodic month index, given by:
    //
    //			K = (year - 1900) * 12.3685
    //
    //		where year is expressed as a year and fractional year.
    //
    double meanphase( double sdate, double k )
    {
        /* Time in Julian centuries from 1900 January 0.5 */
        double t = (sdate - 2415020.0) / 36525;
        double t2 = t * t;			/* Square for frequent use */
        double t3 = t2 * t;			/* Cube for frequent use */

        double nt1 = 2415020.75933 + synmonth * k
            + 0.0001178 * t2
            - 0.000000155 * t3
            + 0.00033 * sind(166.56 + 132.87 * t - 0.009173 * t2);

        return nt1;
    }

    //
    // PHASEHUNT  --  Find time of phases of the moon which surround
    //		the current date.  Five phases are found, starting
    //		and ending with the new moons which bound the
    //		current lunation.
    //
    void phasehunt( double sdate, double phases[3] )
    {
        Date date;

        double adate = sdate - 45;
        jyear(adate, date);

        double k1 = floor((date.year + ((date.month - 1) * (1.0 / 12.0)) - 1900) * 12.3685);

        double k2 = 0.0;
        double nt1, nt2;
        adate = nt1 = meanphase(adate, k1);

        bool cont = true;
        while( cont ) 
        {
            adate += synmonth;
            k2 = k1 + 1;
            nt2 = meanphase(adate, k2);

            if (nt1 <= sdate && nt2 > sdate)
            {
                cont = false;
            }
            else
            {
                nt1 = nt2;
                k1 = k2;
            }
        }

        phases[0] = truephase( k1, 0.0 );
//        phases[1] = truephase( k1, 0.25 );
        phases[1] = truephase( k1, 0.5 );
//        phases[3] = truephase( k1, 0.75 );
        phases[2] = truephase( k2, 0.0 );
    }

    void findNextNewFullMoonDates( const Date& baseDate, double jdate_base, 
        bool& foundNewMoonDate, Date& nextNewMoonDate,
        bool& foundFullMoonDate, Date& nextFullMoonDate )
    {
        double phases[3];
        phasehunt( jdate_base + 0.5, phases );
        
        Date candidates[3];
        for ( int i = 0; i < 3; i++ )
        {
            jyear( phases[i], candidates[i] );
            jhms( phases[i], candidates[i] );
        }

        if ( !foundNewMoonDate && isLater( baseDate, candidates[0] ) )
        {
            nextNewMoonDate = candidates[0];
            foundNewMoonDate = true;
        }
        else
        {
            if ( !foundNewMoonDate && isLater( baseDate, candidates[2] ) )
            {
                nextNewMoonDate = candidates[2];
                foundNewMoonDate = true;
            }
        }

        if ( !foundFullMoonDate && isLater( baseDate, candidates[1] ) )
        {
            nextFullMoonDate = candidates[1];
            foundFullMoonDate = true;
        }    
    }

    double GMST0(double d)
    {
        double sidtim0;
        /* Sidtime at 0h UT = L (Sun's mean longitude) + 180.0 degr  */
        /* L = M + w, as defined in sunpos().  Since I'm too lazy to */
        /* add these numbers, I'll let the C compiler do it for me.  */
        /* Any decent C compiler will add the constants at compile   */
        /* time, imposing no runtime or code overhead.               */
        sidtim0 = rev( ( 180.0 + 356.0470 + 282.9404 ) +
                       ( 0.9856002585 + 4.70935E-5 ) * d );
        return sidtim0;
    }

    double rev180(double x)
    {
        return ( x - 360.0 * floor( x / 360.0 + 0.5 ) );
    }

    CelestialObjectRiseSet calcSunRiseSet(const Date& date, double lat, double lon)
    {
        double altit = -35.0 / 60.0;
        int upper_limb = 1;

        /***************************************************************************/
        /* Note: year,month,date = calendar date, 1801-2099 only.             */
        /*       Eastern longitude positive, Western longitude negative       */
        /*       Northern latitude positive, Southern latitude negative       */
        /*       The longitude value IS critical in this function!            */
        /*       altit = the altitude which the Sun should cross              */
        /*               Set to -35/60 degrees for rise/set, -6 degrees       */
        /*               for civil, -12 degrees for nautical and -18          */
        /*               degrees for astronomical twilight.                   */
        /*         upper_limb: non-zero -> upper limb, zero -> center         */
        /*               Set to non-zero (e.g. 1) when computing rise/set     */
        /*               times, and to zero when computing start/end of       */
        /*               twilight.                                            */
        /*        *rise = where to store the rise time                        */
        /*        *set  = where to store the set  time                        */
        /*                Both times are relative to the specified altitude,  */
        /*                and thus this function can be used to compute       */
        /*                various twilight times, as well as rise/set times   */
        /* Return value:  0 = sun rises/sets this day, times stored at        */
        /*                    *trise and *tset.                               */
        /*               +1 = sun above the specified "horizon" 24 hours.     */
        /*                    *trise set to time when the sun is at south,    */
        /*                    minus 12 hours while *tset is set to the south  */
        /*                    time plus 12 hours. "Day" length = 24 hours     */
        /*               -1 = sun is below the specified "horizon" 24 hours   */
        /*                    "Day" length = 0 hours, *trise and *tset are    */
        /*                    both set to the time when the sun is at south.  */
        /*                                                                    */
        /**********************************************************************/
        double d, /* Days since 2000 Jan 0.0 (negative before) */
        sr, /* Solar distance, astronomical units */
        sRA, /* Sun's Right Ascension */
        sdec, /* Sun's declination */
        sradius, /* Sun's apparent radius */
        t, /* Diurnal arc */
        tsouth, /* Time when Sun is at south */
        sidtime; /* Local sidereal time */

        int rc = 0; /* Return cde from function - usually 0 */

        /* Compute d of 12h local mean solar time */
        d = convertDateToDayNumber( date ) + 0.5 - lon / 360.0;

        /* Compute local sidereal time of this moment */
        sidtime = rev( GMST0( d ) + 180.0 + lon );

        /* Compute Sun's RA + Decl at this moment */
        //inlined: sun_RA_dec( d, &sRA, &sdec, &sr );
        double sunLon, obl_ecl, x, y, z;

        /* Compute Sun's ecliptical coordinates */
        // inlined: sunLon = sunpos( d, r );
        // Compute mean elements
        // Mean anomaly of the Sun
        double M = rev( 356.0470 + 0.9856002585 * d );
        // Mean longitude of perihelion
        double w = 282.9404 + 4.70935E-5 * d;
        // Eccentricity of Earth's orbit
        double e = 0.016709 - 1.151E-9 * d;

        // Compute true longitude and radius vector
        // Eccentric anomaly
        double E = M + e * RADEG * sind( M ) * ( 1.0 + e * cosd( M ) );

        // x, y coordinates in orbit
        double orb_x = cosd( E ) - e;
        double orb_y = sqrt( 1.0 - e * e ) * sind( E );

        //Solar distance
        sr = sqrt( orb_x * orb_x + orb_y * orb_y );

        // True anomaly
        double v = atan2d( orb_y, orb_x );

        // True solar longitude
        sunLon = v + w;
        if ( sunLon >= 360.0 )
        {
            sunLon -= 360.0; /* Make it 0..360 degrees */
        }

        /* Compute ecliptic rectangular coordinates (z=0) */
        x = sr * cosd( sunLon );
        y = sr * sind( sunLon );

        /* Compute obliquity of ecliptic (inclination of Earth's axis) */
        obl_ecl = 23.4393 - 3.563E-7 * d;

        /* Convert to equatorial rectangular coordinates - x is unchanged */
        z = y * sind( obl_ecl );
        y = y * cosd( obl_ecl );

        /* Convert to spherical coordinates */
        sRA = atan2d( y, x );
        sdec = atan2d( z, sqrt( x * x + y * y ) );

        /* Compute time when Sun is at south - in hours UT */
        tsouth = 12.0 - rev180( sidtime - sRA ) / 15.0;

        /* Compute the Sun's apparent radius, degrees */
        sradius = 0.2666 / sr;

        /* Do correction to upper limb, if necessary */
        if ( upper_limb > 0 )
        {
            altit -= sradius;
        }

        /* Compute the diurnal arc that the Sun traverses to reach */
        /* the specified altitude altit: */
        {
            double cost;
            cost = ( sind( altit ) - sind( lat ) * sind( sdec ) ) /
                    ( cosd( lat ) * cosd( sdec ) );
            if ( cost >= 1.0 )
            {
                rc = -1;
                t = 0.0; /* Sun always below altit */
            }
            else
            {
                if ( cost <= -1.0 )
                {
                    rc = +1;
                    t = 12.0; /* Sun always above altit */
                }
                else
                {
                    t = acosd( cost ) / 15.0; /* The diurnal arc, hours */
                }
            }
        }

        CelestialObjectRiseSet result;

        /* Store rise and set times - in hours UT */
        result.rise = tsouth - t;
        result.set = tsouth + t;

        switch ( rc )
        {
            case 0:
                result.type = CelestialObjectRiseSet::Normal;
                break;
            case -1:
                result.type = CelestialObjectRiseSet::AllDayBelowHorizon;
                break;
            case 1:
                result.type = CelestialObjectRiseSet::AllDayAboveHorizon;
                break;
        }

        return result;
    }

    //
    // http://bodmas.org/kepler/js_sunrise_moonrise.html
    //
    double frac( double x ) 
    {
    //
    //	returns the fractional part of x as used in minimoon and minisun
    //
	    double a = x - floor(x);
	    if (a < 0) 
        {
            a += 1;
        }

	    return a;
    }

    int ipart( double x ) 
    {
    //
    //	returns the integer part - like int() in basic
    //
	    int a;
	    if ( x > 0 ) 
        {
	        a = (int) floor( x );
	    }
	    else 
        {
		    a = (int) ceil( x );
	    }

	    return a;
    }

    double range( double x ) 
    {
    //
    //	returns an angle in degrees in the range 0 to 360
    //
	    double b = x / 360;
	    double a = 360 * (b - ipart(b));
	    if (a  < 0 ) 
        {
		    a = a + 360;
	    }
    	
        return a;
    }

    double lmst( double mjd, double glong ) 
    {
    //
    //	Takes the mjd and the longitude (west negative) and then returns
    //  the local sidereal time in hours. Im using Meeus formula 11.4
    //  instead of messing about with UTo and so on
    //
	    double d = mjd - 51544.5;
	    double t = d / 36525.0;
	    double lst = range(280.46061837 + 360.98564736629 * d + 0.000387933 *t*t - t*t*t / 38710000);
	    return (lst/15.0 + glong/15);
    }

    void minimoon( double t, double& dec, double& ra ) 
    {
    //
    // takes t and returns the geocentric ra and dec in an array mooneq
    // claimed good to 5' (angle) in ra and 1' in dec
    // tallies with another approximate method and with ICE for a couple of dates
    //
	    double L0 = frac(0.606433 + 1336.855225 * t);	// mean longitude of moon
	    double L = p2 * frac(0.374897 + 1325.552410 * t); //mean anomaly of Moon
	    double LS = p2 * frac(0.993133 + 99.997361 * t); //mean anomaly of Sun
	    double D = p2 * frac(0.827361 + 1236.853086 * t); //difference in longitude of moon and sun
	    double F = p2 * frac(0.259086 + 1342.227825 * t); //mean argument of latitude

	    // corrections to mean longitude in arcsec
	    double DL =  22640 * sin(L);
	    DL += -4586 * sin(L - 2*D);
	    DL += +2370 * sin(2*D);
	    DL +=  +769 * sin(2*L);
	    DL +=  -668 * sin(LS);
	    DL +=  -412 * sin(2*F);
	    DL +=  -212 * sin(2*L - 2*D);
	    DL +=  -206 * sin(L + LS - 2*D);
	    DL +=  +192 * sin(L + 2*D);
	    DL +=  -165 * sin(LS - 2*D);
	    DL +=  -125 * sin(D);
	    DL +=  -110 * sin(L + LS);
	    DL +=  +148 * sin(L - LS);
	    DL +=   -55 * sin(2*F - 2*D);

	    // simplified form of the latitude terms
	    double S = F + (DL + 412 * sin(2*F) + 541* sin(LS)) / arc;
	    double H = F - 2*D;
	    double N =   -526 * sin(H);
	    N +=   +44 * sin(L + H);
	    N +=   -31 * sin(-L + H);
	    N +=   -23 * sin(LS + H);
	    N +=   +11 * sin(-LS + H);
	    N +=   -25 * sin(-2*L + F);
	    N +=   +21 * sin(-L + F);

	    // ecliptic long and lat of Moon in rads
	    double L_moon = p2 * frac(L0 + DL / 1296000);
	    double B_moon = (18520.0 * sin(S) + N) /arc;

	    // equatorial coord conversion - note fixed obliquity
	    double CB = cos(B_moon);
	    double X = CB * cos(L_moon);
	    double V = CB * sin(L_moon);
	    double W = sin(B_moon);
	    double Y = coseps * V - sineps * W;
	    double Z = sineps * V + coseps * W;
	    double RHO = sqrt(1.0 - Z*Z);

	    dec = (360.0 / p2) * atan(Z / RHO);
	    ra = (48.0 / p2) * atan(Y / (X + RHO));
	    if (ra <0 ) 
        {
            ra += 24;
        }
    }

    double sin_alt( double mjd0, double hour, double glong, double cglat, double sglat ) 
    {
    //
    //	this rather mickey mouse function takes a lot of
    //  arguments and then returns the sine of the altitude of
    //  the object labelled by iobj. iobj = 1 is moon, iobj = 2 is sun
    //
	    double mjd = mjd0 + hour/24.0;
	    double t = (mjd - 51544.5) / 36525.0;
        double ra = 0.0;
        double dec = 0.0;
        
        minimoon( t, dec, ra );

        // hour angle of object
	    double tau = 15.0 * (lmst(mjd, glong) - ra);
	    // sin(alt) of object using the conversion formulas
	    return sglat * sin(rads*dec) + cglat * cos(rads*dec) * cos(rads*tau);
    }

    void quad( double ym, double yz, double yp, double& nz,
	    double& z1, double& z2, double& xe, double& ye )
    {
    //
    //	finds the parabola throuh the three points (-1,ym), (0,yz), (1, yp)
    //  and returns the coordinates of the max/min (if any) xe, ye
    //  the values of x where the parabola crosses zero (roots of the quadratic)
    //  and the number of roots (0, 1 or 2) within the interval [-1, 1]
    //
    //	well, this routine is producing sensible answers
    //
    //  results passed as array [nz, z1, z2, xe, ye]
    //

	    nz = 0;
	    double a = 0.5 * (ym + yp) - yz;
	    double b = 0.5 * (yp - ym);
	    double c = yz;

	    xe = -b / (2 * a);
	    ye = (a * xe + b) * xe + c;
    	
        const double dis = b * b - 4.0 * a * c;
	    if ( dis > 0 )	
        {
		    const double dx = 0.5 * sqrt( dis ) / fabs( a );
		    z1 = xe - dx;
		    z2 = xe + dx;
    		
            if ( fabs( z1 ) <= 1.0 ) 
            {
                nz += 1;
            }
    		
            if ( fabs( z2 ) <= 1.0 ) 
            {
                nz += 1;
            }

		    if ( z1 < -1.0 ) 
            {
                z1 = z2;
            }
	    }
    }

    CelestialObjectRiseSet find_moonrise_set( double mjd, double tz, double lat, double lon ) 
    {
        CelestialObjectRiseSet result;

	    double yz = 0.0;
        double utrise = 0.0;
        double utset = 0.0;

	    double sinho = sin(rads * 8/60);		//moonrise taken as centre of moon at +8 arcmin
	    double sglat = sin(rads * lat);
	    double cglat = cos(rads * lat);

	    double date = mjd - tz/24;
	    bool rise = false;
	    bool sett = false;
	    bool above = false;
	    double hour = 1.0;

	    double ym = sin_alt(date, hour - 1.0, lon, cglat, sglat) - sinho;
	    if ( ym > 0.0 ) 
        {
            above = true;
        }

	    double yp, nz, z1, z2, xe, ye;

        while ( ( hour < 25 ) && ( ( sett == false ) || ( rise == false ) ) ) 
        {
		    yz = sin_alt( date, hour, lon, cglat, sglat) - sinho;
		    yp = sin_alt( date, hour + 1.0, lon, cglat, sglat) - sinho;
    		
            quad(ym, yz, yp, nz, z1, z2, xe, ye );

		    // case when one event is found in the interval
		    if ( nz == 1 ) 
            {
			    if ( ym < 0.0 ) 
                {
				    utrise = hour + z1;
				    rise = true;
			    }
			    else 
                {
				    utset = hour + z1;
				    sett = true;
			    }
		    } // end of nz = 1 case

		    // case where two events are found in this interval
		    // (rare but whole reason we are not using simple iteration)
		    if ( nz == 2 ) 
            {
			    if ( ye < 0.0 ) 
                {
				    utrise = hour + z2;
				    utset = hour + z1;
			    }
			    else 
                {
				    utrise = hour + z1;
				    utset = hour + z2;
			    }
		    }

		    // set up the next search interval
		    ym = yp;
		    hour += 2.0;

	    } // end of while loop

	    if ( ( rise == true ) || ( sett == true ) )
        {
            result.type = CelestialObjectRiseSet::Normal;

            if (rise == true) 
            {
                result.rise = utrise;
            }
            else
            {
                result.type = CelestialObjectRiseSet::RiseInvalid;
            }

            if (sett == true) 
            {
                result.set = utset;
		    }
            else
            {
                result.type = CelestialObjectRiseSet::SetInvalid;
            }
        }
	    else 
        {
            result.type = above ? CelestialObjectRiseSet::AllDayAboveHorizon : CelestialObjectRiseSet::AllDayBelowHorizon;
	    }

	    return result;
    }

    double mjd( int day, int month, int year, double hour ) 
    {
    //
    //	Takes the day, month, year and hours in the day and returns the
    //  modified julian day number defined as mjd = jd - 2400000.5
    //  checked OK for Greg era dates - 26th Dec 02
    //
	    if ( month <= 2 ) 
        {
		    month = month + 12;
		    year = year - 1;
	    }
    	
        double a = 10000.0 * year + 100.0 * month + day;
        double b = 0.0;

	    if ( a <= 15821004.1 ) 
        {
		    b = -2 * floor( ( year + 4716.0)/4.0 ) - 1179;
	    }
	    else 
        {
		    b = floor( year/400.0 ) - floor( year/100.0 ) + floor( year/4.0 );
	    }
    	
        a = 365.0 * year - 679004.0;
    	
        return ( a + b + floor( 30.6001 * ( month + 1 ) ) + day + hour/24.0 );
    }

    CelestialObjectRiseSet calcMoonRiseSet(const Date& date, double utcdiff, double lat, double lon)
    {
        double mj = mjd( date.day, date.month, date.year, 0.0);

        return find_moonrise_set( mj, utcdiff, lat, lon );
    }

}


