#ifndef MIME_U32UTILS_HH
#define MIME_U32UTILS_HH

#include <algorithm>
#include <cctype>
#include <immer/flex_vector.hpp>
#include <locale>
#include <string>

namespace mime::u32 {
bool isspace(wchar_t c);

void ltrim(std::wstring &s);
void ltrim(immer::flex_vector<wchar_t> &s);
void rtrim(std::wstring &s);
void rtrim(immer::flex_vector<wchar_t> &s);
void trim(std::wstring &s);
void trim(immer::flex_vector<wchar_t> &s);
wchar_t tolower(wchar_t a);
wchar_t toupper(wchar_t a);
} // namespace mime::u32
#endif /* MIME_U32UTILS_HH */
