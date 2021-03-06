#ifndef __MemoryMap_h__
#define __MemoryMap_h__

#ifndef PC_DEBUG
#include "WProgram.h"
#else
#include "testlibs.h"
#endif

#include "CommunicationStream.h"

//
//	MemoryMap Protocol Lib for Arduino
//	Coded by Yasuhiro ISHII 17 Nov,2011
//

#ifndef NULL
#define NULL	((void*)0)
#endif

#define	MEMMAP_HEADER1			0xfa
#define MEMMAP_HEADER2			0xaf
#define MEMMAP_CMD_READ			0x01
#define MEMMAP_CMD_RETURN		0x02
#define MEMMAP_CMD_WRITE		0x03
#define MEMMAP_CMD_ACK			0x04

#define OPERATION_NOP	0x00
#define OPERATION_READ	0x01
#define OPERATION_WRITE 0x02
#define OPERATION_JOB	0x04

#define Job FUNCPTR
#define Var unsigned char

#define TIMEOUT_TIME		3000
#define DETECT_TIMEOUT(t)	(((millis() - t) > TIMEOUT_TIME) ? true : false)

typedef enum ADDRESS_MAPPING {
    MEMMAP_ADR_MMPK_ModelNumber_L = 0x00,
    MEMMAP_ADR_MMPK_ModelNumber_H,
    MEMMAP_ADR_MMPK_FirmwareVerion,
    MEMMAP_ADR_MMPK_Reserve_0,
    MEMMAP_ADR_MMPK_RobotID,
    MEMMAP_ADR_MMPK_Reserve_1,
    MEMMAP_ADR_MMPK_BPS,
    MEMMAP_ADR_MMPK_ReturnDelay,
    MEMMAP_ADR_MMPK_Param_0,
    MEMMAP_ADR_MMPK_Param_1,
    MEMMAP_ADR_MMPK_Command,
    MEMMAP_ADR_MMPK_Reserve_2,
    MEMMAP_ADR_MMPK_GPIO_A_DDR,
    MEMMAP_ADR_MMPK_GPIO_A_OUT,
    MEMMAP_ADR_MMPK_GPIO_A_IN,
    MEMMAP_ADR_MMPK_GPIO_B_DDR,
    MEMMAP_ADR_MMPK_GPIO_B_OUT, // 0x10
    MEMMAP_ADR_MMPK_GPIO_B_IN,
    MEMMAP_ADR_MKPB_BEEP_SET,
    MEMMAP_ADR_MMPK_ADC_0,
    MEMMAP_ADR_MMPK_ADC_1,
    MEMMAP_ADR_MMPK_ADC_2,
    MEMMAP_ADR_MMPK_ADC_3,
    MEMMAP_ADR_MMPK_ADC_4,
    MEMMAP_ADR_MMPK_ADC_5,
    MEMMAP_ADR_MMPK_ADC_6,
    MEMMAP_ADR_MMPK_ADC_7,
    MEMMAP_ADR_MMPK_Motor_0,	// 0x1B
    MEMMAP_ADR_MMPK_Motor_1,	// 0x1C
} AddressMapping;

typedef struct S_Command {
    unsigned char RWOP;
    void* ptr;
} Command;

typedef void (*FUNCPTR)(unsigned char ,unsigned char ,unsigned char* );

typedef enum E_JOBID {
    JOBID_NOP,
    JOBID_MOTOR
} JobID;

#define MEMMAP_MAX_DATA_LENGTH	8
typedef union U_MEMMAP_Message {
    struct S_MEMMAP_Message {
	unsigned char		Header[2];
	unsigned char		Cmd;
	unsigned char		Address;
	unsigned char		Length;
	unsigned char		data[MEMMAP_MAX_DATA_LENGTH];
	unsigned char		Sum;
    } Field;
    unsigned char Raw[14];
} MEMMAP_Message;

typedef enum SERIAL_RECEIVE_STATE {
    SRECV_IDLE,
    SRECV_RECEIVING_HEADER,
    SRECV_RECEIVING,
    SRECV_ERROR
} SerialReceiveState;

#define	MAX_NUMBER_OF_ADDRESSES		0x30

class MemoryMap {

private:
    MEMMAP_Message packet;
    MEMMAP_Message packetReply;
    Command MemoryMapTable[MAX_NUMBER_OF_ADDRESSES];
    CommunicationStream* mCommunicationStream;

    int checkPacketHeader(MEMMAP_Message& msg);
    int isReadOperation(MEMMAP_Message& msg);
    int checkPacketSum(MEMMAP_Message& msg);
    int sendMessage(MEMMAP_Message& msg);
    int processCommand(MEMMAP_Message& msg);
    int processWriteCommand(MEMMAP_Message& msg);
    int processReadCommand(MEMMAP_Message& msg,MEMMAP_Message& msg_reply);
    int writeMemory(unsigned char address,unsigned char value);
    int readMemory(unsigned char address,unsigned char* value);

public:
    void poll(void);
    int registerMapAddressJob(unsigned char address,unsigned char RWOP,Job ptr);
    int registerMapAddressVar(unsigned char address,unsigned char RWOP,unsigned char* ptr);
    int registerMapAddressBlock(Command* );
    int removeMapAddress(unsigned char address);
    int setStreamInterface(CommunicationStream* );
    MemoryMap();

};

#endif /* __MemoryMap_h__ */
