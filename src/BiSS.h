#include <Arduino.h>
#include <SPI.h>

#include "CustomSPI.h"
// #include "CRC.h"

template <uint8_t kNumPreambleBits, uint8_t kNumAckBits, uint8_t kNumDataBits, uint8_t kNumCrcBits>
class BiSS
{
public:
    BiSS(CustomSPIClass &spi, uint32_t clock_speed /* , CRC &crc */)
        : spi_{spi}, settings_{clock_speed, MSBFIRST, SPI_MODE3} /* , crc_{crc} */
    {
    }

    struct Data  // order backwards because of big vs little endian
    {
        uint16_t CRC : kNumCrcBits;
        bool Warn : 1;
        bool Error : 1;
        uint64_t Data : kNumDataBits;
        bool CDS : 1;
        bool Start : 1;
        uint64_t ACK : kNumAckBits;
        uint64_t Preamble : kNumPreambleBits;
    };

    union BISSCData
    {
        uint64_t raw;
        Data data;
    };

    void Begin() { spi_.begin(); }

    // Can read up to 64 bits of data
    Data ReadPacket(bool CDM = 0)
    {
        //                start             ACK      Start CDS      Data    Error Warn  CRC
        uint8_t n_bits = kNumPreambleBits + kNumAckBits + 1 + 1 + kNumDataBits + 1 + 1 + kNumCrcBits;
        BISSCData data{0};
        spi_.beginTransaction(settings_);
        for (int i = 0; i < (n_bits / 8) - 1; i++)
        {
            data.raw |= spi_.transfer(0) << (n_bits - (8 * (i + 1)));  // MSB first reading
        }
        // last 8+bits%8 bits
        data.raw |= (spi_.transfern(0, 8 + (n_bits % 8) + !CDM)
                     << (8 - (n_bits % 8)));  // ignore trailing bits, append clock pulse if !CDM
        spi_.endTransaction();

        // set error if failed ACK or start
        // todo: check CRC
        if (data.data.ACK != 0 || data.data.Start != 1)
        {
            data.data.Error = true;
        }
        /* uint8_t b = ~data.data.CRC;
        b = ((b * 0x0802LU & 0x22110LU) | (b * 0x8020LU & 0x8440LU)) * 0x10101LU >> 16;
        data.data.CRC = b >> 2;
        Serial.println(crc_.CalculateCRC(data.raw)); */
        // Serial.println(data.raw);
        return data.data;
    }

private:
    CustomSPIClass &spi_;
    SPISettings settings_;
    // CRC &crc_;
};

template <uint8_t kNumPositionBits, bool kMultiTurn>
class BiSSAKSIM2
{
public:
    BiSSAKSIM2(CustomSPIClass &spi, uint32_t clock_speed) : bissc_{spi, clock_speed /* , *(new CRC{0x43, 7}) */} {}

    void Begin() { bissc_.Begin(); }

    uint64_t ReadRawPosition()
    {
        auto data = bissc_.ReadPacket();
        warn_ = data.Warn;
        error_ = data.Error;
        return data.Data;
    }

    void Zero() { zero_value_ = ReadRawPosition(); }

    void SetZeroPosition(float zero_position) { zero_value_ = zero_position * kMaxValue / (2 * M_PI); }

    float ReadPosition()  // radians
    {
        int64_t offset_position = ReadRawPosition() - zero_value_;
        if (offset_position < 0)
        {
            offset_position = kMaxValue + offset_position;
        }
        float position = offset_position * (2 * M_PI) / kMaxValue;
        if (position > M_PI)
        {
            position = position - (2 * M_PI);
        }
        return position;
    }

    bool GetWarning()
    {
        return !warn_;  // active low
    }

    bool GetError()
    {
        return !error_;  // active low
    }

    // void QueueRead()

private:
    BiSS<2, 13, kNumPositionBits + (kMultiTurn * 16), 6> bissc_;
    uint64_t kMaxValue{(1 << kNumPositionBits) - 1};
    bool warn_ = false;
    bool error_ = false;
    uint64_t zero_value_ = 0;
};
