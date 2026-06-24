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

int getMoonphase(int y, int m, int d)
{
    // Gregorian JDN (Meeus) – all integer steps before the final floating‑point
    if (m <= 2) {
        y--;
        m += 12;
    }
    long long a = y / 100;
    long long b = 2 - a + a / 4;
    double jd = static_cast<long long>(365.25 * (y + 4716)) +
                static_cast<long long>(30.6001 * (m + 1)) +
                d + b - 1524 + 0.5;   // midnight UTC

    // Reference new moon: 2000‑01‑06 18:14 UTC ≈ JDN 2451550.09765
    const double epoch = 2451550.09765;
    const double synMonth = 29.530588853;

    double days = jd - epoch;
    double cycles = days / synMonth;
    double fraction = cycles - floor(cycles);
    if (fraction < 0.0) fraction += 1.0;

    int phase = static_cast<int>(round(fraction * 8));
    return phase % 8;
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