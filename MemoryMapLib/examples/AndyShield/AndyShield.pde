//
// MemoryMapLib Sample sketch
//

// Set 0 here for Serial Interface(Bluetooth I/F),otherwise for Android Open Accessory(ADK)
#if 1
#define ADK_ENABLE
#else
#define SERIAL_ENABLE
#endif

#ifdef ADK_ENABLE
#include <Max3421e.h>
#include <Usb.h>
#include <AndroidAccessory.h>
#include <AndroidAccessoryStream.h>
#endif

#ifdef SERIAL_ENABLE
#include <SerialStream.h>
#endif

#include <MemoryMapLib.h>


#define PIN_MOTOR_L_VREF      6
#define PIN_MOTOR_R_VREF      5
#define PIN_MOTOR_L_CONTROL1  A0
#define PIN_MOTOR_L_CONTROL2  A1
#define PIN_MOTOR_R_CONTROL1  A3
#define PIN_MOTOR_R_CONTROL2  A2
#define MOTORTYPE_MOTOR_R    0
#define MOTORTYPE_MOTOR_L    1

MemoryMap mMemoryMap;

#ifdef SERIAL_ENABLE
SerialStream mSerialStream;
#endif

#ifdef ADK_ENABLE
AndroidAccessory mAndroidAccessory("ISHII Works",
		     "AndyTank",
		     "AndyTank",
		     "1.0",
		     "http://www.andytank.org/",
		     "0000000012345678");
AndroidAccessoryStream mAndroidAccessoryStream;
#endif

void setup()
{
    // for Andy shield rev.1 Hardware bug
    pinMode(4,INPUT);
    digitalWrite(4,LOW);
    
    pinMode(PIN_MOTOR_R_VREF,OUTPUT);
    pinMode(PIN_MOTOR_L_VREF,OUTPUT);
    pinMode(PIN_MOTOR_R_CONTROL1,OUTPUT);
    pinMode(PIN_MOTOR_R_CONTROL2,OUTPUT);
    pinMode(PIN_MOTOR_L_CONTROL1,OUTPUT);
    pinMode(PIN_MOTOR_L_CONTROL2,OUTPUT);
    
    digitalWrite(PIN_MOTOR_R_CONTROL1,LOW);
    digitalWrite(PIN_MOTOR_R_CONTROL2,LOW);
    digitalWrite(PIN_MOTOR_L_CONTROL1,LOW);
    digitalWrite(PIN_MOTOR_L_CONTROL2,LOW);
    
    analogWrite(PIN_MOTOR_R_VREF,0);
    analogWrite(PIN_MOTOR_L_VREF,0);

#ifdef SERIAL_ENABLE
    Serial.begin(9600);
    Serial.flush();
    mSerialStream.setInterface(&Serial);
    mMemoryMap.setStreamInterface(&mSerialStream);
#endif

#ifdef ADK_ENABLE
    mAndroidAccessoryStream.setInterface(&mAndroidAccessory);
    mMemoryMap.setStreamInterface(&mAndroidAccessoryStream);
#endif

    mMemoryMap.registerMapAddressJob(0x1b,OPERATION_WRITE,&jobMotor);
    mMemoryMap.registerMapAddressJob(0x1c,OPERATION_WRITE,&jobMotor);
}

void loop()
{
    mMemoryMap.poll();
}

void jobMotor(unsigned char RWOP,unsigned char addr,unsigned char* value)
{
    struct S_MOTOR_BIT_FIELD {
	unsigned char power :6;
	unsigned char cont :2;
    };
    
    if(RWOP & OPERATION_WRITE){
	unsigned char cont,power;
	cont  = ((struct S_MOTOR_BIT_FIELD*)value)->cont;
	power = ((struct S_MOTOR_BIT_FIELD*)value)->power << 2;
	controlMotorPower(addr == MEMMAP_ADR_MMPK_Motor_0 ? MOTORTYPE_MOTOR_R : MOTORTYPE_MOTOR_L,cont,power);
    }
}

void controlMotorPower(int motortype,int motoraction,int power)
{
    int pinno_1;
    int pinno_2;
    int pinno_pwm;
    
    const unsigned char cont[4][2] = {
	{ LOW	,	LOW	},	// STOP
	{ LOW	,	HIGH	},	// REVERSE
	{ HIGH	,	LOW	},	// FORWARD
	{ HIGH	,	HIGH	}	// BRAKE
    };
    
    if(motortype == MOTORTYPE_MOTOR_R){
	pinno_1 = PIN_MOTOR_R_CONTROL1;
	pinno_2 = PIN_MOTOR_R_CONTROL2;
	pinno_pwm = PIN_MOTOR_R_VREF;
    } else {
	pinno_1 = PIN_MOTOR_L_CONTROL1;
	pinno_2 = PIN_MOTOR_L_CONTROL2;
	pinno_pwm = PIN_MOTOR_L_VREF;
    }
    digitalWrite(pinno_1,cont[motoraction][0]);
    digitalWrite(pinno_2,cont[motoraction][1]);
    analogWrite(pinno_pwm,power);
}
