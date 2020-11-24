#include "buffer.hh"
#include "u32utils.hh"
#include <codecvt>
#include <fstream>
#include <immer/box.hpp>
#include <iostream>
#include <iterator>
#include <locale>
#include <spdlog/spdlog.h>
#include <string>

namespace mime {

const static int chunk_size = 1e6;

std::u32string read_file_chunk(std::ifstream &file, uint64_t max_chunk_size) {
    std::string chunk;
    chunk.resize(max_chunk_size);

    file.read(&chunk[0], chunk.size());
    if (file.gcount() < max_chunk_size) {
        chunk.resize(file.gcount());
    }
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    return cvt.from_bytes(chunk);
}

buffer update_all_cursors(buffer b, std::size_t mark, std::size_t point, std::size_t forward) {
    auto back = point - mark;
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
        auto csr = b.cursors[cid];
        csr.mark.reset();
        if (csr.point >= point) {
            csr.point = csr.point - back + forward;
        } else if (csr.point > mark && back > 0) {
            csr.point = mark;
        }

        if (csr.view.has_value()) {
            if (csr.view->lower >= point) {
                csr.view->lower = csr.view->lower - back + forward;
                csr.view->upper = csr.view->upper - back + forward;
            } else if (csr.view->lower >= mark && csr.view->upper >= point) {
                csr.view->lower = mark;
                csr.view->upper = csr.view->upper - back + forward;
            } else if (csr.view->lower < mark && csr.view->upper >= point) {
                csr.view->upper = csr.view->upper - back + forward;
            } else if (csr.view->lower >= mark && csr.view->upper > mark) {
                csr.view->lower = mark;
                csr.view->upper = mark;
            } else if (csr.view->lower < mark && csr.view->upper > mark) {
                csr.view->upper = mark;
            }
        }

        b.cursors = b.cursors.set(cid, csr);
    }
    return b;
}

buffer_bool open(std::string name) {
    if (name.empty()) {
        SPDLOG_INFO("Received empty filename. Creating a new buffer.");
        buffer b{};
        b.cursors = b.cursors.push_back(cursor{});
        return buffer_bool{b, true}; // new anonymous buffer
    }
    auto file = std::ifstream(name, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        SPDLOG_INFO("File '{0}' doesn't exist. Creating a new buffer with that name instead.",
                    name);
        buffer b{.file_name = name};
        b.cursors = b.cursors.push_back(cursor{});
        return buffer_bool{b, true}; // new file = true
    }
    auto vv = text{};

    while (!file.eof()) {
        auto chunk = read_file_chunk(file, chunk_size);
        text tt{chunk.begin(), chunk.end()};
        vv = vv + tt;
    }

    buffer b{
        .contents = vv,
        .file_name = name,
    };
    b.cursors = b.cursors.push_back(cursor{});

    return buffer_bool{b, false};
}

void save(buffer b) { save_as(b, b.file_name); }

void save_as(buffer b, std::string name) {
    std::ofstream fs;
    fs.open(name, std::ios::trunc | std::ios::out | std::ios::binary);

    auto max_size = chunk_size / 4;
    auto pending = b.contents.size();
    while (pending > 0) {
        auto to_write = b.contents.take(max_size);
        b.contents = b.contents.drop(max_size);
        auto chunk = to_string(to_write);

        fs.write(chunk.c_str(), chunk.length());
        if (fs.bad()) {
            throw std::runtime_error(std::string("unable to write to output file '") + name + "'");
        }
        pending = b.contents.size();
    }
}

template <typename T> buffer_bool find(buffer b, std::size_t cursor, T t) {
    if (t.empty()) {
        return buffer_bool{b, false};
    }

    auto match_iter = t.begin();
    auto c = b.cursors[cursor];
    auto offset = c.point;

    auto lim = b.contents.size();
    if (c.view.has_value()) { // narrowed view
        lim = c.view->upper;
    }

    for (auto it = b.contents.begin() + offset; it != b.contents.end() && offset < lim; ++it) {
        ++offset;
        if (*match_iter != *it) {
            match_iter = t.begin();
            continue;
        }
        ++match_iter;
        if (match_iter == t.end()) {
            auto c = b.cursors[cursor];
            c.point = offset;
            b.cursors = b.cursors.set(cursor, c);
            return buffer_bool{b, true};
        }
    }
    return buffer_bool{b, false};
}
// we only need these two instanciations and one specialization below.
template buffer_bool find(buffer b, std::size_t cursor, text t);
template buffer_bool find(buffer b, std::size_t cursor, std::u32string t);
template <> buffer_bool find(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string ustr = cvt.from_bytes(t);
    return find(b, cursor, ustr);
}

