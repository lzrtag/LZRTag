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
	Endpoint * const 	nextEndpoint;

	uint8_t const 	targetCommand;
	uint8_t	const	dataLength;
	uint8_t * const targetData;
	void (* const 	endCallback)();

	static Endpoint * getHeadEndpoint();

	Endpoint(const uint8_t cmd, uint8_t * const dataDir, const uint8_t dataLen, void (* const callback)());

	void callCallback();
};

} /* namespace ESPComs */

#endif /* LOCALCODE_ESPCOMS_ENDPOINT_H_ */
