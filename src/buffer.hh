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

buffer open_file(std::string name);

buffer find(buffer b, std::string cursor_name, std::string text);
buffer set_mark(buffer b, std::string cursor_name);
std::string copy(buffer b, std::string cursor_name);

} // namespace meme

#endif /* MEME_BUFFER_HH */
