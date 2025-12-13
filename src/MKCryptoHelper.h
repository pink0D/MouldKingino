//
// Repository info:     https://github.com/pink0D/MouldKingino
//
// Licensed under the MIT license. See LICENSE file in the project root for details.
//
// Mould King BLE protocol was originally implemented by @J0EK3R and @vicocz
// https://github.com/vicocz/brickcontroller2
//

#ifndef _MK_CRYPTO_HELPER_H_
#define _MK_CRYPTO_HELPER_H_

#include <Arduino.h>

//#define CRYPTO_DEBUG
//#define CRYPTO_DEBUG_EXT

class MKCryptoHelper {

    private:

        static uint8_t reverseByte(uint8_t value) {
            value = (uint8_t)(((value * 0x0802U & 0x22110U) | (value * 0x8020U & 0x88440U)) * 0x10101U >> 16);
            return value;
        };

        static uint16_t reverseWord(uint16_t value)
        {
            // Swap odd and even bits
            value = (uint16_t)(((value & 0xAAAA) >> 1) | ((value & 0x5555) << 1));
            // Swap consecutive pairs
            value = (uint16_t)(((value & 0xCCCC) >> 2) | ((value & 0x3333) << 2));
            // Swap nibbles
            value = (uint16_t)(((value & 0xF0F0) >> 4) | ((value & 0x0F0F) << 4));
            // Swap bytes
            value = (uint16_t)((value >> 8) | (value << 8));
            return value;
        };

        static uint16_t checkCRC16(const uint8_t* array1, int array1_len, const uint8_t* array2, int array2_len)
        {
            int result = 0xFFFF;

            // Process array1 in reverse order
            for (int i = array1_len - 1; i >= 0; i--)
            {
                result ^= array1[i] << 8;
                for (int j = 0; j < 8; j++)
                {
                    result = (result & 0x8000) == 0 ? result << 1 : (result << 1) ^ 0x1021;
                }
            }

            // Process array2 in forward order, with bit inversion
            for (int i = 0; i < array2_len; i++)
            {
                result ^= reverseByte(array2[i]) << 8;
                for (int j = 0; j < 8; j++)
                {
                    result = (result & 0x8000) == 0 ? result << 1 : (result << 1) ^ 0x1021;
                }
            }

            // Final inversion and XOR
            return (uint16_t)(reverseWord((uint16_t)result) ^ 0xFFFF);
        };

        static void whiteningInit(uint8_t val, uint8_t* ctx) {
            ctx[0] = 1;
            ctx[1] = (uint8_t)((val >> 5) & 1);
            ctx[2] = (uint8_t)((val >> 4) & 1);
            ctx[3] = (uint8_t)((val >> 3) & 1);
            ctx[4] = (uint8_t)((val >> 2) & 1);
            ctx[5] = (uint8_t)((val >> 1) & 1);
            ctx[6] = (uint8_t)(val & 1);
        }

        static uint8_t whiteningOutput(uint8_t* ctx) {
            uint8_t value_3 = ctx[3];
            uint8_t value_6 = ctx[6];
            ctx[3] = ctx[2];
            ctx[2] = ctx[1];
            ctx[1] = ctx[0];
            ctx[0] = ctx[6];
            ctx[6] = ctx[5];
            ctx[5] = ctx[4];
            ctx[4] = (uint8_t)(value_3 ^ value_6);
            return ctx[0];
        }

        static void whiteningEncode(uint8_t* data, int dataStartIndex, int len, uint8_t* ctx)
        {
            for (int index = 0; index < len; index++)
            {
                uint8_t currentByte = data[dataStartIndex + index];
                int currentResult = 0;
                for (int bitIndex = 0; bitIndex < 8; bitIndex++)
                {
                    uint8_t uVar2 = whiteningOutput(ctx);
                    currentResult |= ((uVar2 ^ ((currentByte >> bitIndex) & 1)) << bitIndex);
                }
                data[dataStartIndex + index] = (uint8_t)currentResult;
            }
        }

