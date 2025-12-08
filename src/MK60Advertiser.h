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

        virtual void setInstanceNumber(int instanceNum);
        virtual void setChannelValue(int instance, int channel, double normalized_value);

        virtual int getConnectPayload(uint8_t *out_payload, int maxlen);
        virtual int getUpdatePayload(uint8_t *out_payload, int maxlen);

    private:
        MK60Advertiser(int instanceNum);

        uint8_t *connectData;
        int connectDataSize;

        uint8_t *channelData;
        int channelDataSize;
};

#endif