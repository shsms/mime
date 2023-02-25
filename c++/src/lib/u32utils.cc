#include <internal/u32utils.hh>

namespace mime::u32 {

bool isspace(wchar_t c) {
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

void ltrim(std::wstring &s) {
    auto to = std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); });
    s.erase(s.begin(), to);
}

void ltrim(immer::flex_vector<wchar_t> &s) {
    auto to = std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !isspace(ch); });
    s = s.erase(0, to - s.begin());
}

void rtrim(std::wstring &s) {
    auto from =
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base();
    s.erase(from, s.end());
}

void rtrim(immer::flex_vector<wchar_t> &s) {
    auto from =
        std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !isspace(ch); }).base();
    s = s.erase(from - s.begin(), s.end() - s.begin());
}

void trim(std::wstring &s) {
    ltrim(s);
    rtrim(s);
}

void trim(immer::flex_vector<wchar_t> &s) {
    ltrim(s);
    rtrim(s);
}

wchar_t tolower(wchar_t a) {
    if (a >= U'A' && a <= U'Z') {
        return a - U'A' + U'a';
    }
    return a;
}

wchar_t toupper(wchar_t a) {
    if (a >= U'a' && a <= U'z') {
        return a - U'a' + U'A';
    }
    return a;
}
} // namespace mime::u32
