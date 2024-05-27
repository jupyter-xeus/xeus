/***************************************************************************
* Copyright (c) Johan Mabille, Sylvain Corlay and Wolf Vollprecht          *
* Copyright (c) QuantStack                                                 *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#include "doctest/doctest.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include "xeus/xhash.hpp"

namespace xeus
{
    enum class endian
    {
        big_endian,
        little_endian,
        mixed
    };

    inline endian endianness()
    {
        uint32_t utmp = 0x01020304;
        char btmp[sizeof(utmp)];
        std::memcpy(&btmp[0], &utmp, sizeof(utmp));
        switch(btmp[0])
        {
        case 0x01:
            return endian::big_endian;
        case 0x04:
            return endian::little_endian;
        default:
            return endian::mixed;
        }
    }

    // Adaptation of tests provided by Austin Appleby in https://github.com/aappleby/smhasher
    template <class F>
    uint32_t verification_test(F f, std::size_t hashbytes)
    {
        uint8_t* key = new uint8_t[256];
        uint8_t* hashes = new uint8_t[hashbytes * 256];
        uint8_t* res = new uint8_t[hashbytes];

        std::memset(key, 0, 256);
        std::memset(hashes, 0, hashbytes * 256);
        std::memset(res, 0, hashbytes);

        // Hash keys of the form {0}, {0,1}, {0,1,2}... up to N=255,using 256-N as
        // the seed
        for(std::size_t i = 0; i < 256; i++)
        {
            key[i] = (uint8_t)i;
            *reinterpret_cast<std::size_t*>(hashes + i * hashbytes) = f(key,i,256-i);
        }

        // Then hash the result array
        *reinterpret_cast<std::size_t*>(res) = f(hashes,hashbytes*256,0);

        // The first four bytes of that hash, interpreted as a little-endian integer, is our
        // verification value
        uint32_t verification = uint32_t((res[0] << 0) | (res[1] << 8) | (res[2] << 16) | (res[3] << 24));

        delete [] key;
        delete [] hashes;
        delete [] res;

        return verification;
    }

    void rand_p(void* buffer, int bytes)
    {
        uint32_t * blocks = reinterpret_cast<uint32_t*>(buffer);

        while(bytes >= 4)
        {
            blocks[0] = static_cast<uint32_t>(std::rand());
            blocks++;
            bytes -= 4;
        }

        uint8_t* tail = reinterpret_cast<uint8_t*>(blocks);
        for(int i = 0; i < bytes; i++)
        {
            tail[i] = static_cast<uint8_t>(std::rand());
        }
    }

    void flipbit(void * block, int len, uint32_t bit)
    {
        uint8_t * b = (uint8_t*)block;
        int byte = int(bit >> 3);
        bit = bit & 0x7;
        if(byte < len)
        {
            b[byte] ^= static_cast<uint8_t>(1 << bit);
        }
    }

    template <class F>
    bool sanity_test(F f, std::size_t hashbytes)
    {
        std::srand(883741);
        bool result = true;

        int reps = 10;
        int keymax = 256;
        int pad = 16;
        int buflen = keymax + pad*3;
  
        uint8_t * buffer1 = new uint8_t[std::size_t(buflen)];
        uint8_t * buffer2 = new uint8_t[std::size_t(buflen)];

        uint8_t * hash1 = new uint8_t[hashbytes];
        uint8_t * hash2 = new uint8_t[hashbytes];

        for(int irep = 0; irep < reps; irep++)
        {
            for(int len = 4; len <= keymax; len++)
            {
                for(int offset = pad; offset < pad*2; offset++)
                {
                    uint8_t * key1 = &buffer1[pad];
                    uint8_t * key2 = &buffer2[pad+offset];

                    rand_p(buffer1,buflen);
                    rand_p(buffer2,buflen);

                    std::memcpy(key2,key1,std::size_t(len));

                    *reinterpret_cast<std::size_t*>(hash1) = f(key1,std::size_t(len),0);

                    for(int bit = 0; bit < (len * 8); bit++)
                    {
                        // Flip a bit, hash the key -> we should get a different result.
                        flipbit(key2,len,uint32_t(bit));
                        *reinterpret_cast<std::size_t*>(hash2) = f(key2,std::size_t(len),0);

                        if(std::memcmp(hash1,hash2,hashbytes) == 0)
                        {
                            result = false;
                        }

                        flipbit(key2,len,uint32_t(bit));
                        *reinterpret_cast<std::size_t*>(hash2) = f(key2,std::size_t(len),0);

                        if(std::memcmp(hash1,hash2,hashbytes) != 0)
                        {
                            result = false;
                        }
                    }
                }
            }
        }
        
        delete [] buffer1;
        delete [] buffer2;
        delete [] hash1;
        delete [] hash2;
        return result;
    }

    TEST_SUITE("hash") {
    TEST_CASE("verification")
    {
#if INTPTR_MAX == INT64_MAX
      uint32_t res;
      switch(endianness()) {
        case endian::big_endian:
          res = 0x8fda498d;
          break;
        case endian::little_endian:
          res = sizeof(std::size_t) == 4 ? 0x27864c1e : 0x1f0d3804;
          break;
        default:
          CHECK_MESSAGE(false, "unsupported exotic architecture");
      }
#elif INTPTR_MAX == INT32_MAX
        uint32_t res = sizeof(std::size_t) == 4 ? 0x27864c1e : 0xdd537c05;
#else
#error Unknown pointer size or missing size macros!
#endif
        uint32_t actual = verification_test(&hash_bytes, sizeof(std::size_t));

        REQUIRE_EQ(actual, res);
    }

    TEST_CASE("sanity")
    {
        REQUIRE(sanity_test(&hash_bytes, sizeof(std::size_t)));
    }
    }
}

