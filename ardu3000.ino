#include "Servo.h"
#include "SimpleSerialProtocol.h"

#define BAUDRATE 115200

#define LED_PIN 13

#define STEERING_PIN 5
#define STEERING_MIN 60
#define STEERING_MAX 120
#define STEERING_DEFAULT 90

#define THROTTLE_PIN 6
#define THROTTLE_MIN 0
#define THROTTLE_MAX 180
#define THROTTLE_DEFAULT 90

#define MSG_HEARTBEAT 0
#define MSG_STEERING  1
#define MSG_THROTTLE  2

SimpleSerialProtocol com;

Servo steering;
Servo throttle;

int hbc = 0;
//----------------------------------------------------------------------------//
void failsafe()
{
    steering.write(STEERING_DEFAULT);
    throttle.write(THROTTLE_DEFAULT);

    while(true)
    {
        delay(100);
        digitalWrite(LED_PIN, HIGH);
        delay(100);
        digitalWrite(LED_PIN, LOW);
    }
}
//----------------------------------------------------------------------------//
int range(int value, int min, int max)
{
    if(value < min)
        return min;
    if(value > max)
        return max;
    return value;
}
//----------------------------------------------------------------------------//
void setup()
{
    Serial1.begin(BAUDRATE);
    while(!Serial1){;}

    pinMode(LED_PIN, OUTPUT);

    steering.attach(STEERING_PIN);
    steering.write(STEERING_DEFAULT);

    throttle.attach(THROTTLE_PIN);
    throttle.write(THROTTLE_DEFAULT);
}
//----------------------------------------------------------------------------//
void loop()
{
    if(com.refresh())
    {
        switch(com.get_type())
        {
            case MSG_HEARTBEAT:
                hbc = 0;
                break;
            case MSG_STEERING:
                if(com.has_data())
                    steering.write(range(com.get_data(), STEERING_MIN, STEERING_MAX));
                break;
            case MSG_THROTTLE:
                if(com.has_data())
                    throttle.write(range(com.get_data(), THROTTLE_MIN, THROTTLE_MAX));
                break;
        }
    }

    if(hbc > 500)
        failsafe();

    //delay(10);
    //hbc++;
}
//----------------------------------------------------------------------------//
