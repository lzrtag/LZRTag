/*
 * BasePattern.cpp
 *
 *  Created on: 5 Sep 2019
 *      Author: xasin
 */

#include "BasePattern.h"

namespace LZR {
namespace FX {

BasePattern::BasePattern() : enabled(true) {
}

BasePattern::~BasePattern() {
}

void BasePattern::apply_color_at(Xasin::NeoController::Color &tgt, float pos) {
}
void BasePattern::tick() {
}

} /* namespace FX */
} /* namespace LZR */
