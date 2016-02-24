#include "Servo.h"
#include "SimpleSerialProtocol.h"

#define BAUDRATE 115200

#define RC_LED_PIN 3
#define PC_LED_PIN 11

#define STEERING_PIN 5
#define STEERING_MIN 60.0
#define STEERING_MAX 120.0
#define STEERING_DEFAULT 90.0

#define THROTTLE_PIN 6
#define THROTTLE_MIN 87.0
#define THROTTLE_MAX 96.0
#define THROTTLE_DEFAULT 92.0

#define MSG_HEARTBEAT 0
#define MSG_STEERING  1
#define MSG_THROTTLE  2

SimpleSerialProtocol com;

Servo steering, throttle;

unsigned long last_heartbeat;
int last_time_check;

typedef union
{
    uint32_t u;
    int i;
    float f;
} bitfield32;
//----------------------------------------------------------------------------//
void failsafe()
{
    steering.write(STEERING_DEFAULT);
    throttle.write(THROTTLE_DEFAULT);

    while(true)
    {
        delay(100);
        digitalWrite(RC_LED_PIN, HIGH);
        digitalWrite(PC_LED_PIN, HIGH);
        delay(100);
        digitalWrite(RC_LED_PIN, LOW);
        digitalWrite(PC_LED_PIN, LOW);
    }
}
//----------------------------------------------------------------------------//
float range(float value, float min, float max)
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

    pinMode(RC_LED_PIN, OUTPUT);
    pinMode(PC_LED_PIN, OUTPUT);

    steering.attach(STEERING_PIN);
    steering.write(STEERING_DEFAULT);

    throttle.attach(THROTTLE_PIN);
    throttle.write(THROTTLE_DEFAULT);

    //wait for first heartbeat to arm system
    bool armed = false;
    while(!armed)
    {
        if(com.refresh() && com.get_type() == MSG_HEARTBEAT)
        {
                last_heartbeat = millis();
                armed = true;
                last_time_check = 0;
        }
    }
}
//----------------------------------------------------------------------------//
void loop()
{
    if(com.refresh())
    {
        switch(com.get_type())
        {
            case MSG_HEARTBEAT:
                last_heartbeat = millis();
                break;
            case MSG_STEERING:
                if(com.has_data())
                {
                    bitfield32 value;
                    value.u = com.get_data();
                    steering.write(range(value.f, STEERING_MIN, STEERING_MAX));
                }
                break;
            case MSG_THROTTLE:
                {
                    bitfield32 value;
                    value.u = com.get_data();
                    throttle.write(range(value.f, THROTTLE_MIN, THROTTLE_MAX));
                }
                break;
        }
    }

	//GET STATE

	//DISPLAY STATE

	//SWITCH PWM SOURCE

    if(last_time_check >= 10)
    {
        last_time_check = 0;
        if(millis()-last_heartbeat > 200)
        {
            failsafe();
        }
    }
    else
    {
        last_time_check++;
    }
}
//----------------------------------------------------------------------------//
