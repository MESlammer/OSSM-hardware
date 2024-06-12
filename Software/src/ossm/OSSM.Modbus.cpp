#include "OSSM.h"

#include "extensions/u8g2Extensions.h"

#include "services/modbus.h"
#include "services/tasks.h"

//#include "Logging.h"

void OSSM::drawServoDataTask(void *pvParameters){

    OSSM *ossm = (OSSM *)pvParameters;

    String strokeString;

    int servoTorque;
    bool servoState;
    uint32_t Token = 1111;

    unsigned long displayLastUpdated = 0;
    bool shouldUpdateDisplay = false;



    //addr 1 func 3
    //torque is in address 0x0843 (hex) 2115 (dec)


    auto isInCorrectState = [](OSSM *ossm) {
        // Add any states that you want to support here.
        return ossm->sm->is("servodata"_s) || ossm->sm->is("servodata.idle"_s);
    };

    while (isInCorrectState(ossm)) {
        requestModbus(DataTokens::TORQUE);
        servoTorque = getServoTorque();
        shouldUpdateDisplay = false;

        shouldUpdateDisplay =
            shouldUpdateDisplay || millis() - displayLastUpdated > 200;

        if (!shouldUpdateDisplay) {
            vTaskDelay(100);
            continue;
        }

        displayLastUpdated = millis();

        
        displayMutex.lock();
        ossm->display.clearBuffer();


        ossm->display.setFont(Config::Font::small);
        strokeString = "Servo status: " + String(servoState);
        ossm->display.drawUTF8(0, 12, strokeString.c_str());
        strokeString = "Servo torque:" + String(servoTorque);     //UserConfig::language.ModbusMotorStatusLine.c_str()
        ossm->display.drawUTF8(0, 22, strokeString.c_str());     //UserConfig::language.ModbusMotorTorqueLine.c_str()
        ossm->display.drawUTF8(0, 62, UserConfig::language.Skip.c_str());
        ossm->display.sendBuffer();
        displayMutex.unlock();

        ESP_LOGD("Servo Data", "Status:%d", servoState);
        ESP_LOGD("Servo Data", "Torque:%d", servoTorque);

        vTaskDelay(100);
    };


    servo.end();

    vTaskDelete(nullptr);

}


void OSSM::drawServoData(){
    reInitModbus();

    int stackSize = 5 * configMINIMAL_STACK_SIZE;
    xTaskCreate(drawServoDataTask, "drawServoDataTask", stackSize, this, 1,
        &drawServoDataTaskH);

}

