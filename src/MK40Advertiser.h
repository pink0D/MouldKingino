//
// Copyright (c) Dmitry Akulov. All rights reserved.
//
// Repository info:     https://github.com/pink0D/MouldKingino
// Contact information: pink0D.github@gmail.com
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//

#ifndef _MK40_ADVERTISER_H
#define _MK40_ADVERTISER_H

#include "MKBLEAdvertiser.h"

class MK40Advertiser : public MKBLEAdvertiser {
    public:

    // MK 4.0 shares one advertiser for all module instances
        static MK40Advertiser* getAdvertiser(int instanceNum) {
          
            // lazy initialization, so this won't prevent other advertisers being constructed
            static MK40Advertiser instance; 
            return &instance;
        };

        static void releaseAdvertiser(MK40Advertiser* adv) {}; // do nothing

        virtual void setChannelValue(int instance, int channel, double normalizedValue);
        virtual void resetChannels(int instance);
        virtual int getChannelCount() { return 4; };

        // MK 4.0 advertiser is common for all instances
        virtual void setInstanceNumber(int instanceNum);

    protected:
        virtual int getConnectPayload(uint8_t *outPayload, int outMaxLen);
        virtual int getUpdatePayload(uint8_t *outPayload, int outMaxLen);


    private:
        MK40Advertiser();

        void setChannelRawValue(int instance, int channel, uint8_t rawValue);

        uint8_t channelData[10];
};

#endif