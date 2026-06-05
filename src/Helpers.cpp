#include "Helpers.h"

struct tm getTime()
{
    time_t now = time(nullptr);
    struct tm *tmNow = localtime(&now);

    // Return by value (this creates a local copy so it's thread-safe)
    if (tmNow == nullptr)
    {
        return {}; // Return empty struct if error
    }
    return *tmNow;
}

uint8_t getHour(time_t zeit)
{
    return static_cast<uint8_t>((zeit % 86400) / 3600);
}

uint8_t getMinute(time_t zeit)
{
    return static_cast<uint8_t>((zeit % 3600) / 60);
}

// Calculate moonphase
int getMoonphase(int y, int m, int d)
{
    // Use double for everything to prevent precision loss/overflow
    double jd;

    if (m < 3)
    {
        y--;
        m += 12;
    }
    m++; // Adjusting for calculation logic

    // More precise Julian Date calculation (simplified astronomical epoch)
    // This is a standard approximation
    jd = 365.25 * y + 30.6001 * m + d + 1720994.5;

    // The actual mean synodic month: 29.530588853 days
    double phase = jd / 29.530588853;

    // Get the fractional part of the cycle (0.0 to 1.0)
    double fraction = phase - floor(phase);

    // Map 0.0-1.0 to 0-7 scale
    int b = (int)round(fraction * 8);
    return b % 8;
}

String padStringZeros(String input)
{
    return (input.length() > 1) ? input : "0" + input;
}

// Set ESP time from Webpage
void handleTimeSetting(String input) // 2026-05-01T17:58
{
    int year, month, day, hour, minute, second = 0;
    int parsed = sscanf(input.c_str(), "%d-%d-%dT%d:%d", &year, &month, &day, &hour, &minute);

    if (parsed == 5)
    {
        struct tm t;
        t.tm_year = year - 1900;
        t.tm_mon = month - 1;
        t.tm_mday = day;
        t.tm_hour = hour;
        t.tm_min = minute;
        t.tm_sec = second;
        t.tm_isdst = -1;

        time_t epoch = mktime(&t);

        if (epoch != -1)
        {
            struct timeval tv;
            tv.tv_sec = epoch;
            tv.tv_usec = 0;
            settimeofday(&tv, NULL);
            DEBUG_SERIAL_PRINTLN(F("[INFO] Manual time set to: ") + input);
        }
        else
        {
            DEBUG_SERIAL_PRINTLN(F("[ERROR] Wrong date"));
        }
    }
    else
    {
        DEBUG_SERIAL_PRINTLN(F("[ERROR] Wrong format"));
    }
}

// Used on Webpage to show the total uptime
String formatUptime(time_t total_seconds)
{
    long days = total_seconds / 86400;
    long remaining_sec = total_seconds % 86400;
    int hours = remaining_sec / 3600;
    remaining_sec %= 3600;
    int minutes = remaining_sec / 60;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld days, %02d:%02dh", days, hours, minutes);
    return String(buffer);
}