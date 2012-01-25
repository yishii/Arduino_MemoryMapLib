// Memory Map Protocol Library for Arduino
//
// Author : Yasuhiro ISHII
//
// 2011/11/27 : Version 0.01	First release
// 2011/12/24 : Version 0.02    Android Accessory supported.And,modified to have capable to fit for another interfaces.
// 2012/01/25 : Version 0.03    Added to skip received data until receive first data marker(MEMMAP_HEADER1).
//
// The Memory-map protocol is originally created by JS Robotics.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PC_DEBUG
#include "testlibs.h"
#else
//#include "SoftwareSerial.h"
#include "WProgram.h"
#endif

#include "MemoryMapLib.h"
#include "CommunicationStream.h"

int MemoryMap::checkPacketHeader(MEMMAP_Message& msg)
{
    int ret = false;

    if((msg.Field.Header[0] == MEMMAP_HEADER1) &&
       (msg.Field.Header[1] == MEMMAP_HEADER2) &&
       (msg.Field.Length <= MEMMAP_MAX_DATA_LENGTH) &&
       (msg.Field.Cmd >= 1) &&
       (msg.Field.Cmd <= 4)){
	ret = true;
    }

    return ret;
}

int MemoryMap::isReadOperation(MEMMAP_Message& msg)
{
    return(msg.Field.Cmd == MEMMAP_CMD_READ ? true : false);
}

int MemoryMap::checkPacketSum(MEMMAP_Message& msg)
{
    int ret = false;
    int len;
    unsigned char sum;
    int i;

    len  = msg.Field.Length;
    sum  = 0;
    sum  = msg.Field.Cmd;
    sum ^= msg.Field.Address;
    sum ^= msg.Field.Length;

    if(isReadOperation(msg) == false){
	for(i=0;i<len;i++){
	    sum ^= msg.Field.data[i];
	}
    }

    if(sum == msg.Field.Sum){
	ret = true;
    }

    return(ret);
}

int MemoryMap::sendMessage(MEMMAP_Message& msg)
{
    mCommunicationStream->write(msg.Raw,5);
    mCommunicationStream->write(msg.Field.data,msg.Field.Length);
    mCommunicationStream->write(&(msg.Field.Sum),1);
}

int MemoryMap::processCommand(MEMMAP_Message& msg)
{
    int ret = false;
    MEMMAP_Message packetReply;

    switch(msg.Field.Cmd){
    case MEMMAP_CMD_READ:
	processReadCommand(msg,packetReply);
	sendMessage(packetReply);
	break;
    case MEMMAP_CMD_RETURN:
	break;
    case MEMMAP_CMD_WRITE:
	processWriteCommand(msg);
	break;
    case MEMMAP_CMD_ACK:
	break;
    default:
	break;
    }

    return(ret);
}

int MemoryMap::processWriteCommand(MEMMAP_Message& msg)
{
    int counter;
    unsigned char address;
    unsigned char value;

    for(counter=0;counter<msg.Field.Length;counter++){
	address = msg.Field.Address + counter;
	value = msg.Field.data[counter];
	writeMemory(address,value);
    }

    return true;
}

int MemoryMap::processReadCommand(MEMMAP_Message& msg,MEMMAP_Message& msg_reply)
{
    int counter;
    unsigned char address;
    unsigned char value;

    for(counter=0;counter<msg.Field.Length;counter++){
	address = msg.Field.Address + counter;
	value = msg.Field.data[counter];
	if(readMemory(address,&value) == true){
	    // reply to the host
	    msg_reply.Field.data[counter] = value;
	} else {
	    // printf("read method error\n");
	}
    }

    msg_reply.Field.Header[0] = MEMMAP_HEADER1;
    msg_reply.Field.Header[1] = MEMMAP_HEADER2;
    msg_reply.Field.Cmd = MEMMAP_CMD_RETURN;
    msg_reply.Field.Address = msg.Field.Address;
    msg_reply.Field.Length = msg.Field.Length;
    {
	int len;
	unsigned char sum;
	int i;

	len  = msg_reply.Field.Length;
	sum  = 0;
	sum  = msg_reply.Field.Cmd;
	sum ^= msg_reply.Field.Address;
	sum ^= msg_reply.Field.Length;

	for(i=0;i<len;i++){
	    sum ^= msg_reply.Field.data[i];
	}
	msg_reply.Field.Sum = sum;
    }
    
    return true;
}

int MemoryMap::registerMapAddressBlock(Command* tbl)
{
    memcpy((void*)MemoryMapTable,(const void*)tbl,sizeof(MemoryMapTable));
    return(1);
}

int MemoryMap::registerMapAddressJob(unsigned char address,unsigned char RWOP,Job ptr)
{
    int ret = false;

    if(MemoryMapTable[address].ptr == NULL){
	MemoryMapTable[address].RWOP = RWOP | OPERATION_JOB;
	MemoryMapTable[address].ptr = (void*)ptr;
	ret = true;
    } else {
	//printf("cannot register\n");
    }

    return(ret);
}

