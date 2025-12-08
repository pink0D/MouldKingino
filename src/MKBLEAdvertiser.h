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

#include <NimBLEDevice.h>
#include <NimBLEAdvertisementData.h>


class MKBLEAdvertiser {
    public:
        void begin(uint16_t _manufacturer_id, 
            const uint8_t* _seedArray, int _seedArraySize, 
            const uint8_t* _headerArray, int _headerArraySize,
            uint8_t _CTXValue1, uint8_t _CTXValue2, 
            int _encryptedHeaderOffset, int _encryptedPacketLength);

        virtual void setInstanceNumber(int instanceNum) = 0;

        void connect(int _connect_duration);
        void disconnect();
        void update();

        virtual void setChannelValue(int instance, int channel, double normalized_value) = 0;

    protected:
        MKBLEAdvertiser();        

        // prepares unencrypted payload and returns its size
        virtual int getConnectPayload(uint8_t *out_payload, int out_maxlen) = 0;
        virtual int getUpdatePayload(uint8_t *out_payload, int out_maxlen) = 0;

    private:
        // encrypts the payload and updates BLE advertisement
        void updateAdvertisement(uint8_t *payload, int payload_len);

        // module specific encrypt function
        int encryptPayload(uint8_t *payload, int payload_len, uint8_t *destination, int maxlen);

        static int advertisingCount;
        bool advertisementDisabled = false;

        bool isConnected = false;

        NimBLEAdvertisementData NimBLE_adv_data; 

        uint16_t manufacturer_id;

        uint8_t adv_data[32];
        int adv_data_len;
        bool enable_adv = false;
        
        int encryptedHeaderOffset;
        int encryptedPacketLength;

        const uint8_t* seedArray;
        int seedArraySize;
        const uint8_t* headerArray;
        int headerArraySize;
        uint8_t CTXValue1;
        uint8_t CTXValue2;
};

#endif