

#include "Arduino.h"

#include "NST1001Driver.h"

void setup()
{
    Serial.begin(115200);
    Serial.println("start...");
    initNST1001(GPIO_NUM_13);
    Serial.println("setup end...");
}

void loop()
{
    Serial.printf("NST1001 temp:%2f \r\n", getNST1001Temp());
    delay(5);
}
