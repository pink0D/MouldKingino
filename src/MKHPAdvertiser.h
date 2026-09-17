//
// Mould King High Performance Power Module support
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MK_HP_ADVERTISER_H
#define _MK_HP_ADVERTISER_H

#include "MKBLEAdvertiser.h"

class MKHPAdvertiser : public MKBLEAdvertiser {
    public:
        // High Performance modules use one BLE advertiser per module instance.
        static MKHPAdvertiser* getAdvertiser(int instanceNum) {
            return new MKHPAdvertiser(instanceNum);
        };

        static void releaseAdvertiser(MKHPAdvertiser* adv) {
            delete adv;
        };

        virtual void setInstanceNumber(int instanceNum);
        virtual void setChannelValue(int instance, int channel, float normalizedValue);
        virtual void resetChannels(int instance);
        virtual int getChannelCount() { return 4; };

    protected:
        virtual int getConnectPayload(uint8_t *outPayload, int outMaxLen);
        virtual int getUpdatePayload(uint8_t *outPayload, int outMaxLen);

    private:
        MKHPAdvertiser(int instanceNum);
        void initChannelData();

        uint8_t channelData[8];
};

#endif
