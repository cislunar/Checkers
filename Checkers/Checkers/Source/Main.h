
// Common includes
#include <assert.h>

#ifndef _CONTROLS_H_
#define _CONTROLS_H_

#define SPRITE_DIAM 10
#define SPRITE_RADIUS (SPRITE_DIAM/2)

// Tweak menu
#define TWEAK_MENU _DEBUG || 1	// Useful in release for tuning water behavior
#if TWEAK_MENU
#include "AntTweakBar.h"
TwBar *TwkBar();
#endif // TWEAK_MENU

#endif