template <typename T> buffer_bool rfind(buffer b, std::size_t cursor, T t) {
    if (t.empty()) {
        return buffer_bool{b, false};
    }

    auto c = b.cursors[cursor];
    std::size_t offset = c.point - t.size();
    if (offset < 0) {
        return buffer_bool{b, false};
    }

    std::size_t lim{};
    if (c.view.has_value()) { // narrowed view
        lim = c.view->lower;
    }

    auto match_iter = t.begin();
    auto iter = b.contents.begin() + offset;
    while (offset >= lim) {
        if (*match_iter != *iter) {
            if (offset == lim) {
                break;
            }
            iter -= match_iter - t.begin();
            --iter;
            --offset;
            match_iter = t.begin();
            continue;
        }
        ++match_iter;
        ++iter;
        if (match_iter == t.end()) {
            auto c = b.cursors[cursor];
            c.point = offset;
            b.cursors = b.cursors.set(cursor, c);
            return buffer_bool{b, true};
        }
    }
    return buffer_bool{b, false};
}

template buffer_bool rfind(buffer b, std::size_t cursor, text t);
template buffer_bool rfind(buffer b, std::size_t cursor, std::u32string t);
template <> buffer_bool rfind(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string ustr = cvt.from_bytes(t);
    return rfind(b, cursor, ustr);
}

template <typename T> buffer_bool find_fuzzy(buffer b, std::size_t cursor, T t) {
    u32::trim(t);
    if (t.empty()) {
        return buffer_bool{b, false};
    }

    auto match_iter = t.begin();
    auto c = b.cursors[cursor];
    auto offset = c.point;

    auto lim = b.contents.size();
    if (c.view.has_value()) { // narrowed view
        lim = c.view->upper;
    }

    auto it = b.contents.begin() + offset;
    while (match_iter != t.end() && it != b.contents.end() && offset < lim) {
        if (u32::isspace(*match_iter) && u32::isspace(*it)) {
            while (u32::isspace(*match_iter) && match_iter != t.end()) {
                ++match_iter;
            }
            while (u32::isspace(*it) && it != b.contents.end() && offset < lim) {
                ++offset;
                ++it;
            }
        }
        if (u32::tolower(*match_iter) != u32::tolower(*it)) {
            match_iter = t.begin();
            ++offset;
            ++it;
            continue;
        }
        ++offset;
        ++it;
        ++match_iter;
        if (match_iter == t.end()) {
            auto c = b.cursors[cursor];
            c.point = offset;
            b.cursors = b.cursors.set(cursor, c);
            return buffer_bool{b, true};
        }
    }
    return buffer_bool{b, false};
}
template buffer_bool find_fuzzy(buffer b, std::size_t cursor, text t);
template buffer_bool find_fuzzy(buffer b, std::size_t cursor, std::u32string t);
template <> buffer_bool find_fuzzy(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string ustr = cvt.from_bytes(t);
    return find_fuzzy(b, cursor, ustr);
}

buffer_int replace(buffer b, std::size_t cursor, std::string from, std::string to, std::size_t n) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wfrom = cvt.from_bytes(from);
    if (wfrom.empty()) {
        return buffer_int{b, 0};
    }

    std::u32string wto = cvt.from_bytes(to);
    text text_to{wto.begin(), wto.end()};

    if (n <= 0) {
        n = std::numeric_limits<std::size_t>::max();
    }

    // TODO: add overload that takes text{} for all find methods
    int ii;
    for (ii = 0; ii < n; ++ii) {
        auto found = find(b, cursor, wfrom);
        if (!found.get_bool()) {
            return buffer_int{found.b, ii};
        }
        b = found.b;
        auto c = b.cursors[cursor];
        auto mark = c.point - wfrom.size();
        auto point = c.point;
        b.contents = b.contents.erase(mark, point);
        b.contents = b.contents.insert(mark, text_to);
        b = update_all_cursors(b, mark, point, wto.size());
    }
    return buffer_int{b, ii};
}

buffer set_mark(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.mark = c.point;
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

std::string to_string(text t) {
    std::u32string wret{t.begin(), t.end()};
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    return cvt.to_bytes(wret);
}

text copy(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    if (!c.mark.has_value()) {
        return text{};
    }

    auto mark = *c.mark;
    auto point = c.point;
    if (mark == point) {
        return text{};
    }
    if (mark > point) {
        std::swap(mark, point);
    }

    return b.contents.drop(mark).take(point - mark);
}

buffer_text cut(buffer b, std::size_t cursor) {
    auto t = copy(b, cursor);
    if (t.empty()) {
        return buffer_text{b, t};
    }
    b = erase_region(b, cursor);
    return buffer_text{b, t};
}

buffer paste(buffer b, std::size_t cursor, text t) {
    auto c = b.cursors[cursor];
    auto point = c.point;

    b.contents = b.contents.insert(c.point, t);
    b = update_all_cursors(b, point, point, t.size());
    return b;
}

buffer paste(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wt = cvt.from_bytes(t);

    return paste(b, cursor, text{wt.begin(), wt.end()});
}

buffer erase_region(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];

    if (!c.mark.has_value()) {
        return b;
    }
    auto mark = *c.mark;
    auto point = c.point;

    if (mark > point) {
        std::swap(mark, point);
    }
    b.contents = b.contents.erase(mark, point);
    b = update_all_cursors(b, mark, point, 0);
    return b;
}

