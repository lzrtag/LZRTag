/*
 * Endpoint.h
 *
 *  Created on: 21.07.2017
 *      Author: xasin
 */

#ifndef LOCALCODE_ESPCOMS_ENDPOINT_H_
#define LOCALCODE_ESPCOMS_ENDPOINT_H_

#include <avr/io.h>

namespace ESPComs {

class Endpoint {
private:
	static Endpoint * 	headEndpoint;

public:
	static uint8_t pubBuffer[16];

	Endpoint * const 	nextEndpoint;

	uint8_t const 	targetCommand;
	uint8_t	const	dataLength;
	void  * const targetData;
	void (* const 	endCallback)();

	static Endpoint * getHeadEndpoint();

	Endpoint(const uint8_t cmd, void * const dataDir, const uint8_t dataLen, void (* const callback)());

	void callCallback();
};

} /* namespace ESPComs */

#endif /* LOCALCODE_ESPCOMS_ENDPOINT_H_ */
