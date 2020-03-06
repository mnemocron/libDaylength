/**
  * @file       uc_chrono.h
  * @author     Simon Burkhardt github.com/mnemocron
  * @copyright  
  * @date       05.03.2020
  * @brief      C functions to work with UNIX timestamps
  * @details
  * @see        https://howardhinnant.github.io/date_algorithms.html
  */

#ifndef _UC_CHRONO
#define _UC_CHRONO

typedef struct {
	int year;
	int month;
	int day;
	int dom;
	int hour;
	int minute;
	int second;
} UNIXTimestamp_t;

int days_from_civil(int y, unsigned m, unsigned d);
UNIXTimestamp_t civil_from_days(int z);
unsigned weekday_from_days(int z);
unsigned weekday_difference(unsigned x, unsigned y);
unsigned next_weekday(unsigned wd);
unsigned prev_weekday(unsigned wd);

int seconds_from_civil(int y, unsigned m, unsigned d, unsigned h, unsigned min, unsigned s);
UNIXTimestamp_t civil_from_seconds(int z);


#endif
