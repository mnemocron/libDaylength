/**
  * @file       unit-tests.c
  * @author     Simon Burkhardt github.com/mnemocron
  * @copyright  
  * @date       05.03.2020
  * @brief      C functions to work with UNIX timestamps
  * @details
  * @see        https://howardhinnant.github.io/date_algorithms.html
  */

#include <stdio.h>
#include <math.h>
#include "../src/uc_chrono/uc_chrono.h"
#include "../src/lib_daylength/lib_daylength.h"

const unsigned int timestamp = 1583418710;

int main()
{
    printf("\n[TESTING uC CHRONO]\n");
    printf("Testing Date: 2020-03-05\n");
    int tst = days_from_civil(2020, 03, 05);
    printf("Timestamp is: %d\n", 24*3600*tst);
    printf("Timestamp is: %d\n", timestamp);
    UNIXTimestamp_t ts = civil_from_days(tst);
    printf("Date is: %d-%d-%d\n", ts.year, ts.month, ts.day);
    printf("Weekday: %d\n", weekday_from_days(tst));


    int year = 2020;
    int month = 03;
    int day = 5;
    int hour = 16;
    int minute = 28;
    int second = 59;
    tst = seconds_from_civil(year, month, day, hour, minute, second);
    printf("New Timestamp: %d\n", tst);
    ts = civil_from_seconds(tst);
    printf("Date in : %d-%d-%d %d:%d:%d\n", year, month, day, hour, minute, second);
    printf("Date out: %d-%d-%d %d:%d:%d\n", ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second);

    ts = getSunrise(&ts, 39.040759, 77.04876);
    printf("Sunrise: %d-%d-%d %d:%d:%d\n", ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second);
    ts = getSunset(&ts, 39.040759, 77.04876);
    printf("Sunset : %d-%d-%d %d:%d:%d\n", ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second);

    UNIXTimestamp_t tset, trise, ttrans;
    for(int imon=1; imon<13; imon++){
        for(int iday=1; iday<32; iday++){
            month = imon;
            day = iday;
            tst = seconds_from_civil(year, month, day, hour, minute, second);
            ts = civil_from_seconds(tst);
            tset = getSunset(&ts, 47.207447, 7.537179);
            trise = getSunrise(&ts, 47.207447, 7.537179);
            ttrans = getSunTransit(&ts, 47.207447, 7.537179);
            printf("%d-%02d-%02d,", ts.year, ts.month, ts.day);
            printf("%02d:%02d:%02d,", trise.hour, trise.minute, trise.second);
            printf("%02d:%02d:%02d,", ttrans.hour, ttrans.minute, ttrans.second);
            printf("%02d:%02d:%02d\n", tset.hour, tset.minute, tset.second);
            if(imon == 2 && iday == 29) iday = 40;
            if( (imon == 4 || imon ==  6 || imon == 9 || imon == 11) && iday == 30) iday = 40;
        }
    }
}

