/**
  * @file       lib_daylength.h
  * @author     Simon Burkhardt github.com/mnemocron
  * @copyright  
  * @date       05.03.2020
  * @brief      C functions to calculate sunrise / sunset time
  * @details
  * @see        http://users.electromagnetic.net/bu/astro/iyf-calc.php
  * @note       double precision is critically necessarry to achieve accuracy.
  *             otherwise errors of +- 30 mins may occur
  */

#include "../uc_chrono/uc_chrono.h"
#include <math.h>

#ifndef _LIB_DAYLENGTH
#define _LIB_DAYLENGTH

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#define JULIAN_DAY_0 2451545
#define DEG2RAD  (2*M_PI/360.0)
#define RAD2DEG  (360.0/2/M_PI)

enum Sun_Events{SUN_RISE, SUN_SET, SUN_TRANSIT};
typedef enum Sun_Events SunEvent_t;

int getJulianDate(int year, int month, int day);
double getJulianFromUnix(int unixSecs);
int getUnixFromJulian(double julian);
UNIXTimestamp_t getSunEvent(UNIXTimestamp_t *ts, double lon, double lat, SunEvent_t event);
UNIXTimestamp_t getSunrise(UNIXTimestamp_t *ts, double lon, double lat);
UNIXTimestamp_t getSunset (UNIXTimestamp_t *ts, double lon, double lat);
UNIXTimestamp_t getSunTransit(UNIXTimestamp_t *ts, double lon, double lat);

#endif
