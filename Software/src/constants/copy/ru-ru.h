#ifndef OSSM_SOFTWARE_RU_RU_H
#define OSSM_SOFTWARE_RU_RU_H

#include "structs/LanguageStruct.h"

// TODO: no fonts for cyrillic language
static const LanguageStruct ruRu = {
    .DeepThroatTrainerSync = "DeepThroat Sync",
    .Error = "Ошибка",
    .GetHelp = "Получить помощь",
    .GetHelpLine1 = "в Discord,",
    .GetHelpLine2 = "на GitHub",
    .Homing = "Обнуление",
    .HomingTookTooLong =
        "Обнуление идёт слишком долго. Проверьте подключение и попробуйте ещё раз ",
    .Idle = "Инициализация",
    .InDevelopment = "Эта функция пока не готова",
    .MeasuringStroke = "Измерение",
    .NoInternalLoop = "Aucun gestionnaire d'affichage implémenté.",
    .Restart = "Перезапуск",
    .Settings = "Параметры",
    .SimplePenetration = "Простая пенетрация",
    .Skip = "Выйти",
    .Speed = "Скорость",
    .SpeedWarning = "Снизьте скорость чтобы запустить машину",
    .StateNotImplemented = "Стейт: %u не готов.",
    .Stroke = "Проникновение",
    .StrokeEngine = "Stroke Engine",
    .StrokeTooShort =
        "Движение слишком короткое, проверьте ремень",
    .Update = "Обновление",
    .UpdateMessage =
        "Идёт обновление. Понадобится не дольше минуты.",
    .WiFi = "Wi-Fi",
    .WiFiSetup = "Настр. Wi-Fi",
    .WiFiSetupLine1 = "Подключитесь к",
    .WiFiSetupLine2 = "'Ossm Setup'",
    .YouShouldNotBeHere = "Вас здесь быть не должно.",
    .StrokeEngineDescriptions = {
        "Acceleration, coasting, deceleration equally split; no sensation.",
        "Speed shifts with sensation; balances faster strokes.",
        "Модификатор меняет ускорение, от постоянного до плавного.",
        "Чередование проникновений на половину и полную длину, модификатор меняет скорость.",
        "Проникновение увеличивается с каждым циклом; модификатор устанавливает количество циклов.",
        "Останавливается между проникновениями; модификатор управляет продолжительностью.",
        "Modifies length, maintains speed; sensation influences direction."
    }
};


#endif  // OSSM_SOFTWARE_RU_RU_H
