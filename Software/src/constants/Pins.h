#ifndef OSSM_SOFTWARE_PINS_CPP
#define OSSM_SOFTWARE_PINS_CPP

/**
 * Pin definitions for the OSSM
 *
 * These are all the hardware pins used by the OSSM.
 * These include pin definitions for Drivers, Buttons and Remotes
 *
 * To reference a pin, import this file and use the Pins namespace as follows:
 *
 * ```cpp
 * #include "constants/pins.cpp"
 *
 * Pins::button_in
 * Pins::Driver::motorStepPin
 * Pins::Homing::stopPin
 *
 * ```
 */

//TODO: separate pin definitions for OSSM Reference board and OSSM ICB

namespace Pins {

    namespace Display {
        // This pin is used by u8g2 to reset the display.
        // Use -1 if this is shared with the board reset pin.
        constexpr int oledReset = -1;

        // Pin used by RGB LED
        constexpr int ledPin = 48;
    }

    namespace Driver {
        constexpr int currentSensorPin = 4;

        // Pin that pulses on servo/stepper steps - likely labelled PUL on
        // drivers.
        constexpr int motorStepPin = 12;
        // Pin connected to driver/servo step direction - likely labelled DIR on
        // drivers. N.b. to iHSV57 users - DIP switch #5 can be flipped to
        // invert motor direction entirely
        constexpr int motorDirectionPin = 11;
        // Pin for motor enable - likely labelled ENA on drivers.
        constexpr int motorEnablePin = 13;

        // define the IO pin the emergency stop switch is connected to
        constexpr int stopPin = 14;
        // define the IO pin where the limit(homingStart) switch(es) are
        // connected to (switches in series in normally open setup) Switches
        // wired from IO pin to ground.
        constexpr int limitSwitchPin = -1;
    }

    namespace Wifi {
        // Pin for Wi-Fi reset button (optional)
        constexpr int resetPin = -1;
        // Pin for the toggle for Wi-Fi control (Can be targeted alone if no
        // hardware toggle is required)
        constexpr int controlTogglePin = 45;
    }

    /** These are configured for the OSSM Remote - which has a screen, a
     * potentiometer and an encoder which clicks*/
    namespace Remote {

        constexpr int speedPotPin = 7;

        // This switch occurs when you press the right button in.
        // With the current state of the code this will send out a "ButtonPress"
        // event automatically.
        constexpr int encoderSwitch = 17;

        // The rotary encoder requires at least two pins to function.
        constexpr int encoderA = 15;
        constexpr int encoderB = 16;
        constexpr int encoderPower =
            -1; /* Put -1 of Rotary encoder Vcc is connected directly to 3,3V;
                   else you can use declared output pin for powering rotary
                   encoder */

        constexpr int displayData = 5;
        constexpr int displayClock = 6;
        constexpr int encoderStepsPerNotch = 4;
    }
}
//first version of manufactured remote board has a error in schematic, displayData swapped with speedPotPin
#endif  // OSSM_SOFTWARE_PINS_CPP
