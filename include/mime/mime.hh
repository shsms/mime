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
    enum open_spec {
        try_open = 0,
        must_open = 1,
    };

    buffer();
    buffer(const std::string &fname, open_spec spec);
    buffer(const std::string &fname);

    inline bool empty() { return contents.empty(); }
    inline std::size_t size() { return contents.size(); }

    void save();
    void save_as(const std::string &fname);
    void set_mark();
    long get_mark();

    inline text get_contents() { return contents; }

    template <typename T> bool find(T t);
    template <typename T> bool rfind(T t);
    template <typename T> bool find_fuzzy(T t);

    int replace(std::string from, std::string to, std::size_t n);
    int replace(std::string from, std::string to);

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

    void forward();
    void backward();
    int next_line();
    int prev_line();

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
