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

        virtual void setChannelValue(int instance, int channel, double normalized_value);

        virtual int getConnectPayload(uint8_t *out_payload, int maxlen);
        virtual int getUpdatePayload(uint8_t *out_payload, int maxlen);

        // MK 4.0 advertiser is common for all instances
        virtual void setInstanceNumber(int instanceNum) {};

    private:
        MK40Advertiser();

        uint8_t *connectData;
        uint8_t *channelData;
};

#endif