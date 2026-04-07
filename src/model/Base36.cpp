#include "model/Base36.h"
#include <cctype>
#include <cstring>

namespace Model {

static int charToDigit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return 10 + (c - 'A');
    if (c >= 'a' && c <= 'z') return 10 + (c - 'a');
    return -1;
}

static char digitToChar(int d) {
    if (d >= 0 && d < 10) return static_cast<char>('0' + d);
    if (d >= 10 && d < 36) return static_cast<char>('A' + (d - 10));
    return '0';
}

int base36ToInt(const char* s) {
    if (!s || std::strlen(s) < 2) return -1;
    int hi = charToDigit(s[0]);
    int lo = charToDigit(s[1]);
    if (hi < 0 || lo < 0) return -1;
    int result = hi * 36 + lo;
    if (result == 0) return -1; // "00" is not a valid slot
    return result;
}

std::string intToBase36(int n) {
    if (n < 1 || n > 1295) return "00";
    char buf[3] = {digitToChar(n / 36), digitToChar(n % 36), '\0'};
    return std::string(buf);
}

} // namespace Model
