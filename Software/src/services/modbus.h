#ifndef OSSM_SOFTWARE_MODBUS_H
#define OSSM_SOFTWARE_MODBUS_H

#include <Arduino.h>

#include "ModbusClientRTU.h"

//#include "Logging.h"

static ModbusClientRTU servo;

static int servoCode;
static int servoArmFirmware;

static uint16_t servoRuntime;
static int servoTorque;

enum DataTokens { INTRODUCTION, TORQUE, RUNTIME };

static void handleData(ModbusMessage response, uint32_t token) {
    uint16_t value;
    uint32_t valueLong;
    switch (token) {
        case DataTokens::INTRODUCTION:
            response.get(3, value);
            servoCode = value;
            response.get(4, value);
            servoArmFirmware = value;
            ESP_LOGD("Modbus", "Servo code: %d,Servo firmware: %d", servoCode,
                     servoArmFirmware);
            break;
        case DataTokens::TORQUE:
            response.get(3, value);
            if (value > 32676) {
                servoTorque = abs(value - 65536);
            } else {
                servoTorque = value;
            }
            ESP_LOGD("Modbus", "Servo Torque: %d", servoTorque);
            break;

        case DataTokens::RUNTIME:
            response.get(3, valueLong);
            if (value > 2147482647) {
                servoRuntime = valueLong - 4294967296;
            } else {
                servoRuntime = valueLong;
            }
            ESP_LOGD("Modbus", "Servo runtime: %d", servoRuntime);
            break;
    }

    // ESP_LOGD("Modbus", "Response: serverID=%d, FC=%d, Token=%08X,
    // length=%d:\n",
    //          response.getServerID(), response.getFunctionCode(), token,
    //          response.size());
    // for (auto &byte : response) {
    //     Serial.printf("%02X ", byte);
    // }
    // Serial.println();
}

// Define an onError handler function to receive error responses
// Arguments are the error code returned and a user-supplied token to identify
// the causing request
static void handleError(Error error, uint32_t token) {
    // ModbusError wraps the error code and provides a readable error message
    // for it
    ModbusError me(error);
    ESP_LOGD("Modbus", "Error response: %02X - %s\n", (int)me,
             (const char *)me);
}

static void requestModbus(uint32_t token = DataTokens::INTRODUCTION) {
    Error err;
    switch (token) {
        case DataTokens::INTRODUCTION: {
            err = servo.addRequest(token, 1, READ_HOLD_REGISTER, 2150, 2);
            if (err != SUCCESS) {
                ModbusError e(err);
                ESP_LOGD("Servo Data", "Error creating request: %02X - %s\n",
                         (int)e, (const char *)e);
            }
            break;
        }
        case DataTokens::TORQUE: {
            err = servo.addRequest(token, 1, READ_HOLD_REGISTER, 2115, 1);
            if (err != SUCCESS) {
                ModbusError e(err);
                ESP_LOGD("Servo Data", "Error creating request: %02X - %s\n",
                         (int)e, (const char *)e);
            }
            break;
        }
        case DataTokens::RUNTIME: {
            err = servo.addRequest(token, 1, READ_HOLD_REGISTER, 2130, 2);
            if (err != SUCCESS) {
                ModbusError e(err);
                ESP_LOGD("Servo Data", "Error creating request: %02X - %s\n",
                         (int)e, (const char *)e);
            }
            break;
        }
    }
}

static int requestModbusTorqueSync() {
  int result = 0;
    ModbusMessage response =
        servo.syncRequest(DataTokens::TORQUE, 1, READ_HOLD_REGISTER, 2115, 1);
    Error err = response.getError();
    if (err != SUCCESS) {
        ModbusError e(err);
        ESP_LOGD("Servo Data", "Error creating request: %02X - %s\n", (int)e,
                 (const char *)e);
    } else {
        uint16_t value;
        response.get(3, value);
        if (value > 32676) {
            result = abs(value - 65536);
        } else {
            result = value;
        }                
        ESP_LOGD("Modbus", "Servo Torque: %d", result);
        
    }
    return result;
}

static void initModbus() {
    RTUutils::prepareHardwareSerial(Serial2);
    Serial2.begin(57600, SERIAL_8E1, GPIO_NUM_9, GPIO_NUM_10);
    // Serial2.println("serial 2 works");

    // Set up ModbusRTU client.
    // - provide onData handler function
    servo.onDataHandler(&handleData);
    // - provide onError handler function
    servo.onErrorHandler(&handleError);
    // Set message timeout to 2000ms
    servo.setTimeout(200);
    // Start ModbusRTU background task
    servo.begin(Serial2);

    requestModbus(DataTokens::INTRODUCTION);
    requestModbus(DataTokens::TORQUE);
    requestModbus(DataTokens::RUNTIME);

    delay(500);

    // servo.end();
}

static void reInitModbus() {
    servo.onDataHandler(&handleData);
    servo.onErrorHandler(&handleError);
    servo.setTimeout(200);
    servo.begin(Serial2);
}

static int getServoIntroduction() { return 0; }

static int getServoTorque() { return servoTorque; }

static void getServoRuntime() {}

#endif  // OSSM_SOFTWARE_MODBUS_H