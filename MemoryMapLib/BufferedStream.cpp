/*
  Arduino Library : Buffered <Byte> Stream
  Author : Yasuhiro ISHII 2011 for Arduino platform
*/

/*
	リングバッファコントローラークラス2 for R8C/Tiny
	Copyright(C) 2003 Yasuhiro ISHII
	All rights reserved.
*/

#include "BufferedStream.h"

BufferedStream::BufferedStream(void* buffer,int size_in_bytes){
    buff = (unsigned char*)buffer;
    BUF_SIZE = size_in_bytes;
    datasize = 0;
    s_ptr = 0;
    e_ptr = 0;
}

BufferedStream::~BufferedStream(){

}

int BufferedStream::size(void){
    return (datasize);
}

int BufferedStream::totalsize(void){
    return (BUF_SIZE);
}

int BufferedStream::push(unsigned char data){
    if (datasize < BUF_SIZE){
	buff[s_ptr] = data;
	s_ptr = (s_ptr + 1) % BUF_SIZE;
	datasize++;
	return(1);
    } else {
	return(0);
    }
}

int BufferedStream::pop(unsigned char& data){
    if (datasize){
	data = buff[e_ptr];
	e_ptr = (e_ptr + 1) % BUF_SIZE;
	datasize--;
	return(1);
    } else {
	return(0);
    }
}

void BufferedStream::flush(void){
    datasize = 0;
    s_ptr = 0;
    e_ptr = 0;
}
