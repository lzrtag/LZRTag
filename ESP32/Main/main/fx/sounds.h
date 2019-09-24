/*
 * sounds.h
 *
 *  Created on: 30 Mar 2019
 *      Author: xasin
 */

#ifndef MAIN_FX_SOUNDS_H_
#define MAIN_FX_SOUNDS_H_

#include <string>

namespace LZR {
namespace Sounds {

extern volatile uint32_t note_until;
extern volatile float 	 volume;
extern volatile uint32_t suggested_hue;

void play_audio(std::string aName);
void init();

}
}

#endif /* MAIN_FX_SOUNDS_H_ */
