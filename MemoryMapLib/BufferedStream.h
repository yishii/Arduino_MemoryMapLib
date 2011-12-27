/*
  Buffered <Byte> Stream
  Author : Yasuhiro ISHII 2011 for Arduino platform
*/

/*
	リングバッファコントロールクラス - ヘッダ -
	Copyright(C) 2002 Yasuhiro ISHII
	All rights reserved.
*/

#ifndef _RINGBUFF_H
#define _RINGBUFF_H

class BufferedStream {
public:
    BufferedStream (void* ,int );
    ~BufferedStream ();
    
    int size(void);
    int push(unsigned char );
    int pop(unsigned char& );
    int totalsize(void);
    void flush(void);

private:
    unsigned char* buff;
    int datasize;
    int BUF_SIZE;
    int s_ptr;
    int e_ptr;

protected:

};


#endif
