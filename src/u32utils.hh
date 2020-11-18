#ifndef MIME_U32UTILS_HH
#define MIME_U32UTILS_HH

#include <algorithm>
#include <cctype>
#include <immer/flex_vector.hpp>
#include <locale>
#include <string>

namespace mime::u32 {
inline bool isspace(char32_t c) {
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
    auto to = std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); });
    s.erase(s.begin(), to);
}

static inline void ltrim(immer::flex_vector<char32_t> &s) {
    auto to = std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); });
    s = s.erase(0, to - s.begin());
}

// trim from end (in place)
static inline void rtrim(std::u32string &s) {
    auto from =
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base();
    s.erase(from, s.end());
}

static inline void rtrim(immer::flex_vector<char32_t> &s) {
    auto from =
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base();
    s = s.erase(from - s.begin(), s.end() - s.begin());
}

// trim from both ends (in place)
template <typename Text> static inline void trim(Text &s) {
    ltrim(s);
    rtrim(s);
}

inline char32_t tolower(char32_t a) {
    if (a >= U'A' && a <= U'Z') {
        return a - U'A' + U'a';
    }
    return a;
}

inline char32_t toupper(char32_t a) {
    if (a >= U'a' && a <= U'z') {
        return a - U'a' + U'A';
    }
    return a;
}
} // namespace mime::u32
#endif /* MIME_U32UTILS_HH */
