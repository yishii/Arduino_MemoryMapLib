/*
  Serial Stream class
  Author : Yasuhiro ISHII
*/
#include <stdio.h>
//#include "testlibs.h"

#include "CommunicationStream.h"
#include "SerialStream.h"

SerialStream::SerialStream(void)
{
    mConnected = false;
}

int SerialStream::isConnected(void)
{
    return(mConnected);
}

int SerialStream::available(void)
{
    return (mHardwareSerial->available());
}

unsigned char SerialStream::read(void)
{
    return (mHardwareSerial->read());
}

int SerialStream::write(unsigned char* buff,int len)
{
    mHardwareSerial->write(buff,len);
}

void SerialStream::flush(void)
{
    mHardwareSerial->flush();
}

void SerialStream::setInterface(HardwareSerial* s)
{
    if(s != NULL){
	mHardwareSerial = s;
	mConnected = true;
    }
}
