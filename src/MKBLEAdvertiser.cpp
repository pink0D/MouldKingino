//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#include "MKBLEAdvertiser.h"
#include "MKCryptoHelper.h"

#include <Arduino.h>


uint8_t MKSeedArray[] = 
    {
        0xC1, 
        0xC2, 
        0xC3, 
        0xC4, 
        0xC5, 
    };

 uint8_t MKHeaderArray[] =
    {
        0x71,   // 0x71 (113)
        0x0f,   // 0x0f (15)
        0x55,   // 0x55 (85)
    };

  
int MKBLEAdvertiser::advertisingCount = 0;

MKBLEAdvertiser::MKBLEAdvertiser() {

    // NimBLE supports only one advertisement, so only one MKBLEAdvertiser can be active
    // this check disables any other MKBLEAdvertiser if more objects are instantiated
    if (advertisingCount > 0) {
        advertisementDisabled = true;
    }

    advertisingCount++;
}

void MKBLEAdvertiser::begin(uint16_t _manufacturer_id , 
            const uint8_t* _seedArray, int _seedArraySize, 
            const uint8_t* _headerArray , int _headerArraySize,
            uint8_t _CTXValue1, uint8_t _CTXValue2, 
            int _encryptedHeaderOffset, int _encryptedPacketLength) {

    if (advertisementDisabled) {
        Serial.println("WARNING: current platform does not allow multiple BLE Advertisements");
    }

    this->manufacturer_id = _manufacturer_id;
    this->encryptedHeaderOffset = _encryptedHeaderOffset;
    this->encryptedPacketLength = _encryptedPacketLength;
    this->CTXValue1 = _CTXValue1;
    this->CTXValue2 = _CTXValue2;

    // copy default seed
    if ( (_seedArray == nullptr) || (_seedArraySize == 0) ) {
        this->seedArray = MKSeedArray;
        this->seedArraySize = sizeof(MKSeedArray);
    } else {
        this->seedArray = _seedArray;
        this->seedArraySize = _seedArraySize;
    }

    // copy default header
    if ( (_headerArray == nullptr) || (_headerArraySize == 0) ) {
        this->headerArray = MKHeaderArray;
        this->headerArraySize = sizeof(MKHeaderArray);
    } else {
        this->headerArray = _headerArray;
        this->headerArraySize = _headerArraySize;
    }
}

void MKBLEAdvertiser::connect(int connect_duration) {
    isConnected = false;

    uint8_t payload[32];
    int payload_len = getConnectPayload(payload, sizeof(payload));

    if (payload_len > 0) {
        updateAdvertisement(payload, payload_len);
    }

    delay(connect_duration);
    isConnected = true;
}

void MKBLEAdvertiser::disconnect() {
    
    enable_adv = false;
    isConnected = false;

    NimBLEAdvertising *adv = NimBLEDevice::getAdvertising();
    adv->stop();
}

void MKBLEAdvertiser::update() {
    if (!isConnected)
        return;

    uint8_t payload[32];
    int payload_len = getUpdatePayload(payload, sizeof(payload));
    
    if (payload_len > 0) {
        updateAdvertisement(payload, payload_len);
    }
}


// encrypts payload and updates BLE advertisement    
void MKBLEAdvertiser::updateAdvertisement(uint8_t *payload, int payload_len) {
    if ( (advertisementDisabled) || (payload_len<=0) ) {
        return;
    }

    uint8_t encrypted_payload[32];
    int encrypted_payload_len = encryptPayload(payload, payload_len, encrypted_payload, sizeof(encrypted_payload));

    adv_data_len = encrypted_payload_len + 2;

    // check that encrypted payload together with headers does not exceed adv data buffer
    if (adv_data_len > sizeof(adv_data)) {
        Serial.println("WARNING: MKBLEAdvertiser::updateAdvertisement adv_data buffer too short");
        return;
    }

    memcpy(&adv_data[0], &manufacturer_id, sizeof(manufacturer_id));    // company id
    memcpy(&adv_data[2], encrypted_payload, encrypted_payload_len);     // payload

    enable_adv = true;

    
    NimBLEAdvertising *NimBLE_adv = NimBLEDevice::getAdvertising();   

    NimBLE_adv_data.clearData();
    NimBLE_adv_data.setFlags(0x06);
    NimBLE_adv_data.setManufacturerData(adv_data, adv_data_len);
    
    NimBLE_adv->setAdvertisementData(NimBLE_adv_data);
    NimBLE_adv->setAdvertisingInterval(32);

    NimBLE_adv->start();

}

int MKBLEAdvertiser::encryptPayload(uint8_t *payload, int payload_len, uint8_t *destination, int maxlen) {

    if (maxlen < encryptedPacketLength) {
        Serial.println("WARNING: MKBLEAdvertiser::encryptPayload destination buffer too short");
        return 0;
    }

    int encrypted_payload_len = MKCryptoHelper::encryptPayload(seedArray, seedArraySize, headerArray, headerArraySize, 
            payload, payload_len, encryptedHeaderOffset, CTXValue1, CTXValue2, destination, encryptedPacketLength);

    for (int index = encrypted_payload_len; index < encryptedPacketLength; index++)
    {
        destination[index] = (uint8_t)(index + 1);
    }

    return encryptedPacketLength;
}
