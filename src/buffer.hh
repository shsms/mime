#ifndef MIME_BUFFER_HH
#define MIME_BUFFER_HH

#include <immer/box.hpp>
#include <immer/flex_vector.hpp>
#include <immer/map.hpp>
#include <immer/vector.hpp>

namespace mime {

struct cursor {
    std::optional<std::size_t> mark{};
    std::size_t point{};
};

using text = immer::flex_vector<char32_t>;

std::string to_string(text t);

struct buffer {
    text contents;
    immer::box<std::string> file_name;
    immer::vector<cursor> cursors;
};

struct buffer_text {
    buffer b;
    text t;

    text get_text() const { return t; }
    buffer get_buffer() const { return b; }
};

struct buffer_bool {
    buffer b;
    bool success;

    bool get_bool() const { return success; }
    buffer get_buffer() const { return b; }
};

struct buffer_int {
    buffer b;
    int n;

    buffer get_buffer() const { return b; }
    int get_int() const { return n; }
};

buffer_bool open(std::string name);
void save(buffer b);
void save_as(buffer b, std::string name);
buffer set_mark(buffer b, std::size_t cursor);

template <typename T> buffer_bool find(buffer b, std::size_t cursor, T t, std::size_t lim);
template <typename T> buffer_bool rfind(buffer b, std::size_t cursor, T t, std::size_t lim);
template <typename T> buffer_bool find_fuzzy(buffer b, std::size_t cursor, T t, std::size_t lim);
// buffer_bool find_beg(buffer b, std::size_t cursor, std::string t, std::size_t lim);
// buffer_bool rfind_end(buffer b, std::size_t cursor, std::string t, std::size_t lim);

// TODO: switch to buffer_int
buffer_int replace(buffer b, std::size_t cursor, std::string from, std::string to, std::size_t n);

text copy(buffer b, std::size_t cursor);
buffer_text cut(buffer b, std::size_t cursor);
buffer paste(buffer b, std::size_t cursor, text t);
buffer paste(buffer b, std::size_t cursor, std::string t);
buffer erase_region(buffer b, std::size_t cursor);
buffer clear(buffer b);

buffer_int new_cursor(buffer b);
std::size_t get_pos(buffer b, std::size_t cursor);
buffer_bool goto_pos(buffer b, std::size_t cursor, std::size_t pos);

// // repeatable navigation
buffer forward(buffer b, std::size_t cursor, std::size_t n);
buffer backward(buffer b, std::size_t cursor, std::size_t n);
buffer_bool next_line(buffer b, std::size_t cursor, std::size_t n);
buffer_bool prev_line(buffer b, std::size_t cursor, std::size_t n);
// buffer forward_word(buffer b, std::size_t cursor, std::size_t n);
// buffer backward_word(buffer b, std::size_t cursor, std::size_t n);

// // non-repeatable navigation
buffer start_of_buffer(buffer b, std::size_t cursor);
buffer end_of_buffer(buffer b, std::size_t cursor);
buffer start_of_line(buffer b, std::size_t cursor);
buffer end_of_line(buffer b, std::size_t cursor);

} // namespace mime

#endif /* MIME_BUFFER_HH */
