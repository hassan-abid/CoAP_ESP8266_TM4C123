
#ifndef __CAN_MSG_DEFS_H_
#define __CAN_MSG_DEFS_H_


typedef enum{
	
	CAN_MSG_SENSOR_IR = 1,
	CAN_MSG_SENSOR_PING = 2,
	CAN_MSG_SENSOR_PUSHBUTTON = 3,
	CAN_MSG_HARD_LEFT,
	CAN_MSG_HARD_RIGHT,
	CAN_MSG_STRAIGHT,
	CAN_MSG_STOP,
	CAN_MSG_SOFT_LEFT,
	CAN_MSG_SOFT_RIGHT,
	CAN_MSG_SET_ANGLE,
	CAN_MSG_SET_SPEED,
	CAN_MSG_SET_DIRECTION,
	
}CAN_Msg_Sensor_t;

#pragma anon_unions
typedef struct{
	
		uint16_t sensorNum  		:2;
		uint16_t sensorType			:14;

}CAN_Msg_Id_t;

typedef struct {
	
	union{
		
		CAN_Msg_Id_t	id;
		uint16_t 			u16Id;
	};
	
	union {
		uint8_t u8Msg[4];
		uint32_t u32Msg;
		int32_t  i32Msg;
	};
	
}CAN_Message_t;




#endif