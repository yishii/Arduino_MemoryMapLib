/*
  Communication Stream class

  Author : Yasuhiro ISHII
*/

#ifndef _COMMUNICATIONSTREAM_H_
#define _COMMUNICATIONSTREAM_H_

class CommunicationStream
{
private:
    
public:
    virtual int isConnected(void);
    virtual unsigned char read(void);
    virtual int write(unsigned char* ,int );
    virtual int available(void);
    virtual void flush(void);
};

#endif /* COMMUNICATIONSTREAM_H_ */
