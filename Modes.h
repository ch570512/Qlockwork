//******************************************************************************
// Modes.h
//******************************************************************************

#ifndef MODES_H
#define MODES_H

typedef enum eMode : uint8_t
{
	MODE_TIME,				// 0
#ifdef SHOW_MODE_AMPM
	MODE_AMPM,				// 1
#endif
#ifdef SHOW_MODE_SECONDS
	MODE_SECONDS,			// 2 if SHOW_MODE_AMPM is set - 1 if not set and so on...
#endif
#ifdef SHOW_MODE_WEEKDAY
	MODE_WEEKDAY,			// 3 if...
#endif
#ifdef SHOW_MODE_DATE
	MODE_DATE,				// 4
#endif
#ifdef SHOW_MODE_MOONPHASE
	MODE_MOONPHASE,			// 5
#endif
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	MODE_TEMP,				// 6
#endif
#ifdef SENSOR_DHT22
	MODE_HUMIDITY,			// 7
#endif
#ifdef APIKEY
	MODE_EXT_TEMP,			// 8
	MODE_EXT_HUMIDITY,		// 9
#endif
#ifdef BUZZER
	MODE_TIMER,				// 10
#endif
#ifdef SHOW_MODE_TEST
	MODE_TEST,				// 11
	MODE_RED,				// 12
	MODE_GREEN,				// 13
	MODE_BLUE,				// 14
	MODE_WHITE,				// 15
#endif
	MODE_COUNT,				// 16
	MODE_BLANK,				// 17
	MODE_FEED				// 18
} Mode;

// Overload the ControlType++ operator.
inline Mode& operator++(Mode& eDOW, int)
{
	const uint8_t i = static_cast<uint8_t>(eDOW) + 1;
	eDOW = static_cast<Mode>((i) % MODE_COUNT);
	return eDOW;
}

enum eTransition
{
	TRANSITION_NORMAL, // 0
	TRANSITION_MOVEUP, // 1
	TRANSITION_FADE    // 2
};

#endif