buffer clear(buffer b) {
    b.contents = text{};
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
        auto csr = b.cursors[cid];
        csr.mark.reset();
        csr.point = 0;
        csr.view.reset();
        b.cursors = b.cursors.set(cid, csr);
    }
    return b;
}

buffer_int new_cursor(buffer b) {
    int sz = b.cursors.size();
    b.cursors = b.cursors.push_back(cursor{});
    return buffer_int{b, sz};
}

std::size_t get_pos(buffer b, std::size_t cursor) { return b.cursors[cursor].point; }

buffer_bool goto_pos(buffer b, std::size_t cursor, std::size_t pos) {
    auto c = b.cursors[cursor];
    std::size_t start = 0, end = b.contents.size();
    if (c.view.has_value()) { // narrowed view
        start = c.view->lower;
        end = c.view->upper;
    }

    if (pos < start || pos > end) {
        SPDLOG_ERROR("Attempt to 'goto_pos({0})', which is outside the {1}: {2}", pos,
                     c.view.has_value() ? "narrowed view" : "buffer", b.file_name.get());
        return buffer_bool{b, false};
    }
    c.point = pos;
    b.cursors = b.cursors.set(cursor, c);
    return buffer_bool{b, true};
}

buffer forward(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    auto lim = b.contents.size();
    if (c.view.has_value()) { // narrowed view
        lim = c.view->upper;
    }
    c.point += n;
    if (c.point > lim) {
        c.point = lim;
    }
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer backward(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    std::size_t lim{};
    if (c.view.has_value()) { // narrowed view
        lim = c.view->lower;
    }
    if (c.point > n + lim) {
        c.point -= n;
    } else {
        c.point = lim;
    }
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer_int next_line(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    auto it = b.contents.begin() + c.point;
    auto end = b.contents.end();
    if (c.view.has_value()) {
        end = b.contents.begin() + c.view->upper;
    }

    int ii;
    for (ii = 0; ii < n; ii++) {
        while (it != end && *it != U'\n') {
            ++it;
        }
        if (*it != U'\n') {
            break;
        }
        // go 1 char past the new line.
        if (it != end) {
            ++it;
        }
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return buffer_int{b, ii};
}

buffer_int prev_line(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    auto it = b.contents.begin() + c.point;
    auto begin = b.contents.begin();
    if (c.view.has_value()) {
        begin = begin + c.view->lower;
    }
    int ii;
    for (ii = 0; ii < n; ii++) {
        while (it != begin && *it != U'\n') {
            --it;
        }
        if (*it != U'\n') {
            break;
        }
        while (it != begin && *(it - 1) != U'\n') {
            --it;
        }
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return buffer_int{b, ii};
}

buffer start_of_buffer(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.point = 0;
    if (c.view.has_value()) { // narrowed view
        c.point = c.view->lower;
    }
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer end_of_buffer(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.point = b.contents.size();
    if (c.view.has_value()) { // narrowed view
        c.point = c.view->upper;
    }
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer start_of_line(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    auto begin = b.contents.begin();
    if (c.view.has_value()) {
        begin = begin + c.view->lower;
    }
    auto it = b.contents.begin() + c.point;
    while (it != begin && *(it - 1) != U'\n') {
        --it;
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer end_of_line(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    auto end = b.contents.end();
    if (c.view.has_value()) {
        end = b.contents.begin() + c.view->upper;
    }
    auto it = b.contents.begin() + c.point;
    while (it != end && *it != U'\n') {
        ++it;
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer_bool narrow_to_region(buffer b, std::size_t cursor) {
    if (!b.cursors[cursor].mark.has_value()) {
        // no mark => no region.
        return buffer_bool{b, false};
    }
    auto c = b.cursors[cursor];
    auto mark = *c.mark;
    auto point = c.point;
    if (mark > point) {
        std::swap(mark, point);
    }
    c.view = bounds{mark, point};
    c.mark.reset();
    c.point = mark; // reset point to beginning of region.
    b.cursors = b.cursors.set(cursor, c);
    return buffer_bool{b, true};
}

buffer widen(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.view.reset();
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

} // namespace mime
