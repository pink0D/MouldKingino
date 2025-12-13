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

#ifdef MK_IMPL_BTSTACK
#include <btstack.h>
#endif

static const uint8_t MKSeedArray[] = 
    {
        0xC1, 
        0xC2, 
        0xC3, 
        0xC4, 
        0xC5, 
    };

 static const uint8_t MKHeaderArray[] =
    {
        0x71,   // 0x71 (113)
        0x0f,   // 0x0f (15)
        0x55,   // 0x55 (85)
    };

static const int encryptedHeaderOffset = 15;
static const int encryptedPacketLength = 24;

static const uint8_t CTXValue1 = 0x3f;
static const uint8_t CTXValue2 = 0x25;

static const uint16_t manufacturer_id = 0xFFF0;

int MKBLEAdvertiser::encryptPayload(uint8_t *payload, int payloadLen, uint8_t *destination, int maxlen) {

    if (maxlen < encryptedPacketLength) {
        Serial.println("WARNING: MKBLEAdvertiser::encryptPayload destination buffer too short");
        return 0;
    }

    int encrypted_payload_len = MKCryptoHelper::encryptPayload(MKSeedArray, sizeof(MKSeedArray), MKHeaderArray, sizeof(MKHeaderArray), 
            payload, payloadLen, encryptedHeaderOffset, CTXValue1, CTXValue2, destination, encryptedPacketLength);

    for (int index = encrypted_payload_len; index < encryptedPacketLength; index++)
    {
        destination[index] = (uint8_t)(index + 1);
    }

    return encryptedPacketLength;
}

int MKBLEAdvertiser::advertisingCount = 0;

MKBLEAdvertiser::MKBLEAdvertiser() {

    // NimBLE supports only one advertisement, so only one MKBLEAdvertiser can be active
    // this check disables any other MKBLEAdvertiser if more objects are instantiated
    if (advertisingCount > 0) {
        advertisingDisabled = true;
    }

    advertisingCount++;
}

void MKBLEAdvertiser::begin() {

    if (advertisingDisabled) {
        Serial.println("WARNING: current platform does not allow multiple BLE Advertisements");
    }

    adv_mutex = xSemaphoreCreateMutex();
    if (adv_mutex == nullptr) {
        Serial.println("WARNING: MKBLEAdvertiser::begin failed to create mutex");
        advertisingDisabled = true;
    }
}

void MKBLEAdvertiser::connect(int connect_duration) {
    isConnected = false;

    uint8_t payload[32];
    int payload_len = getConnectPayload(payload, sizeof(payload));

    if (payload_len > 0) {
        startAdvertising(payload, payload_len);
    }

    delay(connect_duration);
    isConnected = true;

    setDataUpdated();
    update(); // update advertisement with default channel data 
}

void MKBLEAdvertiser::disconnect() {
    
    stopAdvertising();    
}

void MKBLEAdvertiser::update() {
    if ( (!isConnected) || (!dataUpdated) )
        return;

    uint8_t payload[32];
    int payload_len = getUpdatePayload(payload, sizeof(payload));
    
    if (payload_len > 0) {
        startAdvertising(payload, payload_len);
    }

    dataUpdated = false;
}


// encrypts payload and updates BLE advertisement    
void MKBLEAdvertiser::startAdvertising(uint8_t *payload, int payloadLen) {

    if ( (advertisingDisabled) || (payloadLen<=0) ) {
        return;
    }
   
    uint8_t encrypted_payload[32];
    int encrypted_payload_len = encryptPayload(payload, payloadLen, encrypted_payload, sizeof(encrypted_payload));

#ifdef MK_IMPL_NIMBLE
    adv_data_len = encrypted_payload_len + 2;
#endif

#ifdef MK_IMPL_BTSTACK
    adv_data_len = encrypted_payload_len + 4 + 3;
#endif


    // check that encrypted payload together with headers does not exceed adv data buffer
    if (adv_data_len > sizeof(adv_data)) {
        Serial.println("WARNING: MKBLEAdvertiser::startAdvertising adv_data buffer too short");
        return;
    }

#ifdef MK_IMPL_NIMBLE
    memcpy(&adv_data[0], &manufacturer_id, sizeof(manufacturer_id));    // company id
    memcpy(&adv_data[2], encrypted_payload, encrypted_payload_len);     // payload
#endif

#ifdef MK_IMPL_BTSTACK

    xSemaphoreTake(adv_mutex, portMAX_DELAY);

    // Flags
    adv_data[0] = 0x02;
    adv_data[1] = 0x01;
    adv_data[2] = 0x06;

    // Manufacturer Data
    adv_data[3] = (uint8_t) encrypted_payload_len + 3;                  // MF Data length 
    adv_data[4] = 0xFF;                                                 // Type = Manufacturer Data
    memcpy(&adv_data[5], &manufacturer_id, sizeof(manufacturer_id));    // company id
    memcpy(&adv_data[7], encrypted_payload, encrypted_payload_len);     // payload

    xSemaphoreGive(adv_mutex);

#endif

    adv_start = true;
    updateBLEAdvertisingState();
}

void MKBLEAdvertiser::stopAdvertising() {

    isConnected = false;

    adv_start = false;
    updateBLEAdvertisingState();
}


#ifdef MK_IMPL_NIMBLE

void MKBLEAdvertiser::updateBLEAdvertisingState() {

    if (adv_start) {

        NimBLEAdvertising *NimBLE_adv = NimBLEDevice::getAdvertising();   

        NimBLE_adv_data.clearData();
        NimBLE_adv_data.setFlags(0x06);
        NimBLE_adv_data.setManufacturerData(adv_data, adv_data_len);
        
        NimBLE_adv->setAdvertisementData(NimBLE_adv_data);
        NimBLE_adv->setAdvertisingInterval(32);

        NimBLE_adv->start();

    } else {

        NimBLEAdvertising *NimBLE_adv = NimBLEDevice::getAdvertising();
        NimBLE_adv->stop();
    }
}

#endif

#ifdef MK_IMPL_BTSTACK

static btstack_context_callback_registration_t update_callback_registration;

void MKBLEAdvertiser::updateBLEAdvertisingState() {
    update_callback_registration.callback = &btstackCallback;
    update_callback_registration.context = this;
    btstack_run_loop_execute_on_main_thread(&update_callback_registration);    
}

void MKBLEAdvertiser::btstackCallback(void *context) {
    static_cast<MKBLEAdvertiser*>(context)->btstackUpdateAdvertisingState();
}

void MKBLEAdvertiser::btstackUpdateAdvertisingState() {
    if (adv_start) {

        xSemaphoreTake(adv_mutex, portMAX_DELAY);

        uint8_t adv_type = 0;
        bd_addr_t null_addr;
        memset(null_addr, 0, 6);

        gap_advertisements_set_params(32, 32, adv_type, 0, null_addr, 0x07, 0x00);
        gap_advertisements_set_data(adv_data_len, adv_data);
        gap_advertisements_enable(1);

        xSemaphoreGive(adv_mutex);

    } else {
        gap_advertisements_enable(0);
    }
}

#endif