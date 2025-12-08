//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MK60Advertiser.h"

#include <Arduino.h>


uint8_t  MK60_Telegram_Connect[] = { 0x6D, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x92};

uint8_t  MK60_Telegram_Base_Device_1[] = { 0x61, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x9E };
uint8_t  MK60_Telegram_Base_Device_2[] = { 0x62, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x9D };
uint8_t  MK60_Telegram_Base_Device_3[] = { 0x63, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x9C };

MK60Advertiser::MK60Advertiser(int instanceNum) {

    setInstanceNumber(instanceNum);
}

void MK60Advertiser::setInstanceNumber(int instanceNum) {

    connectDataSize = sizeof(MK60_Telegram_Connect);
    connectData = new uint8_t[connectDataSize];
    memcpy(connectData, MK60_Telegram_Connect, connectDataSize);

    if (instanceNum == 0) {
        channelDataSize = sizeof(MK60_Telegram_Base_Device_1);
        channelData = new uint8_t[channelDataSize];
        memcpy(channelData, MK60_Telegram_Base_Device_1, channelDataSize);

        return;
    }

    if (instanceNum == 1) {
        channelDataSize = sizeof(MK60_Telegram_Base_Device_2);
        channelData = new uint8_t[channelDataSize];
        memcpy(channelData, MK60_Telegram_Base_Device_2, channelDataSize);

        return;
    }

    if (instanceNum == 2) {
        channelDataSize = sizeof(MK60_Telegram_Base_Device_3);
        channelData = new uint8_t[channelDataSize];
        memcpy(channelData, MK60_Telegram_Base_Device_3, channelDataSize);

        return;
    }

    Serial.println("WARNING: invalid instance number for MK 6.0");
    channelData = new uint8_t;
    channelDataSize = 1;
}

void MK60Advertiser::setChannelValue(int instance, int channel, double normalized_value) {

    if ( (instance < 0) || (instance > 2) ) {
        Serial.println("WARNING: invalid instance for MK 6.0");
        return;
    }

    if ( (channel < 0) || (channel > 5) ) {
        Serial.println("WARNING: invalid channel for MK 6.0");
        return;
    }

    int iValue = ((double) (normalized_value * 127.0 + 128.0) );

    channelData[3+channel] = (uint8_t) iValue;
}

int MK60Advertiser::getConnectPayload(uint8_t *out_payload, int out_maxlen) {
    if (out_maxlen < connectDataSize)
        return 0;

    memcpy(out_payload, connectData, connectDataSize);
    return connectDataSize;
}

int MK60Advertiser::getUpdatePayload(uint8_t *out_payload, int out_maxlen) {
    if (out_maxlen < channelDataSize)
        return 0;

    memcpy(out_payload, channelData, channelDataSize);
    return channelDataSize;
}
