/*
 * IR.h
 *
 *  Created on: 22.07.2017
 *      Author: xasin
 */

#ifndef LOCALCODE_IRCOMS_IR_H_
#define LOCALCODE_IRCOMS_IR_H_

#define F_IR 40000
#define F_FRAME 2000

#define IR_TICKS F_CPU/F_IR
#define FRAME_TICKS F_CPU/F_FRAME

#define START_FRAMES 	8
#define START_BITS 		0b1110111

#define DATA_BITS 		12

#define CHECKSUM_FRAMES 4

namespace IR {

enum IRStates {
	IDLE,
	START,
	DATA,
	CHECKSUM
};

struct ShotPacket {
	uint8_t playerID;
	uint8_t shotID:4;
	uint8_t checksum:4;
};

void init(void (*msCallback)());

}

#endif /* LOCALCODE_IRCOMS_IR_H_ */
