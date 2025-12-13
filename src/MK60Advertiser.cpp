//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MK60Advertiser.h"

static const uint8_t  MK60_Telegram_Connect[] = { 0x6D, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x92};
static const uint8_t  MK60_Telegram_Base[] = { 0x61, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x9E };

MK60Advertiser::MK60Advertiser(int instanceNum) {
    setInstanceNumber(instanceNum);
}

void MK60Advertiser::initChannelData(int instanceNum) {

    if (sizeof(channelData) != sizeof(MK60_Telegram_Base) ) {
        Serial.println("WARNING: invalid sizeof(channelData) for MK 6.0");
        return;
    }

    memcpy(channelData, MK60_Telegram_Base, sizeof(MK60_Telegram_Base));

    switch(instanceNum) {

        case 0:
            channelData[0] = 0x61;
            channelData[9] = 0x9E;
            break;

        case 1:
            channelData[0] = 0x62;
            channelData[9] = 0x9D;
            break;

        case 2:
            channelData[0] = 0x63;
            channelData[9] = 0x9C;
            break;

        default:
            Serial.println("WARNING: invalid instance number for MK 6.0");
            break;
    }
    
}

void MK60Advertiser::setInstanceNumber(int instanceNum) {
    initChannelData(instanceNum);
}

void MK60Advertiser::resetChannels(int instance) {
    initChannelData(instance);
}

void MK60Advertiser::setChannelValue(int instance, int channel, double normalizedValue) {

    if ( (instance < 0) || (instance > 2) ) {
        Serial.println("WARNING: invalid instance for MK 6.0");
        return;
    }

    if ( (channel < 0) || (channel > 5) ) {
        Serial.println("WARNING: invalid channel for MK 6.0");
        return;
    }

    int iValue = ((double) (normalizedValue * 127.0 + 128.0) );

    uint8_t oldValue = channelData[3+channel];
    channelData[3+channel] = (uint8_t) iValue;

    if (channelData[3+channel] != oldValue)
        setDataUpdated();
}

int MK60Advertiser::getConnectPayload(uint8_t *outPayload, int outMaxLen) {

    if (outMaxLen < sizeof (MK60_Telegram_Connect))
        return 0;

    memcpy(outPayload, MK60_Telegram_Connect, sizeof(MK60_Telegram_Connect));
    return sizeof (MK60_Telegram_Connect);
}

int MK60Advertiser::getUpdatePayload(uint8_t *outPayload, int outMaxLen) {

    if (outMaxLen < sizeof (MK60_Telegram_Base))
        return 0;

    memcpy(outPayload, channelData, sizeof (MK60_Telegram_Base));
    return sizeof (MK60_Telegram_Base);
}
