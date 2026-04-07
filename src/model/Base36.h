#pragma once
#include <string>

namespace Model {

/// Converts a base-36 two-character string (e.g. "01", "ZZ") to an integer [1, 1295].
/// Returns -1 if the string is invalid or represents 0 ("00").
int base36ToInt(const char* s);

/// Converts an integer [1, 1295] to a base-36 two-character string (e.g. 1 → "01", 1295 → "ZZ").
/// Returns "00" for out-of-range values.
std::string intToBase36(int n);

} // namespace Model
