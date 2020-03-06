/**
  * @file       lib_daylength.c
  * @author     Simon Burkhardt github.com/mnemocron
  * @copyright  
  * @date       05.03.2020
  * @brief      C functions to calculate sunrise / sunset time
  * @details
  * @see        http://users.electromagnetic.net/bu/astro/iyf-calc.php
  */

#include "lib_daylength.h"
#include "../uc_chrono/uc_chrono.h"
#include <math.h>
#include <stdio.h>

int getJulianDate(int year, int month, int day)
{
    // (days since Jan 1, 2000 + 2451545):
    int days_70_to_00 = days_from_civil(2000, 1, 1)+1;
    int julian = days_from_civil(year, month, day) - days_70_to_00 + 2451545;
    return julian;
}

double getJulianFromUnix(int unixSecs)
{
   return ( unixSecs / 86400.0 ) + 2440587.5;
}

int getUnixFromJulian(double julian)
{
    return (int)((double)(86400.0 * (julian - 2440587.5)));
}

UNIXTimestamp_t getSunEvent(UNIXTimestamp_t *ts, double lon, double lat, SunEvent_t event)
{
    static double _const_9 = 0.0009;
    static double _const_69 = 0.0069;
    static double _const_53 = 0.0053;
    double lw = lat; 
    double ln = lon;
    double Jdate = (double)getJulianDate(ts->year, ts->month, ts->day);
    double n_s = (Jdate - JULIAN_DAY_0 - _const_9) - (lw/360.0);
    int n = round(n_s);

    double J_s = JULIAN_DAY_0 + _const_9 + (lw/360.0) + n;
    double M = fmod((357.5291 + 0.98560028 * (J_s - JULIAN_DAY_0)), 360.0);
    double C = (1.9148 * sin(DEG2RAD*M)) + (0.0200 * sin(DEG2RAD*2 * M)) + (0.0003 * sin(DEG2RAD*3 * M));
    double lam = fmod((M + 102.9372 + C + 180), 360);
    double Jtransit = J_s + (_const_53 * sin(DEG2RAD*M)) - (_const_69 * sin(DEG2RAD*2 * lam));
    for(int i=0; i<4; i++){
        M = fmod((357.5291 + 0.98560028 * (Jtransit - JULIAN_DAY_0)), 360.0);
        Jtransit = J_s + (_const_53 * sin(2*M_PI/360*M)) - (_const_69 * sin(2 * lam));
        C = (1.9148 * sin(DEG2RAD*M)) + (0.0200 * sin(DEG2RAD*2 * M)) + (0.0003 * sin(DEG2RAD*3 * M));
        lam = fmod((M + 102.9372 + C + 180), 360);
        Jtransit = J_s + (_const_53 * sin(DEG2RAD*M)) - (_const_69 * sin(DEG2RAD*2 * lam));
    }
    double rho = RAD2DEG*asin( sin(DEG2RAD*lam) * sin(DEG2RAD*23.45) );
    double H = RAD2DEG*acos( (sin(DEG2RAD*(-0.83)) - sin(DEG2RAD*ln) * sin(DEG2RAD*rho)) / (cos(DEG2RAD*ln) * cos(DEG2RAD*rho)) );
    double Jss = JULIAN_DAY_0 + _const_9 + ((H + lw)/360) + n;
    double Jset = Jss + (_const_53 * sin(DEG2RAD*M)) - (_const_69 * sin(DEG2RAD*2*lam));
    double Jrise = Jtransit - (Jset - Jtransit);

    UNIXTimestamp_t tsss;
    switch(event){
        case SUN_RISE:
            tsss = civil_from_seconds(getUnixFromJulian(Jrise));
            break;
        case SUN_SET:
            tsss = civil_from_seconds(getUnixFromJulian(Jset));
            break;
        default:
            tsss = civil_from_seconds(getUnixFromJulian(Jtransit));
            break;
    }
    return tsss;
}


UNIXTimestamp_t getSunrise(UNIXTimestamp_t *ts, double lon, double lat)
{
    return getSunEvent(ts, lon, lat, SUN_RISE);
}

UNIXTimestamp_t getSunset(UNIXTimestamp_t *ts, double lon, double lat)
{
    return getSunEvent(ts, lon, lat, SUN_SET);
}

UNIXTimestamp_t getSunTransit(UNIXTimestamp_t *ts, double lon, double lat)
{
    return getSunEvent(ts, lon, lat, SUN_TRANSIT);
}
