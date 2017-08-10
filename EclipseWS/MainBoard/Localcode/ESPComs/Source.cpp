/*
 * Source.cpp
 *
 *  Created on: 29.07.2017
 *      Author: xasin
 */

#include "Source.h"
#include "ESPUART.h"

namespace ESPComs {

Source * Source::headSource = 0;

Source::Source(const uint8_t cmd, void * const dataDir, const uint8_t dataLen)
	: targetCommand(cmd), targetData(dataDir), dataLength(dataLen), nextSource(headSource) {

	headSource = this;
}

Source * Source::getHeadSource() {
	return Source::headSource;
}

void Source::fire() {
	if(!tryToStart(this))
		this->ignited = true;
}

} /* namespace ESPComs */
