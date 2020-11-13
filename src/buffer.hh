#ifndef MEME_BUFFER_HH
#define MEME_BUFFER_HH

#include <immer/box.hpp>
#include <immer/vector.hpp>
#include <immer/flex_vector.hpp>
#include <immer/map.hpp>

namespace meme {

struct cursor {
    std::optional<std::size_t> mark{};
    std::size_t point{};
};

using text = immer::flex_vector<wchar_t>;

std::string get_string(text t);

struct buffer {
    text contents;
    immer::box<std::string> file_name;
    immer::vector<cursor> cursors;
};

struct buffer_text {
    buffer b;
    text t;

    text get_text() { return t; }
    buffer get_buffer() { return b; }
};

struct buffer_bool {
    buffer b;
    bool success;

    bool get_bool() { return success; }
    buffer get_buffer() { return b; }
};

buffer open_file(std::string name);

buffer set_mark(buffer b, std::size_t cursor);
buffer_bool find(buffer b, std::size_t cursor, std::string t);
buffer_bool rfind(buffer b, std::size_t cursor, std::string t);

text copy(buffer b, std::size_t cursor);
buffer_text cut(buffer b, std::size_t cursor);
buffer paste(buffer b, std::size_t cursor, text t);

buffer insert(buffer b, std::size_t cursor, std::string t);
// buffer new_cursor(buffer b);

// // repeatable navigation
// buffer forward(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer backward(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer forward_word(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer backward_word(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer forward_subword(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer backward_subword(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer next_line(buffer b, std::size_t cursor, std::size_t n = 0);
// buffer prev_line(buffer b, std::size_t cursor, std::size_t n = 0);

// // non-repeatable navigation
// buffer start_of_buffer(buffer b, std::size_t cursor);
// buffer end_of_buffer(buffer b, std::size_t cursor);
// buffer start_of_line(buffer b, std::size_t cursor);
// buffer end_of_line(buffer b, std::size_t cursor);


} // namespace meme

#endif /* MEME_BUFFER_HH */
