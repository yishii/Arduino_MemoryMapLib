/*
  Open Accessory Stream class
  Author : Yasuhiro ISHII
*/

#ifndef _ANDROIDACCESSORYSTREAM_H_
#define _ANDROIDACCESSORYSTREAM_H_

//#include "testlibs.h"

#include "Max3421e.h"
#include "Usb.h"
#include "AndroidAccessory.h"
#include "CommunicationStream.h"
#include "BufferedStream.h"
#include "WProgram.h"

class AndroidAccessoryStream : public CommunicationStream
{
private:
    int mConnected;
    AndroidAccessory* mAndroidAccessory;
    BufferedStream* mBufferedStream;

public:
    AndroidAccessoryStream(void);
    int isConnected(void);
    unsigned char read(void);
    int write(unsigned char* ,int );
    void setInterface(AndroidAccessory* );
    int available(void);
    void flush(void);
};

#endif /* _ANDROIDACCESSORYSTREAM_H_ */