        static void debugPrint(const char* title, const uint8_t* data, int data_len) {
            Serial.printf("\r\n%s\r\n", title);
            for (int i = 0; i < data_len; i++)
                Serial.printf("%02X ", data[i]);
            Serial.printf("\r\n");
        }

    public:
        static int encryptPayload(const uint8_t* seed, int seedLength, const uint8_t* header, int headerLength, const uint8_t* data, int dataLength, int headerOffset, uint8_t ctxValue1, uint8_t ctxValue2, uint8_t* out_rfPayload, int out_rfPayloadLength, int out_rfPayloadOffset = 0)
        {
            const int checksumLength = 2;

            int resultArrayLength = headerLength + seedLength + dataLength + checksumLength;
            if (resultArrayLength > out_rfPayloadLength - out_rfPayloadOffset)
            {
                Serial.println("WARNING: MKCryptoHelper::GetRfPayload data buffer too short");
                return 0;
            }

            int seedOffset = headerOffset + headerLength;
            int dataOffset = seedOffset + seedLength;
            int checksumOffset = dataOffset + dataLength;
            int resultBufferLength = checksumOffset + checksumLength;

            uint8_t resultBuffer[64];
            memset(resultBuffer, 0 , sizeof(resultBuffer));

            if (resultBufferLength > sizeof(resultBuffer)) {
                Serial.println("WARNING: MKCryptoHelper::GetRfPayload resultBuffer buffer too short");
                return 0;
            }


            // Copy header
            memcpy(&resultBuffer[headerOffset], header, headerLength);

            #ifdef CRYPTO_DEBUG_EXT                
            debugPrint("header", resultBuffer, resultBufferLength);
            #endif

            // Reverse-copy seed-array into resultBuffer after header
            for (int index = 0; index < seedLength; index++)
            {
                resultBuffer[seedOffset + index] = seed[seedLength - 1 - index];
            }

            #ifdef CRYPTO_DEBUG_EXT                
            debugPrint("seed", resultBuffer, resultBufferLength);
            #endif

            // Invert bytes of header and seed-array in resultBuffer
            for (int index = 0; index < headerLength + seedLength; index++)
            {
                resultBuffer[headerOffset + index] = reverseByte(resultBuffer[headerOffset + index]);
            }

            #ifdef CRYPTO_DEBUG_EXT                
            debugPrint("invert", resultBuffer, resultBufferLength);
            #endif

            // Copy data
            memcpy(&resultBuffer[dataOffset], data, dataLength);

            #ifdef CRYPTO_DEBUG_EXT                
            debugPrint("data", resultBuffer, resultBufferLength);
            #endif

            // Write checksum
            uint16_t checksum = checkCRC16(seed, seedLength, data, dataLength);
            memcpy(&resultBuffer[checksumOffset], &checksum, checksumLength);

            #ifdef CRYPTO_DEBUG                
            debugPrint("before whitening", resultBuffer, resultBufferLength);
            #endif

            // Whitening
            uint8_t ctxArray1[7];
            whiteningInit(ctxValue1, ctxArray1);
            whiteningEncode(resultBuffer, seedOffset, seedLength + dataLength + checksumLength, ctxArray1);

            uint8_t ctxArray2[7];
            whiteningInit(ctxValue2, ctxArray2);
            whiteningEncode(resultBuffer, 0, resultBufferLength, ctxArray2);

            #ifdef CRYPTO_DEBUG_EXT                
            debugPrint("after whitening", resultBuffer, resultBufferLength);
            #endif

            // Copy result to rfPayload
            memcpy(&out_rfPayload[out_rfPayloadOffset], &resultBuffer[headerOffset], resultArrayLength);

            #ifdef CRYPTO_DEBUG                
            debugPrint("final", out_rfPayload, resultArrayLength);
            #endif

            return resultArrayLength;
        };
};


#endif