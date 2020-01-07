//******************************************************************************
// Modes.h
//******************************************************************************

#ifndef MODES_H
#define MODES_H

typedef enum eMode : uint8_t
{
	MODE_TIME,
#ifdef SHOW_MODE_AMPM
	MODE_AMPM,
#endif
#ifdef SHOW_MODE_SECONDS
	MODE_SECONDS,
#endif
#ifdef SHOW_MODE_WEEKDAY
	MODE_WEEKDAY,
#endif
#ifdef SHOW_MODE_DATE
	MODE_DATE,
#endif
#ifdef SHOW_MODE_MOONPHASE
	MODE_MOONPHASE,
#endif
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	MODE_TEMP,
#endif
#ifdef SENSOR_DHT22
	MODE_HUMIDITY,
#endif
#ifdef APIKEY
	MODE_EXT_TEMP,
	MODE_EXT_HUMIDITY,
#endif
#ifdef BUZZER
	MODE_TIMER,
#endif
#ifdef SHOW_MODE_TEST
	MODE_TEST,
	MODE_RED,
	MODE_GREEN,
	MODE_BLUE,
	MODE_WHITE,
#endif
	MODE_COUNT,
	MODE_BLANK,
	MODE_FEED
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
