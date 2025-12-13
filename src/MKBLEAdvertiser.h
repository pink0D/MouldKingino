//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MK_BLE_ADVERTISER_H_
#define _MK_BLE_ADVERTISER_H_

#include <Arduino.h>

// auto detect Bluetooth library
#if __has_include(<Bluepad32.h>)
    #define MK_IMPL_BTSTACK
#elif __has_include(<btstack.h>)
    #define MK_IMPL_BTSTACK
#elif __has_include(<NimBLEDevice.h>)
    #define MK_IMPL_NIMBLE
#else
    #error No supported Bluetooth libraries found
#endif

// NimBLE includes
#ifdef MK_IMPL_NIMBLE
    #include <NimBLEDevice.h>
    #include <NimBLEAdvertisementData.h>
#endif

// BTStack includes
#ifdef MK_IMPL_BTSTACK
    #include "freertos/semphr.h"
#endif

class MKBLEAdvertiser {
    public:
        void begin();

        virtual void setInstanceNumber(int instanceNum) = 0;

        void connect(int _connect_duration);
        void disconnect();
        void update();

        virtual void setChannelValue(int instance, int channel, double normalizedValue) = 0;
        virtual void resetChannels(int instance) = 0;
        virtual int getChannelCount() = 0;

    protected:
        MKBLEAdvertiser();        

        // prepares unencrypted payload and returns its size
        virtual int getConnectPayload(uint8_t *outPayload, int outMaxLen) = 0;
        virtual int getUpdatePayload(uint8_t *outPayload, int outMaxLen) = 0;

        // sets updateNeeded flag
        void setDataUpdated() {
          dataUpdated = true;  
        };

    private:
        // encrypts the payload and updates BLE advertisement
        void startAdvertising(uint8_t *payload, int payloadLen);
        void stopAdvertising();

        // module specific encrypt function
        int encryptPayload(uint8_t *payload, int payloadLen, uint8_t *destination, int maxlen);

        static int advertisingCount;
        bool advertisingDisabled = false;

        bool dataUpdated = false;
        bool isConnected = false;

        // raw advertisement data
        bool adv_start = false;
        uint8_t adv_data[32];
        int adv_data_len;
        
        // implementation specific & thread safe BLE API invocation
        void updateBLEAdvertisingState();

        // library specific BLE implementation
    #ifdef MK_IMPL_NIMBLE
        NimBLEAdvertisementData NimBLE_adv_data; 
    #endif

    #ifdef MK_IMPL_BTSTACK
        SemaphoreHandle_t adv_mutex;
        static void btstackCallback(void *context);
        void btstackUpdateAdvertisingState();
    #endif

};

#endif