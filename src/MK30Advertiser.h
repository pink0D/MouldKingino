//
// Mould King 3.0 Module support
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MK30_ADVERTISER_H
#define _MK30_ADVERTISER_H

#include "MKBLEAdvertiser.h"

class MK30Advertiser : public MKBLEAdvertiser {
    public:
        // 3.0 modules use one BLE advertiser per module instance.
        static MK30Advertiser* getAdvertiser(int instanceNum) {
            return new MK30Advertiser(instanceNum);
        };

        static void releaseAdvertiser(MK30Advertiser* adv) {
            delete adv;
        };

        virtual void setInstanceNumber(int instanceNum);
        virtual void setChannelValue(int instance, int channel, float normalizedValue);
        virtual void resetChannels(int instance);
        virtual int getChannelCount() { return 4; }; // possible 4 channels - not verified

    protected:
        virtual int getConnectPayload(uint8_t *outPayload, int outMaxLen);
        virtual int getUpdatePayload(uint8_t *outPayload, int outMaxLen);

    private:
        MK30Advertiser(int instanceNum);
        void initChannelData();

        uint8_t channelData[8];
};

#endif
