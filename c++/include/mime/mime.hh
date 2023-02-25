#ifndef MIME_HH
#define MIME_HH

#include <immer/box.hpp>
#include <immer/flex_vector.hpp>
#include <immer/vector.hpp>
#include <optional>
#include <regex>

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

struct regex_t {
    immer::box<std::wregex> rex;
    bool empty{};
};

using text = immer::flex_vector<wchar_t>;

std::string to_string(const text &t);
regex_t regex(const std::string &r);

class buffer {
  public:
    enum open_spec {
        try_open = 0,
        must_open = 1,
    };

    buffer();
    buffer(const std::string &fname, open_spec spec);
    buffer(const std::string &fname);
    buffer(const text &fname);

    inline bool empty() const {
        const auto &v = cursors[cursor].view;
        if (v.has_value()) {
            return v->upper == v->lower;
        }
        return contents.empty();
    }
    inline std::size_t size() const {
        const auto &v = cursors[cursor].view;
        if (v.has_value()) {
            return v->upper - v->lower;
        }
        return contents.size();
    }
    inline bool narrowed() const { return cursors[cursor].view.has_value(); }

    void save() const;
    void save_as(const std::string &fname) const;
    void set_mark();
    long get_mark() const;

    inline text get_contents() const {
        const auto &v = cursors[cursor].view;
        if (v.has_value()) {
            return contents.drop(v->lower).take(v->upper - v->lower);
        }
        return contents;
    }

    inline std::string get_name() const { return filename; }

    template <typename T> long find(const T &t);
    template <typename T> long rfind(const T &t);
    template <typename T> long find_fuzzy(const T &t);

    int replace(const std::string &from, const std::string &to, std::size_t n);
    int replace(const std::string &from, const std::string &to);

    text copy() const;
    text cut();
    void paste(const text &t);
    void paste(const std::string &t);
    void erase_region();
    void clear();

    std::size_t del_backward(std::size_t n);
    std::size_t del_backward();
    std::size_t del_forward(std::size_t n);
    std::size_t del_forward();

    std::size_t new_cursor();
    void use_cursor(std::size_t c);
    std::size_t get_pos() const;
    bool goto_pos(long pos);

    std::size_t forward(std::size_t n);
    std::size_t backward(std::size_t n);
    std::size_t next_line(std::size_t n);
    std::size_t prev_line(std::size_t n);

    std::size_t forward();
    std::size_t backward();
    std::size_t next_line();
    std::size_t prev_line();

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

    // {{noexport
    int get_cursor_count() const { return cursors.size(); }
    // noexport}}

    // Methods for rust inter-op
    inline std::unique_ptr<text> get_contents_box() const {
        return std::make_unique<text>(get_contents());
    }
    inline std::unique_ptr<text> copy_box() const { return std::make_unique<text>(copy()); }
    inline std::unique_ptr<text> cut_box() { return std::make_unique<text>(cut()); }

    inline long find_text(const text &t) { return find(t); }
    inline long rfind_text(const text &t) { return rfind(t); }
    inline long find_fuzzy_text(const text &t) { return find_fuzzy(t); }
    inline void paste_text(const text &t) { paste(t); }

  private:
    text contents;
    immer::box<std::string> filename;
    immer::vector<cursor_t> cursors;
    std::size_t cursor{};

    void update_all_cursors(std::size_t mark, std::size_t point, std::size_t forward);
};

// Methods for rust inter-op
inline std::unique_ptr<buffer> new_buffer() { return std::make_unique<buffer>(); };
inline std::unique_ptr<buffer> open_buffer(const std::string &name) {
    return std::make_unique<buffer>(name);
};
std::unique_ptr<std::string> text_to_string(const text &t);
} // namespace mime

#endif /* MIME_HH */
