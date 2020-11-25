#ifndef MIME_HH
#define MIME_HH

#include <immer/box.hpp>
#include <immer/flex_vector.hpp>
#include <immer/vector.hpp>

namespace mime {

struct bounds {
    std::size_t lower{};
    std::size_t upper{};
};

struct cursor_t {
    std::optional<std::size_t> mark{};
    std::size_t point{};
    std::optional<bounds> view{};
};

using text = immer::flex_vector<char32_t>;

std::string to_string(text t);

class buffer {
  public:
    buffer();
    buffer(const std::string &fname);
    void save();
    void save_as(const std::string &fname);
    void set_mark();

    template <typename T> bool find(T t);
    template <typename T> bool rfind(T t);
    template <typename T> bool find_fuzzy(T t);

    int replace(std::string from, std::string to, std::size_t n);

    inline int replace(std::string from, std::string to) {
        return replace(from, to, 0); // replace all
    }

    text copy();
    text cut();
    void paste(text t);
    void paste(std::string t);
    void erase_region();
    void clear();

    std::size_t new_cursor();
    void use_cursor(std::size_t c);
    std::size_t get_pos();
    bool goto_pos(std::size_t pos);

    void forward(std::size_t n);
    void backward(std::size_t n);
    int next_line(std::size_t n);
    int prev_line(std::size_t n);

    inline void forward() { forward(1); } // forward one char32_t.
    inline void backward() { backward(1); }
    inline int next_line() { return next_line(1); }
    inline int prev_line() { return prev_line(1); }

    void start_of_buffer();
    void end_of_buffer();
    void start_of_line();
    void end_of_line();

    bool start_of_block();
    bool end_of_block();

    bool narrow_to_block();
    bool narrow_to_region();
    void widen();

    // buffer forward_word(buffer b, std::size_t cursor, std::size_t n);
    // buffer backward_word(buffer b, std::size_t cursor, std::size_t n);
    // buffer_bool find_beg(buffer b, std::size_t cursor, std::string t, std::size_t lim);
    // buffer_bool rfind_end(buffer b, std::size_t cursor, std::string t, std::size_t lim);
  private:
    text contents;
    immer::box<std::string> filename;
    immer::vector<cursor_t> cursors;
    std::size_t cursor{};

    void update_all_cursors(std::size_t mark, std::size_t point, std::size_t forward);
};
} // namespace mime

#endif /* MIME_HH */
