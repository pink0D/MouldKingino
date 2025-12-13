//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MK60_ADVERTISER_H
#define _MK60_ADVERTISER_H

#include "MKBLEAdvertiser.h"

class MK60Advertiser : public MKBLEAdvertiser {
    public:

        // MK 6.0 needs an advertiser for each module instance
        static MK60Advertiser* getAdvertiser(int instanceNum) {           
            return new MK60Advertiser(instanceNum);
        };

        static void releaseAdvertiser(MK60Advertiser* adv) {
            delete adv;
        };

        virtual void setInstanceNumber(int instanceNum);

        virtual void setChannelValue(int instance, int channel, double normalizedValue);
        virtual void resetChannels(int instance);
        virtual int getChannelCount() { return 6; };

    protected:
        virtual int getConnectPayload(uint8_t *outPayload, int outMaxLen);
        virtual int getUpdatePayload(uint8_t *outPayload, int outMaxLen);

    private:
        MK60Advertiser(int instanceNum);
        void initChannelData(int instanceNum);

        uint8_t channelData[10];
};

#endif