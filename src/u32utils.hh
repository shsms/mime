#ifndef MEME_U32UTILS_HH
#define MEME_U32UTILS_HH

bool isspace(char32_t c) {
    switch(c) {
    case U' ':
    case U'\n':
    case U'\t':
    case U'\r':
	return true;
    default:
	return false;
    }
}

#endif /* MEME_U32UTILS_HH */
