/*
 * Endpoint.cpp
 *
 *  Created on: 21.07.2017
 *      Author: xasin
 */

#include "Endpoint.h"

namespace ESPComs {

Endpoint * Endpoint::headEndpoint = 0;
uint8_t    Endpoint::pubBuffer[16] = {0};

Endpoint::Endpoint(const uint8_t cmd, void * const dataDir, const uint8_t dataLen, void (* const callback)())
	: targetCommand(cmd), targetData(dataDir), dataLength(dataLen), endCallback(callback), nextEndpoint(headEndpoint) {

	headEndpoint = this;
}

Endpoint * Endpoint::getHeadEndpoint() {
	return headEndpoint;
}

void Endpoint::callCallback() {
	if(endCallback == 0)
		return;
	endCallback();
}

} /* namespace ESPComs */
