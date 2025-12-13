//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MK40Advertiser.h"

static const uint8_t MK40_Telegram_Connect[] = { 0xAD, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x4F, 0x52 };
static const uint8_t MK40_Telegram_Base[] = { 0x7D, 0x7B, 0xA7, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x82 };

MK40Advertiser::MK40Advertiser() {

    if (sizeof(channelData) != sizeof(MK40_Telegram_Base) ) {
        Serial.println("WARNING: invalid sizeof(channelData) for MK 4.0");
        return;
    }

    memcpy(channelData, MK40_Telegram_Base, sizeof(MK40_Telegram_Base));
}

void MK40Advertiser::setChannelValue(int instance, int channel, double normalizedValue) {
   
    uint8_t bValue = 0x08; // zero value by default

    double abs_value = abs(normalizedValue);
    if (abs_value > 0.875)
        bValue = 7;
    else if (abs_value > 0.750)
        bValue = 6;
    else if (abs_value > 0.625)
        bValue = 5;
    else if (abs_value > 0.5)
        bValue = 4;
    else if (abs_value > 0.375)
        bValue = 3;
    else if (abs_value > 0.250)
        bValue = 2;
    else if (abs_value > 0.125)
        bValue = 1;

    if (normalizedValue < -0.125)
        bValue += 8;

    setChannelRawValue(instance, channel, bValue);
}

void MK40Advertiser::setChannelRawValue(int instance, int channel, uint8_t rawValue) {

    if ( (instance < 0) || (instance > 2) ) {
        Serial.println("WARNING: invalid instance for MK 4.0");
        return;
    }

    if ( (channel < 0) || (channel > 3) ) {
        Serial.println("WARNING: invalid channel for MK 4.0");
        return;
    }    

    int offset = 3 + 2 * instance;
    uint8_t oldValue0 = channelData[offset+0];
    uint8_t oldValue1 = channelData[offset+1];

    switch (channel) {
        case 0:
            channelData[offset+0] = (rawValue << 4) + (channelData[offset+0] & 0x0F);
            break;
        case 1:
            channelData[offset+0] = (channelData[offset+0] & 0xF0) + rawValue;
            break;
        case 2:
            channelData[offset+1] = (rawValue << 4) + (channelData[offset+1] & 0x0F);
            break;
        case 3:
            channelData[offset+1] = (channelData[offset+1] & 0xF0) + rawValue;
            break;
    }

    if ( (oldValue0 != channelData[offset+0]) || (oldValue1 != channelData[offset+1]) )
        setDataUpdated();

}

void MK40Advertiser::resetChannels(int instance) {
    for (int i=0; i<getChannelCount(); i++) {
        setChannelRawValue(instance, i, 0x88);
    }
}

int MK40Advertiser::getConnectPayload(uint8_t *outPayload, int outMaxLen) {

    if (outMaxLen < sizeof(MK40_Telegram_Connect))
        return 0;

    memcpy(outPayload, MK40_Telegram_Connect, sizeof(MK40_Telegram_Connect));
    return sizeof(MK40_Telegram_Connect);
}

int MK40Advertiser::getUpdatePayload(uint8_t *outPayload, int outMaxLen) {

    if (outMaxLen < sizeof(MK40_Telegram_Base))
        return 0;

    memcpy(outPayload, channelData, sizeof(MK40_Telegram_Base));
    return sizeof(MK40_Telegram_Base);
}
