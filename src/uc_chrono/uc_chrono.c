/**
  * @file       uc_chrono.c
  * @author     Simon Burkhardt github.com/mnemocron
  * @copyright  
  * @date       05.03.2020
  * @brief      C functions to work with UNIX timestamps
  * @details
  * @see        https://howardhinnant.github.io/date_algorithms.html
  */

#include "uc_chrono.h"

// Returns number of days since civil 1970-01-01.  Negative values indicate
//    days prior to 1970-01-01.
// Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
//                 m is in [1, 12]
//                 d is in [1, last_day_of_month(y, m)]
//                 y is "approximately" in
//                   [numeric_limits<Int>::min()/366, numeric_limits<Int>::max()/366]
//                 Exact range of validity is:
//                 [civil_from_days(numeric_limits<Int>::min()),
//                  civil_from_days(numeric_limits<Int>::max()-719468)]
int days_from_civil(int y, unsigned m, unsigned d)
{
    /** @TODO: make it an uint32_t */
    y -= m <= 2;
    const int era = (y >= 0 ? y : y-399) / 400;
    const unsigned yoe = (unsigned)(y - era * 400);      // [0, 399]
    const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
    const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
    return era * 146097 + (int)(doe) - 719468;
}

int seconds_from_civil(int y, unsigned m, unsigned d, unsigned h, unsigned min, unsigned s)
{
    int seconds = days_from_civil(y, m, d) * 24 * 3600;
    seconds += h * 3600;
    seconds += min * 60;
    seconds += s;
    return seconds;
}

// Returns year/month/day triple in civil calendar
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-719468].
UNIXTimestamp_t civil_from_days(int z)
{
    /** @TODO: make it an uint32_t */
    z += 719468;
    const int era = (z >= 0 ? z : z - 146096) / 146097;
    const unsigned doe = (unsigned)(z - era * 146097);          // [0, 146096]
    const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
    const int y = (int)(yoe) + era * 400;
    const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
    const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
    const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
    const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
    UNIXTimestamp_t ts;
    ts.year = (y + (m <= 2));
    ts.month = m;
    ts.day = d;
    return ts;
}

UNIXTimestamp_t civil_from_seconds(int z)
{
    UNIXTimestamp_t ts = civil_from_days(z / 24 / 3600);
    int dayseconds = 24*3600*days_from_civil(ts.year, ts.month, ts.day);
    ts.hour = (z-dayseconds)/3600;
    ts.minute = ((z-dayseconds)-(3600*ts.hour))/60;
    ts.second = z-dayseconds - ts.hour*3600 - ts.minute*60;
    return ts;
}

// Returns day of week in civil calendar [0, 6] -> [Sun, Sat]
// Preconditions:  z is number of days since 1970-01-01 and is in the range:
//                   [numeric_limits<Int>::min(), numeric_limits<Int>::max()-4].
unsigned weekday_from_days(int z)
{
    return (unsigned)(z >= -4 ? (z+4) % 7 : (z+5) % 7 + 6);
}

// Preconditions: x <= 6 && y <= 6
// Returns: The number of days from the weekday y to the weekday x.
// The result is always in the range [0, 6].
unsigned weekday_difference(unsigned x, unsigned y)
{
    x -= y;
    return x <= 6 ? x : x + 7;
}

// Preconditions: wd <= 6
// Returns: The weekday following wd
// The result is always in the range [0, 6].
unsigned next_weekday(unsigned wd)
{
    return wd < 6 ? wd+1 : 0;
}

// Preconditions: wd <= 6
// Returns: The weekday prior to wd
// The result is always in the range [0, 6].
unsigned prev_weekday(unsigned wd)
{
    return wd > 0 ? wd-1 : 6;
}

