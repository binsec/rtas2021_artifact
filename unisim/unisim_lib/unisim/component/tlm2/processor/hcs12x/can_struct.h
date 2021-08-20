
#ifndef __CAN_STRUCT_H__
#define __CAN_STRUCT_H__

#if defined(WIN32) || defined(WIN64)
  typedef unsigned char uint8_t;

#else
	#include <inttypes.h>
#endif


#ifdef __cplusplus
extern "C" {
#endif

// ******* CAN Data structures **************

#define CAN_MSG_SIZE			16
#define CAN_EXG_SIZE			13
#define CAN_ID_INDEX			0
#define CAN_ID_SIZE				4
#define CAN_DATA_INDEX			4
#define CAN_DATA_SIZE			8
#define CAN_LENGTH_INDEX		12
#define CAN_DATA_LENGTH_SIZE	1
#define CAN_PRIORITY_INDEX		13
#define CAN_PRIORITY_SIZE		1
#define CAN_TIMESTAMP_INDEX		14
#define CAN_TIMESTAMP_SIZE		2

typedef enum {CAN_MSG_STD, CAN_MSG_EXT} CanMsgType;
#define INVALID_CAN_ID 0xFFFFFFFF // identify an invalid message

#define CAN_BUS_WIDTH 32

/*
The CAN_DATATYPE structure
*/
typedef struct CAN_MESSAGE {

    /* Is Extended frame: extended ID = 29-bits and standard ID = 11-bits */
    uint8_t Extended;

    /* RTR: Remote Transmission Request (Request for transmission)
     *   0: Data frame
     *   1: Remote frame (forces 0 bytes of data)
     *
     *   This flag reflects the status of the Remote Transmission Request bit in the CAN frame.
     *   In the case of a receive buffer, it indicates the status of the received frame and supports the transmission of an answering frame in software.
     *   In the case of a transmit buffer, this flag defines the setting of the RTR bit to be sent.
     */
    uint8_t Remote;

    /* Error ? */
    uint8_t Error;

    /* CAN ID */
    uint8_t ID[CAN_ID_SIZE];

    /* Data field */
    uint8_t Data[CAN_DATA_SIZE];

    /* Length */
    uint8_t Length; // 4 bits of DLC (Data Length Code), which may only take on the values 0 - 8

    /* Priority */
    uint8_t Priority;

    /* MSG Time Stamp */
    uint8_t Timestamp[CAN_TIMESTAMP_SIZE];
	
	CAN_MESSAGE() : Extended(), Remote(), Error(), ID(), Data(), Length(), Priority(), Timestamp() {}

}  CAN_MESSAGE;

typedef CAN_MESSAGE CAN_DATATYPE;

/**
 * Hold 1xN array of CAN_DATATYPE
 *
 * @param nmsgs: Indicates how many messages are in the array.
 * @param canMsg: The array of messages.
 */
typedef struct {
    unsigned int nmsgs;
    CAN_DATATYPE* canMsg;
} CAN_DATATYPE_ARRAY;

/**
 * Hold CAN ID information.
 *
 * @param nIDs: Indicates how many messages IDs are in the array.
 * @param IDs: The array of messages IDs.
 */
typedef struct {
    unsigned int nIDs;
    uint8_t** IDs;
} CAN_ID_ARRAY;

#ifdef __cplusplus
}
#endif

#endif /*__CAN_STRUCT_H__*/

