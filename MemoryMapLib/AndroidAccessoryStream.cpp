/*
  Android Open Accessory Stream class
  Author : Yasuhiro ISHII
*/
#include <stdio.h>
//#include "testlibs.h"

#include "CommunicationStream.h"
#include "AndroidAccessoryStream.h"
#include "BufferedStream.h"

//#include "Max3421e.h"
//#include "Usb.h"
#include "AndroidAccessory.h"
#include "WProgram.h"

unsigned char buffer[128];
BufferedStream aBufferedStream((void*)buffer,sizeof(buffer));

AndroidAccessoryStream::AndroidAccessoryStream(void)
{
    mConnected = false;

    mBufferedStream = &aBufferedStream;
    mBufferedStream->flush();
}

int AndroidAccessoryStream::isConnected(void)
{
    return(mConnected);
}

int AndroidAccessoryStream::available(void)
{
    int i;
    int len;
    unsigned char buff[64];

    len = mAndroidAccessory->read(buff,64,1);
    if(len > 0){
	for(i=0;i<len;i++){
	    mBufferedStream->push(buff[i]);
	}
    }

    return(mBufferedStream->size());
}

unsigned char AndroidAccessoryStream::read(void)
{
    unsigned char ret = 0;

    if(mBufferedStream->size() >= 1){
	mBufferedStream->pop(ret);
    }

    return(ret);
}

int AndroidAccessoryStream::write(unsigned char* buff,int len)
{
    return(mAndroidAccessory->write(buff,len));
}

void AndroidAccessoryStream::flush(void)
{
    mBufferedStream->flush();
}

void AndroidAccessoryStream::setInterface(AndroidAccessory* a)
{
    if(a != NULL){
	mAndroidAccessory = a;
	mAndroidAccessory->powerOn();

	while(mAndroidAccessory->isConnected() == false){
	    delay(10);
	}

	mConnected = true;
    }
}
