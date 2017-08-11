/*
 * Source.h
 *
 *  Created on: 29.07.2017
 *      Author: xasin
 */

#ifndef LOCALCODE_ESPCOMS_SOURCE_H_
#define LOCALCODE_ESPCOMS_SOURCE_H_

#include <avr/io.h>

namespace ESPComs {

class Source {
private:
	static Source * headSource;
public:
	Source * const nextSource;

	uint8_t const 	targetCommand;
	uint8_t	const	dataLength;
	void * const targetData;

	bool ignited = false;

	static Source * getHeadSource();

	Source(const uint8_t cmd, void * const dataDir, const uint8_t dataLen);

	void fire();
};

} /* namespace ESPComs */

#endif /* LOCALCODE_ESPCOMS_SOURCE_H_ */