int MemoryMap::registerMapAddressVar(unsigned char address,unsigned char RWOP,unsigned char* ptr)
{
    int ret = false;

    if(MemoryMapTable[address].ptr == NULL){
	MemoryMapTable[address].RWOP = RWOP;
	MemoryMapTable[address].ptr = (void*)ptr;
	ret = true;
    } else {
	//printf("cannot register\n");
    }
    
    return(ret);
}

int MemoryMap::removeMapAddress(unsigned char address)
{
    int ret = false;

    if(MemoryMapTable[address].ptr != NULL){
	MemoryMapTable[address].RWOP = 0x00;
	MemoryMapTable[address].ptr = NULL;
	ret = true;
    }

    return(true);
}


int MemoryMap::writeMemory(unsigned char address,unsigned char value)
{
    unsigned char RWOP;
    int ret = true;

    RWOP = MemoryMapTable[address].RWOP;
    
    if(RWOP & OPERATION_WRITE){
	if(RWOP & OPERATION_JOB){
	    (*((Job*)&(MemoryMapTable[address].ptr)))(OPERATION_WRITE,address,&value);
	} else {
	    *((unsigned char*)(MemoryMapTable[address].ptr)) = value;
	}
    } else {
	// cannot write onto memory
	ret = false;
    }

    return(ret);
}

int MemoryMap::readMemory(unsigned char address,unsigned char* value)
{
    unsigned char RWOP;
    int ret = true;

    RWOP = MemoryMapTable[address].RWOP;

    if(RWOP & OPERATION_READ){
	if(RWOP & OPERATION_JOB){
	    (*((Job*)&(MemoryMapTable[address].ptr)))(OPERATION_READ,address,value);
	} else {
	    *value = *((unsigned char*)(MemoryMapTable[address].ptr));
	}
    } else {
	// cannot read from memory
	ret = false;
    }

    return(ret);
}

MemoryMap::MemoryMap()
{
    int count_of_slots;
    int i;
    const Command initialCommand = { OPERATION_NOP, NULL };

    // initialize each memory slot
    count_of_slots = sizeof(MemoryMapTable) / sizeof(MemoryMapTable[0]);
    for(i=0;i<count_of_slots;i++){
	memcpy(&(MemoryMapTable[i]),(void*)&initialCommand,sizeof(initialCommand));
    }

    mCommunicationStream = NULL;
}

int MemoryMap::setStreamInterface(CommunicationStream* stream)
{
    int result = false;

    if(stream != NULL){
	mCommunicationStream = stream;
	result = true;
    }

    return (result);
}

void MemoryMap::poll(void)
{
    static SerialReceiveState serial_receive_state = SRECV_IDLE;
    static unsigned long startReceiveTime = 0;
    int counter;
    unsigned char first;

    if(DETECT_TIMEOUT(startReceiveTime) == true){
	serial_receive_state = SRECV_IDLE;
    }
    
    switch(serial_receive_state){
    case SRECV_IDLE:
	if(mCommunicationStream->available() != 0){
	    first = mCommunicationStream->read();
	    if(first == MEMMAP_HEADER1){
		packet.Raw[0] = first;
		serial_receive_state = SRECV_RECEIVING_HEADER;
		startReceiveTime = millis();
	    }
	}
	break;
	
    case SRECV_RECEIVING_HEADER:
	if(mCommunicationStream->available() >= 4){
	    for(counter=1;counter<5;counter++){
		packet.Raw[counter] = mCommunicationStream->read();
	    }
	    if(checkPacketHeader(packet) == true){
		serial_receive_state = SRECV_RECEIVING;
	    } else {
		// data that received might be broken -> flush
		mCommunicationStream->flush();
		serial_receive_state = SRECV_IDLE;
	    }
	}
	break;
	
    case SRECV_RECEIVING:
	if(isReadOperation(packet) == false){
	    if((mCommunicationStream->available()) >= (packet.Field.Length+1)){
		for(counter=0;counter < (packet.Field.Length+1);counter++){
		    if(counter != (packet.Field.Length)) {
			packet.Field.data[counter] = mCommunicationStream->read();
		    } else {
			packet.Field.Sum = mCommunicationStream->read();
		    }
		}
		if(checkPacketSum(packet) == true){
		    processCommand(packet);
		} else {
		    // data that received might be broke -> flush
		    mCommunicationStream->flush();
		}
		serial_receive_state = SRECV_IDLE;
	    }
	} else {
	    if(mCommunicationStream->available() >= 1){
		packet.Field.Sum = mCommunicationStream->read();
		if(checkPacketSum(packet) == true){
		    processCommand(packet);
		} else {
		    // data that received might be broke -> flush
		    mCommunicationStream->flush();
		}
		serial_receive_state = SRECV_IDLE;
	    }
	}
	
    default:
	break;
    }
}
