//
// Mould King High Performance Power Module support
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MKHPAdvertiser.h"

// TxId bytes (payload[1..2]) are filled by MKBLEAdvertiser::connect()/update().
//
// Observed High Performance protocol:
// CONNECT: AA <TxId> 00 00 00 00 55
// UPDATE : 66 <TxId> CH1 CH2 CH3 CH4 99
//
// Confirmed with the High Performance Power Module included in
// Mould King set 15075 (Robot Dog):
// CH1: steering (0xFF left, 0x80 neutral, 0x00 right)
// CH2: drive    (0xFF forward, 0x80 stop, 0x00 reverse)
// CH3/CH4: protocol channels present; physical function not yet identified.

static const uint8_t MKHP_Telegram_Connect[] = {
    0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55
};

static const uint8_t MKHP_Telegram_Base[] = {
    0x66, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x99
};

MKHPAdvertiser::MKHPAdvertiser(int instanceNum) {
    setInstanceNumber(instanceNum);
}

void MKHPAdvertiser::initChannelData() {
    if (sizeof(channelData) != sizeof(MKHP_Telegram_Base)) {
        Serial.println("WARNING: invalid sizeof(channelData) for MK High Performance");
        return;
    }

    memcpy(channelData, MKHP_Telegram_Base, sizeof(MKHP_Telegram_Base));
}

void MKHPAdvertiser::setInstanceNumber(int instanceNum) {
    // No instance-specific telegram byte has been observed for this protocol.
    // Keep the API compatible with MKModule/MKBLEAdvertiser.
    if (instanceNum != 0) {
        Serial.println("WARNING: MK High Performance module instances > MODULE_1 are not verified");
    }
    initChannelData();
}

void MKHPAdvertiser::resetChannels(int instance) {
    (void)instance;
    initChannelData();
    setDataUpdated();
}

void MKHPAdvertiser::setChannelValue(int instance, int channel, float normalizedValue) {
    (void)instance;

    if ((channel < 0) || (channel > 3)) {
        Serial.println("WARNING: invalid channel for MK High Performance");
        return;
    }

    // Keep the common MouldKingino -1.0 .. +1.0 API and map it to the
    // protocol's 8-bit channel representation around neutral 0x80.
    // Clamp out-of-range input to avoid integer wraparound.
    if (normalizedValue > 1.0f) normalizedValue = 1.0f;
    if (normalizedValue < -1.0f) normalizedValue = -1.0f;

    uint8_t rawValue;
    if (normalizedValue >= 1.0f) {
        rawValue = 0xFF;
    } else if (normalizedValue <= -1.0f) {
        rawValue = 0x00;
    } else if (normalizedValue >= 0.0f) {
        rawValue = (uint8_t)(128.0f + normalizedValue * 127.0f);
    } else {
        rawValue = (uint8_t)(128.0f + normalizedValue * 128.0f);
    }

    uint8_t oldValue = channelData[3 + channel];
    channelData[3 + channel] = rawValue;

    if (channelData[3 + channel] != oldValue) {
        setDataUpdated();
    }
}

int MKHPAdvertiser::getConnectPayload(uint8_t *outPayload, int outMaxLen) {
    if (outMaxLen < (int)sizeof(MKHP_Telegram_Connect)) {
        return 0;
    }

    memcpy(outPayload, MKHP_Telegram_Connect, sizeof(MKHP_Telegram_Connect));
    return sizeof(MKHP_Telegram_Connect);
}

int MKHPAdvertiser::getUpdatePayload(uint8_t *outPayload, int outMaxLen) {
    if (outMaxLen < (int)sizeof(MKHP_Telegram_Base)) {
        return 0;
    }

    memcpy(outPayload, channelData, sizeof(MKHP_Telegram_Base));
    return sizeof(MKHP_Telegram_Base);
}
