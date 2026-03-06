/**
   Debug
   Klasse fuers elegantere Debuggen.

   @mc       Arduino/RBBB
   @autor    Christian Aschoff / caschoff _AT_ mac _DOT_ com
   @version  1.1
   @created  21.1.2013
*/

#ifdef DEBUG
#define DEBUG_SERIAL_PRINT(x)       Serial.print(x)
#define DEBUG_SERIAL_PRINTLN(x)     Serial.println(x)
#define DEBUG_SERIAL_FLUSH()        Serial.flush()
#else
#define DEBUG_SERIAL_PRINT(x)
#define DEBUG_SERIAL_PRINTLN(x)
#define DEBUG_SERIAL_FLUSH()
#endif