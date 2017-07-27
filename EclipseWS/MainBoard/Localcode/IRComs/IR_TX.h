/*
 * IR_TX.h
 *
 *  Created on: 26.07.2017
 *      Author: xasin
 */

#ifndef LOCALCODE_IRCOMS_IR_TX_H_
#define LOCALCODE_IRCOMS_IR_TX_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include "IR.h"

namespace IR {
namespace TX {

void startTX(ShotPacket TXData);
void update();

}
}


#endif /* LOCALCODE_IRCOMS_IR_TX_H_ */
