#include "OSSM.h"

#include "Events.h"
#include "constants/UserConfig.h"
#include "utils/analog.h"
#include "services/modbus.h"

namespace sml = boost::sml;
using namespace sml;

/** OSSM Homing methods
 *
 * This is a collection of methods that are associated with the homing state on
 * the OSSM. technically anything can go here, but please try to keep it
 * organized.
 */
void OSSM::clearHoming() {
    ESP_LOGD("Homing", "Homing started");
    isForward = true;

    // Set acceleration and deceleration in steps/s^2
    stepper->setAcceleration(1000_mm);
    // Set speed in steps/s
    stepper->setSpeedInHz(25_mm);

    // Clear the stored values.
    this->measuredStrokeSteps = 0;

    // Recalibrate the current sensor offset.
    this->currentSensorOffset = (getAnalogAveragePercent(
        SampleOnPin{Pins::Driver::currentSensorPin, 1000}));
};

void OSSM::startHomingTask(void *pvParameters) {
    TickType_t xTaskStartTime = xTaskGetTickCount();

    // parse parameters to get OSSM reference
    OSSM *ossm = (OSSM *)pvParameters;

    // Stroke Engine and Simple Penetration treat this differently.
    ossm->stepper->enableOutputs();
    ossm->stepper->setDirectionPin(Pins::Driver::motorDirectionPin, false);
    int16_t sign = ossm->sm->is("homing.backward"_s) ? 1 : -1;

    int32_t targetPositionInSteps =
        round(sign * Config::Driver::maxStrokeSteps);

    ESP_LOGD("Homing", "Target position in steps: %d", targetPositionInSteps);
    ossm->stepper->moveTo(targetPositionInSteps, false);

    auto isInCorrectState = [](OSSM *ossm) {
        // Add any states that you want to support here.
        return ossm->sm->is("homing"_s) || ossm->sm->is("homing.forward"_s) ||
               ossm->sm->is("homing.backward"_s);
    };

    // run loop for 15second or until loop exits
    while (isInCorrectState(ossm)) {
        TickType_t xCurrentTickCount = xTaskGetTickCount();
        // Calculate the time in ticks that the task has been running.
        TickType_t xTicksPassed = xCurrentTickCount - xTaskStartTime;

        // If you need the time in milliseconds, convert ticks to milliseconds.
        // 'portTICK_PERIOD_MS' is the number of milliseconds per tick.
        uint32_t msPassed = xTicksPassed * portTICK_PERIOD_MS;

        if (msPassed > 30000) {
            ESP_LOGE("Homing", "Homing took too long. Check power and restart");
            ossm->errorMessage = UserConfig::language.HomingTookTooLong;
            ossm->sm->process_event(ErrorEv{});
            break;
        }

        // measure the current analog value.
        //float current = getAnalogAveragePercent(
        //                    SampleOnPin{Pins::Driver::currentSensorPin, 200}) -
        //                ossm->currentSensorOffset;
        //
        //ESP_LOGV("Homing", "Current: %f", current);

        

        //bool isCurrentOverLimit =
        //    current > Config::Driver::sensorlessCurrentLimit;

        //if (!isCurrentOverLimit) {
        //    vTaskDelay(1);
        //    continue;
        //}

        //ESP_LOGD("Homing", "Current over limit: %f", current);


        //shouldRequestModbusData =
        //    shouldRequestModbusData || millis() - modbusDataLastUpdated > 200;
        //requestModbus(DataTokens::TORQUE);
        //int torque = getServoTorque();

        int torque = requestModbusTorqueSync();

        ESP_LOGV("Homing", "Torque: %d", torque);

        bool isTorqueOverLimit =
            torque > Config::Driver::sensorlessTorqueLimit;

        if (!isTorqueOverLimit) {
            vTaskDelay(5);
            continue;
        }
        ESP_LOGD("Homing", "Torque over limit: %d", torque);

        torque = 0;
        ossm->stepper->stopMove();

        //vTaskDelay(500);
        //servo.clearQueue();

        // step away from the hard stop, with your hands in the air!
        int32_t currentPosition = ossm->stepper->getCurrentPosition();
        ossm->stepper->moveTo(currentPosition - sign * 10_mm, true);

        requestModbus(DataTokens::TORQUE);
        torque = getServoTorque();
        vTaskDelay(100);

        // measure and save the current position
        ossm->measuredStrokeSteps =
            min(float(abs(ossm->stepper->getCurrentPosition())),
                Config::Driver::maxStrokeSteps);

        ossm->stepper->setCurrentPosition(0);
        ossm->stepper->forceStopAndNewPosition(0);

        ossm->sm->process_event(Done{});
        break;
    };

    //servo.end();


    vTaskDelete(nullptr);
}

void OSSM::startHoming() {
    reInitModbus();


    int stackSize = 10 * configMINIMAL_STACK_SIZE;
    xTaskCreatePinnedToCore(startHomingTask, "startHomingTask", stackSize, this,
                            configMAX_PRIORITIES - 1, &runHomingTaskH,
                            operationTaskCore);
}

auto OSSM::isStrokeTooShort() -> bool {
    if (measuredStrokeSteps > Config::Driver::minStrokeLengthMm) {
        return false;
    }
    this->errorMessage = UserConfig::language.StrokeTooShort;
    return true;
}
