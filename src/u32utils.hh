#ifndef MEME_U32UTILS_HH
#define MEME_U32UTILS_HH

#include <algorithm>
#include <cctype>
#include <locale>
#include <string>

namespace meme::u32 {
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
} // namespace meme::u32
#endif /* MEME_U32UTILS_HH */
