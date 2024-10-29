#include "Arduino.h"
#include "OneButton.h"
#include "WiFi.h"
#include "ossm/Events.h"
#include "ossm/OSSM.h"
#include "services/board.h"
#include "services/display.h"
#include "services/encoder.h"
#include "services/stepper.h"
#include "services/modbus.h"

#include <FastLED.h>

#include "ModbusClientRTU.h"

#include "Logging.h"

/*
 *  ██████╗ ███████╗███████╗███╗   ███╗
 * ██╔═══██╗██╔════╝██╔════╝████╗ ████║
 * ██║   ██║███████╗███████╗██╔████╔██║
 * ██║   ██║╚════██║╚════██║██║╚██╔╝██║
 * ╚██████╔╝███████║███████║██║ ╚═╝ ██║
 *  ╚═════╝ ╚══════╝╚══════╝╚═╝     ╚═╝
 *
 * Welcome to the open source sex machine!
 * This is a product of Kinky Makers and is licensed under the MIT license.
 *
 * Research and Desire is a financial sponsor of this project.
 *
 * But our biggest sponsor is you! If you want to support this project, please
 * contribute, fork, branch and share!
 */

OSSM* ossm;

OneButton button(Pins::Remote::encoderSwitch, false);

CRGB leds [1];

void setup() {
    /** Board setup */
    initBoard();

    /** Service setup */
    // Encoder
    initEncoder();

    initModbus();
    // Display
    display.setI2CAddress(0x3C*2);
    display.setBusClock(400000);
    display.begin();

    FastLED.addLeds<NEOPIXEL, Pins::Display::ledPin>(leds, 1);

    leds[0] = CRGB::Red;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Green;
    FastLED.show();
    delay(500);
    leds[0] = CRGB::Blue;
    FastLED.show();
    delay(500);
    // Now turn the LED off, then pause
    leds[0] = CRGB::Black;
    FastLED.show();
    delay(500);

    ossm = new OSSM(display, encoder, stepper);
    // link functions to be called on events.
    button.attachClick([]() { ossm->sm->process_event(ButtonPress{}); });
    button.attachDoubleClick([]() { ossm->sm->process_event(DoublePress{}); });
    button.attachLongPressStart([]() { ossm->sm->process_event(LongPress{}); });


};

void loop() {
    button.tick();
    ossm->wm.process();
    //requestModbus();
};