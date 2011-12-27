/*
  Serial Stream class
  Author : Yasuhiro ISHII
*/

#ifndef _SERIALSTREAM_H_
#define _SERIALSTREAM_H_

//#include "testlibs.h"
#include "CommunicationStream.h"
#include "WProgram.h"

class SerialStream : public CommunicationStream
{
private:
    int mConnected;
    HardwareSerial* mHardwareSerial;
    
public:
    SerialStream(void);
    int isConnected(void);
    unsigned char read(void);
    int write(unsigned char* ,int );
    void setInterface(HardwareSerial* );
    int available(void);
    void flush(void);
};

#endif /* _SERIALSTREAM_H_ */
