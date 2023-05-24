#include <cstdint>

class CRC
{
public:
    CRC(uint32_t polynomial, uint8_t length) : kPolynomial{polynomial}, kLength{length} {}

    uint32_t CalculateCRC(uint64_t data)
    {
        uint8_t first_one = GetFirstOne(data);
        while (first_one + 1 >= kLength)
        {
            data ^= kPolynomial << (first_one + 1 - kLength);
            first_one = GetFirstOne(data);
        }
        return data;
    }

private:
    uint32_t kPolynomial;
    uint8_t kLength;

    uint8_t GetFirstOne(uint64_t data)
    {
        uint8_t one = 0;
        data = data >> 1;
        while (data != 0)
        {
            one++;
            data = data >> 1;
        }
        return one;
    }
};