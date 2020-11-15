#ifndef MIME_U32UTILS_HH
#define MIME_U32UTILS_HH

#include <algorithm>
#include <cctype>
#include <locale>
#include <string>

namespace mime::u32 {
bool isspace(char32_t c) {
    switch (c) {
    case U' ':
    case U'\n':
    case U'\t':
    case U'\r':
    case U' ':
        return true;
    default:
        return false;
    }
}

// trim from start (in place)
static inline void ltrim(std::u32string &s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::u32string &s) {
    s.erase(
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base(),
        s.end());
}

// trim from both ends (in place)
static inline void trim(std::u32string &s) {
    ltrim(s);
    rtrim(s);
}

char32_t tolower(char32_t a) {
    if (a >= U'A' && a <= U'Z') {
	return a - U'A' + U'a';
    }
    return a;
}

char32_t toupper(char32_t a) {
    if (a >= U'a' && a <= U'z') {
	return a - U'a' + U'A';
    }
    return a;
}
} // namespace mime::u32
#endif /* MIME_U32UTILS_HH */
