#ifndef MIME_U32UTILS_HH
#define MIME_U32UTILS_HH

#include <algorithm>
#include <cctype>
#include <immer/flex_vector.hpp>
#include <locale>
#include <string>

namespace mime::u32 {
bool isspace(char32_t c);

void ltrim(std::u32string &s);
void ltrim(immer::flex_vector<char32_t> &s);
void rtrim(std::u32string &s);
void rtrim(immer::flex_vector<char32_t> &s);
void trim(std::u32string &s);
void trim(immer::flex_vector<char32_t> &s);
char32_t tolower(char32_t a);
char32_t toupper(char32_t a);
} // namespace mime::u32
#endif /* MIME_U32UTILS_HH */
