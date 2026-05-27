#include "Debug.h"

void debugScreenBuffer(uint16_t screenBuffer[])
{
    const char buchstabensalat[][12] =
        {
            {'E', 'S', 'K', 'I', 'S', 'T', 'A', 'F', 'U', 'N', 'F', '1'},
            {'Z', 'E', 'H', 'N', 'Z', 'W', 'A', 'N', 'Z', 'I', 'G', '2'},
            {'D', 'R', 'E', 'I', 'V', 'I', 'E', 'R', 'T', 'E', 'L', '3'},
            {'V', 'O', 'R', 'F', 'U', 'N', 'K', 'N', 'A', 'C', 'H', '4'},
            {'H', 'A', 'L', 'B', 'A', 'E', 'L', 'F', 'U', 'N', 'F', 'A'},
            {'E', 'I', 'N', 'S', 'X', 'A', 'M', 'Z', 'W', 'E', 'I', ' '},
            {'D', 'R', 'E', 'I', 'P', 'M', 'J', 'V', 'I', 'E', 'R', ' '},
            {'S', 'E', 'C', 'H', 'S', 'N', 'L', 'A', 'C', 'H', 'T', ' '},
            {'S', 'I', 'E', 'B', 'E', 'N', 'Z', 'W', 'O', 'L', 'F', ' '},
            {'Z', 'E', 'H', 'N', 'E', 'U', 'N', 'K', 'U', 'H', 'R', ' '}};
    // Serial.println("\033[0;0H"); // set cursor to 0, 0 position
    Serial.println("+-----------+");
    for (uint8_t y = 0; y <= 9; y++)
    {
        Serial.print('|');
        for (uint8_t x = 0; x <= 10; x++)
        {
            Serial.print((bitRead(screenBuffer[y], 15 - x) ? buchstabensalat[y][x] : ' '));
        }
        Serial.print('|');
        Serial.println((bitRead(screenBuffer[y], 4) ? buchstabensalat[y][11] : ' '));
    }
    Serial.println("+-----------+");
    
    struct tm tmNow = getTime();
    Serial.printf("Time (ESP): %02d:%02d:%02d %02d.%02d.%02d \n", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, tmNow.tm_mday, tmNow.tm_mon + 1, tmNow.tm_year + 1900);

    return;
}