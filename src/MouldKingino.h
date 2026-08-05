//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MOULD_KINGINO_H
#define _MOULD_KINGINO_H

#include "MK40Advertiser.h"
#include "MK60Advertiser.h"

#define MODULE_1 0
#define MODULE_2 1
#define MODULE_3 2

#define MOTOR_A 0
#define MOTOR_B 1
#define MOTOR_C 2
#define MOTOR_D 3
#define MOTOR_E 4
#define MOTOR_F 5

class IMKModule {
    public:
        virtual void setInstanceNumber(int instanceNum) = 0;
        virtual void setTxId(uint16_t txId) = 0;
        virtual uint16_t getTxId() = 0;
        virtual void setImmediateUpdates(bool immediateUpdate) = 0;
        virtual void begin() = 0;
        virtual void connect(int duration = 1000) = 0;
        virtual void connectAsync(int duration = 1000, int preDelay = 1000) = 0;
        virtual void disconnect() = 0;
        virtual int getChannelCount() = 0;
        virtual void updateMotorOutput(int channel, float normalized_value) = 0;
        virtual void applyUpdates(uint32_t durationMillis = 0, bool forcedUpdate = false) = 0;
        virtual void resetChannels(uint32_t durationMillis = 0, bool forcedUpdate = false) = 0;

        virtual ~IMKModule() = default; 
};

template<class AdvertiserClass>
class MKModule : public IMKModule {
    public:
        MKModule(int instanceNum = 0, bool immediateUpdate = false) {
            this->advertiser = AdvertiserClass::getAdvertiser(instanceNum);
            this->instanceNum = instanceNum;
            this->immediateUpdate = immediateUpdate;
        };

        ~MKModule() {
            AdvertiserClass::releaseAdvertiser(this->advertiser);
        };

        void setInstanceNumber(int instanceNum) override {
            this->instanceNum = instanceNum;
            this->advertiser->setInstanceNumber(instanceNum);
        };

        void setTxId(uint16_t txId) override {
            advertiser->setTxId(txId);
        };

        uint16_t getTxId() override {
            return advertiser->getTxId();
        };        

        void setImmediateUpdates(bool immediateUpdate) override {
            this->immediateUpdate = immediateUpdate;
        };

        virtual void begin() override {
            advertiser->begin();
        };

        void connect(int duration = 1000) override {
            advertiser->connect(duration);
        };

        void connectAsync(int duration = 1000, int preDelay = 1000) override {
            
            asyncConnectDuration = duration;
            asyncConnectPreDelay = preDelay;

            xTaskCreate( [](void* ctx) {

                MKModule *obj = static_cast<MKModule*>(ctx);

                if (obj->asyncConnectPreDelay > 0) {
                    vTaskDelay(pdMS_TO_TICKS(obj->asyncConnectPreDelay));
                }

                obj->connect(obj->asyncConnectDuration);
                vTaskDelete(NULL);

            }, "MKModule::connectAsync", 2*1024, this, 0, nullptr);
        };

        void disconnect() override {
            advertiser->disconnect();
        };

        void applyUpdates(uint32_t durationMillis = 0, bool forcedUpdate = false) override {
            advertiser->update(durationMillis, forcedUpdate);
        };

        int getChannelCount() override {
            return advertiser->getChannelCount();
        };

        virtual void updateMotorOutput(int channel, float normalized_value) override {
            
            advertiser->setChannelValue(instanceNum, channel, normalized_value);

            if (immediateUpdate)
                applyUpdates();
        };

        void resetChannels(uint32_t durationMillis = 0, bool forcedUpdate = false) override {
            advertiser->resetChannels(instanceNum);
            applyUpdates(durationMillis, forcedUpdate);
        };        

    private:
        int instanceNum = 0;
        bool immediateUpdate = false;
        int asyncConnectDuration = 0;
        int asyncConnectPreDelay = 0;
        AdvertiserClass *advertiser;

};

typedef MKModule<MK40Advertiser> MouldKing40;

typedef MKModule<MK60Advertiser> MouldKing60;

#endif