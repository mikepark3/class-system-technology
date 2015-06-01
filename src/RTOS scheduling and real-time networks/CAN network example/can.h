/*
 * can.h
 *
 *  Created on: May 9, 2015
 *      Author: Avees Lab.
 */

#ifndef CAN_H_
#define CAN_H_

#ifdef __cplusplus
	extern "C" {
#endif

		void CAN_initialize(byte Baudrate);  // to initialize CAN
		void CAN_write(unsigned long id, byte len, unsigned char *buf);  // to write CAN Message
		void CAN_read(unsigned char *len, unsigned char *buf); // to read CAN Message
		void CAN_id(void); // get the ID of CAN Message


#ifdef __cplusplus
	}
#endif

#endif /* CAN_H_ */
