/*
 * @file 	OS_Mailbox.h
 * @author	Hassan Abid
 * @date	Feb 22, 2018
 *
 * 
 */

#ifndef INC_OS_MAILBOX_H_
#define INC_OS_MAILBOX_H_

#include "OS_FIFO.h"


#define OS_MailBox_Def(name, dataType) 		OS_FIFO_Def(name, dataType, 1)


#define OS_MailBox_Init(mailbox)					OS_FIFO_Init(mailbox)
#define OS_MailBox_Send(mailbox, data)		OS_FIFO_Put_Blocking(mailbox, data)
#define OS_MailBox_Recv(mailbox, data)		OS_FIFO_Get(mailbox, data)



#endif /* INC_OS_MAILBOX_H_ */
