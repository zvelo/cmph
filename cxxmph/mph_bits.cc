#include "mph_bits.h"

namespace cxxmph {

const uint8_t dynamic_2bitset::vmask[] = { 0xfc, 0xf3, 0xcf, 0x3f};
dynamic_2bitset::dynamic_2bitset() : size_(0), fill_(false) {}
dynamic_2bitset::dynamic_2bitset(uint32_t size, bool fill)
    : size_(size), fill_(fill), data_(ceil(size / 4.0), ones()*fill) {}
dynamic_2bitset::~dynamic_2bitset() {}

}
