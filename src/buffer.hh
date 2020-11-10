#ifndef MEME_BUFFER_HH
#define MEME_BUFFER_HH

#include <immer/box.hpp>
#include <immer/flex_vector.hpp>
#include <immer/map.hpp>

namespace meme {

struct cursor {
    std::optional<std::size_t> mark{};
    std::size_t point{};
};

using text = immer::flex_vector<wchar_t>;

struct buffer {
    text contents;
    immer::box<std::string> file_name;
    immer::map<std::string, cursor> cursors;
};

struct buffer_text {
    buffer b;
    text t;
};

struct buffer_bool {
    buffer b;
    bool success;
};

buffer open_file(std::string name);

buffer_bool search(buffer b, std::string cursor_name, std::string t);
buffer set_mark(buffer b, std::string cursor_name);
text copy(buffer b, std::string cursor_name);
buffer_text cut(buffer b, std::string cursor_name);

std::string get_string(text t);
} // namespace meme

#endif /* MEME_BUFFER_HH */
