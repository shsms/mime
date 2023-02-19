#include <codecvt>
#include <fstream>
#include <immer/box.hpp>
#include <internal/u32utils.hh>
#include <iostream>
#include <iterator>
#include <locale>
#include <mime/mime.hh>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>

namespace mime {

const static int chunk_size = 1e6;
const long not_found = -1;

std::wstring read_file_chunk(std::ifstream &file, uint64_t max_chunk_size) {
    std::string chunk;
    chunk.resize(max_chunk_size);

    file.read(&chunk[0], chunk.size());
    if (file.gcount() < max_chunk_size) {
        chunk.resize(file.gcount());
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    return cvt.from_bytes(chunk);
}

buffer::buffer() { cursors = cursors.push_back(cursor_t{}); }

buffer::buffer(const std::string &fname, open_spec spec) {
    filename = fname;
    cursors = cursors.push_back(cursor_t{});

    if (fname.empty()) {
        SPDLOG_ERROR("Received empty filename. Nothing to open.");
        throw std::runtime_error("filename required");
    }
    std::ifstream fs(fname, std::ios::in | std::ios::binary);

    if (!fs.is_open() && spec == must_open) {
        SPDLOG_ERROR("Unable to open file '{0}'", fname);
        throw std::runtime_error("unable to open file");
    } else if (!fs.is_open()) {
        SPDLOG_INFO("Unable to open file '{0}'.  Creating an empty buffer instead.", fname);
        return;
    }
    auto vv = text{};

    while (!fs.eof()) {
        auto chunk = read_file_chunk(fs, chunk_size);
        text tt{chunk.begin(), chunk.end()};
        vv = vv + tt;
    }

    contents = vv;
}

buffer::buffer(const std::string &fname) : buffer(fname, try_open) {}

buffer::buffer(const text &fname) : buffer(to_string(fname), try_open) {}

void buffer::save() const { save_as(filename); }

void buffer::save_as(const std::string &fname) const {
    std::ofstream fs(fname, std::ios::trunc | std::ios::out | std::ios::binary);
    auto data = contents;
    auto max_size = chunk_size / 4;
    auto pending = data.size();
    while (pending > 0) {
        auto to_write = data.take(max_size);
        data = data.drop(max_size);
        auto chunk = to_string(to_write);

        fs.write(chunk.c_str(), chunk.length());
        if (fs.bad()) {
            throw std::runtime_error(std::string("unable to write to output file '") + fname + "'");
        }
        pending = data.size();
    }
}

void buffer::update_all_cursors(std::size_t mark, std::size_t point, std::size_t forward) {
    auto back = point - mark;
    for (std::size_t cid = 0; cid < cursors.size(); ++cid) {
        auto csr = cursors[cid];
        csr.mark.reset();

        // move the cursors as necessary.
        if (csr.point == point && cursor != cid && back == 0) {
            // a different cursor located at the same position
            // as the current cursor is pasting stuff.  don't move
            // the current cursor.
        } else if (csr.point >= point) {
            csr.point = csr.point - back + forward;
        } else if (csr.point > mark && back > 0) {
            csr.point = mark;
        }

        // move the narrowed regions as necessary.
        if (csr.view.has_value()) {
            if (csr.view->lower >= point && mark < point) {
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

        cursors = cursors.set(cid, csr);
    }
}

void buffer::set_mark() {
    auto c = cursors[cursor];
    c.mark = c.point;
    cursors = cursors.set(cursor, c);
}

long buffer::get_mark() {
    if (cursors[cursor].mark.has_value()) {
        return *cursors[cursor].mark;
    }
    return not_found;
}

template <typename T> long buffer::find(const T &t) {
    if (t.empty()) {
        return not_found;
    }

    auto match_iter = t.begin();
    auto c = cursors[cursor];
    auto offset = c.point;

    auto lim = contents.size();
    if (c.view.has_value()) { // narrowed view
        lim = c.view->upper;
    }

    long match_start = offset;
    for (auto it = contents.begin() + offset; it != contents.end() && offset < lim; ++it) {
        ++offset;
        if (*match_iter != *it) {
            match_iter = t.begin();
            match_start = offset;
            continue;
        }
        ++match_iter;
        if (match_iter == t.end()) {
            auto c = cursors[cursor];
            c.point = offset;
            cursors = cursors.set(cursor, c);
            return match_start;
        }
    }
    return not_found;
}

// we only need these two instanciations and one specialization below.
template long buffer::find(const text &t);
template long buffer::find(const std::wstring &t);
template <> long buffer::find(const std::string &t) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    std::wstring ustr = cvt.from_bytes(t);
    return find(ustr);
}

template <> long buffer::find(const regex_t &t) {
    std::match_results<text::iterator> m;
    if (t.empty) {
        return not_found;
    }
    auto c = cursors[cursor];
    auto offset = c.point;
    auto begin = contents.begin() + offset;
    auto end = contents.end();
    if (c.view.has_value()) { // narrowed view
        end = contents.begin() + c.view->upper;
    }

    bool found = std::regex_search(begin, end, m, t.rex.get());
    if (!found || m.empty()) {
        return not_found;
    }

    c.point = m[0].second - contents.begin();
    cursors = cursors.set(cursor, c);

    return m[0].first - contents.begin();
}

template <typename T> long buffer::rfind(const T &t) {
    if (t.empty()) {
        return not_found;
    }

    auto c = cursors[cursor];
    std::size_t offset = c.point - t.size();
    if (t.size() > c.point) {
        return not_found;
    }

    std::size_t lim{};
    if (c.view.has_value()) { // narrowed view
        lim = c.view->lower;
    }

    auto match_iter = t.begin();
    auto iter = contents.begin() + offset;

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
            auto c = cursors[cursor];
            c.point = offset;
            cursors = cursors.set(cursor, c);
            return iter - contents.begin();
        }
    }
    return not_found;
}

template long buffer::rfind(const text &t);
template long buffer::rfind(const std::wstring &t);
template <> long buffer::rfind(const std::string &t) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    std::wstring ustr = cvt.from_bytes(t);
    return rfind(ustr);
}

template <typename T> long buffer::find_fuzzy(const T &text) {
    auto t = text;
    u32::trim(t);
    if (t.empty()) {
        return not_found;
    }

    auto match_iter = t.begin();
    auto c = cursors[cursor];
    auto offset = c.point;
    auto lim = contents.size();
    std::size_t match_length{};
    long match_start = offset;

    if (c.view.has_value()) { // narrowed view
        lim = c.view->upper;
    }

    auto it = contents.begin() + offset;
    while (match_iter != t.end() && it != contents.end() && offset < lim) {
        if (u32::isspace(*match_iter) && u32::isspace(*it)) {
            while (u32::isspace(*match_iter) && match_iter != t.end()) {
                ++match_iter;
            }
            while (u32::isspace(*it) && it != contents.end() && offset < lim) {
                ++offset;
                ++it;
            }
        }
        if (u32::tolower(*match_iter) != u32::tolower(*it)) {
            match_iter = t.begin();
            if (match_length == 0) {
                ++offset;
                ++it;
            } else {
                offset -= (match_length - 1);
                it -= (match_length - 1);
                match_length = 0;
            }
            match_start = offset;
            continue;
        }
        ++match_length;
        ++offset;
        ++it;
        ++match_iter;
        if (match_iter == t.end()) {
            auto c = cursors[cursor];
            c.point = offset;
            cursors = cursors.set(cursor, c);
            return match_start;
        }
    }
    return not_found;
}
template long buffer::find_fuzzy(const text &t);
template long buffer::find_fuzzy(const std::wstring &t);
template <> long buffer::find_fuzzy(const std::string &t) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    std::wstring ustr = cvt.from_bytes(t);
    return find_fuzzy(ustr);
}

int buffer::replace(const std::string &from, const std::string &to, std::size_t n) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    std::wstring wfrom = cvt.from_bytes(from);
    if (wfrom.empty()) {
        return 0;
    }

    std::wstring wto = cvt.from_bytes(to);
    text text_to{wto.begin(), wto.end()};

    if (n <= 0) {
        n = std::numeric_limits<std::size_t>::max();
    }

    // TODO: add overload that takes text{} for all find methods
    int ii;
    for (ii = 0; ii < n; ++ii) {
        auto found = find(wfrom);
        if (found < 0) {
            return ii;
        }
        auto c = cursors[cursor];
        auto mark = c.point - wfrom.size();
        auto point = c.point;
        contents = contents.erase(mark, point);
        contents = contents.insert(mark, text_to);
        update_all_cursors(mark, point, wto.size());
    }
    return ii;
}

int buffer::replace(const std::string &from, const std::string &to) {
    return replace(from, to, 0); // replace all
}

text buffer::copy() const {
    auto c = cursors[cursor];
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

    return contents.drop(mark).take(point - mark);
}

text buffer::cut() {
    auto t = copy();
    if (t.empty()) {
        return t;
    }
    erase_region();
    return t;
}

void buffer::paste(const text &t) {
    auto point = cursors[cursor].point;

    contents = contents.insert(point, t);
    update_all_cursors(point, point, t.size());
}

void buffer::paste(const std::string &t) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    std::wstring wt = cvt.from_bytes(t);

    return paste(text{wt.begin(), wt.end()});
}

void buffer::erase_region() {
    auto c = cursors[cursor];

    if (!c.mark.has_value()) {
        return;
    }
    auto mark = *c.mark;
    auto point = c.point;

    if (mark > point) {
        std::swap(mark, point);
    }
    contents = contents.erase(mark, point);
    update_all_cursors(mark, point, 0);
}

void buffer::clear() {
    contents = text{};
    for (std::size_t cid = 0; cid < cursors.size(); ++cid) {
        auto csr = cursors[cid];
        csr.mark.reset();
        csr.point = 0;
        csr.view.reset();
        cursors = cursors.set(cid, csr);
    }
}

std::size_t buffer::new_cursor() {
    auto sz = cursors.size();
    cursors = cursors.push_back(cursor_t{});
    return sz;
}

void buffer::use_cursor(std::size_t c) {
    if (c > cursors.size()) {
        SPDLOG_ERROR("attempt to move cursor to outside file_range");
        throw std::out_of_range("attempt to move cursor to outside file_range");
    }
    cursor = c;
}

std::size_t buffer::get_pos() const { return cursors[cursor].point; }

bool buffer::goto_pos(long pos) {
    if (pos == -1) {
        return false;
    }
    auto c = cursors[cursor];
    std::size_t start = 0, end = contents.size();
    if (c.view.has_value()) { // narrowed view
        start = c.view->lower;
        end = c.view->upper;
    }

    if (pos < start || pos > end) {
        SPDLOG_ERROR("Attempt to 'goto_pos({0})', which is outside the {1}: {2}", pos,
                     c.view.has_value() ? "narrowed view" : "buffer", filename.get());
        return false;
    }
    c.point = pos;
    cursors = cursors.set(cursor, c);
    return true;
}

std::size_t buffer::forward(std::size_t n) {
    auto c = cursors[cursor];
    auto lim = contents.size();
    if (c.view.has_value()) { // narrowed view
        lim = c.view->upper;
    }
    std::size_t orig = c.point;
    c.point += n;
    if (c.point > lim) {
        c.point = lim;
    }
    cursors = cursors.set(cursor, c);

    return c.point - orig;
}

std::size_t buffer::backward(std::size_t n) {
    auto c = cursors[cursor];
    std::size_t lim{};
    if (c.view.has_value()) { // narrowed view
        lim = c.view->lower;
    }
    std::size_t orig = c.point;
    if (c.point > n + lim) {
        c.point -= n;
    } else {
        c.point = lim;
    }
    cursors = cursors.set(cursor, c);

    return orig - c.point;
}

std::size_t buffer::next_line(std::size_t n) {
    auto c = cursors[cursor];
    auto it = contents.begin() + c.point;
    auto end = contents.end();
    if (c.view.has_value()) {
        end = contents.begin() + c.view->upper;
    }

    std::size_t ii;
    for (ii = 0; ii < n; ii++) {
        while (it != end && *it != L'\n') {
            ++it;
        }
        if (*it != L'\n') {
            break;
        }
        if (it == end) {
            break;
        }
        ++it;
    }
    c.point = it - contents.begin();
    cursors = cursors.set(cursor, c);
    return ii;
}

std::size_t buffer::prev_line(std::size_t n) {
    backward(); // if at end of line,  that \n shouldn't be counted.
    auto c = cursors[cursor];
    auto it = contents.begin() + c.point;
    auto begin = contents.begin();
    if (c.view.has_value()) {
        begin = begin + c.view->lower;
    }
    std::size_t ii;
    for (ii = 0; ii < n; ii++) {
        while (it != begin && *it != L'\n') {
            --it;
        }
        if (*it != L'\n') {
            break;
        }
        while (it != begin && *(it - 1) != L'\n') {
            --it;
        }
    }
    c.point = it - contents.begin();
    cursors = cursors.set(cursor, c);
    return ii;
}

// TODO: make efficient implementation for del_backward and del_forward
std::size_t buffer::del_backward(std::size_t n) {
    auto orig_c = cursor;
    auto orig_pos = get_pos();
    cursor = new_cursor();
    cursors = cursors.set(cursor, cursors[orig_c]);
    goto_pos(orig_pos);
    set_mark();
    std::size_t dist;
    if (dist = backward(n); dist > 0) {
        erase_region();
    }
    cursors = cursors.take(cursor);
    cursor = orig_c;
    return dist;
}

std::size_t buffer::del_forward(std::size_t n) {
    auto orig_c = cursor;
    auto orig_pos = get_pos();
    cursor = new_cursor();
    cursors = cursors.set(cursor, cursors[orig_c]);
    goto_pos(orig_pos);
    set_mark();
    std::size_t dist;
    if (dist = forward(n); dist > 0) {
        erase_region();
    }
    cursors = cursors.take(cursor);
    cursor = orig_c;
    return dist;
}

std::size_t buffer::forward() { return forward(1); } // forward one wchar_t.
std::size_t buffer::backward() { return backward(1); }
std::size_t buffer::next_line() { return next_line(1); }
std::size_t buffer::prev_line() { return prev_line(1); }
std::size_t buffer::del_backward() { return del_backward(1); }
std::size_t buffer::del_forward() { return del_forward(1); }

void buffer::start_of_buffer() {
    auto c = cursors[cursor];
    c.point = 0;
    if (c.view.has_value()) { // narrowed view
        c.point = c.view->lower;
    }
    cursors = cursors.set(cursor, c);
}

void buffer::end_of_buffer() {
    auto c = cursors[cursor];
    c.point = contents.size();
    if (c.view.has_value()) { // narrowed view
        c.point = c.view->upper;
    }
    cursors = cursors.set(cursor, c);
}

void buffer::start_of_line() {
    auto c = cursors[cursor];
    auto begin = contents.begin();
    if (c.view.has_value()) {
        begin = begin + c.view->lower;
    }
    auto it = contents.begin() + c.point;
    while (it != begin && *(it - 1) != L'\n') {
        --it;
    }
    c.point = it - contents.begin();
    cursors = cursors.set(cursor, c);
}

void buffer::end_of_line() {
    auto c = cursors[cursor];
    auto end = contents.end();
    if (c.view.has_value()) {
        end = contents.begin() + c.view->upper;
    }
    auto it = contents.begin() + c.point;
    while (it != end && *it != L'\n') {
        ++it;
    }
    c.point = it - contents.begin();
    cursors = cursors.set(cursor, c);
}

bool buffer::start_of_block() {
    auto c = cursors[cursor];
    auto begin = contents.begin();
    if (c.view.has_value()) {
        begin = begin + c.view->lower;
    }
    int depth{};
    auto it = contents.begin() + c.point;
    while (it != begin) {
        bool found_start{false};
        --it;
        switch (*it) {
        case L'}':
            ++depth;
            break;
        case L'{':
            if (depth > 0) {
                --depth;
            } else {
                found_start = true;
            }
            break;
        }
        if (found_start) {
            break;
        }
    }
    if (*it != L'{') {
        return false;
    }
    c.point = it - contents.begin();
    cursors = cursors.set(cursor, c);
    return true;
}

bool buffer::end_of_block() {
    auto curr_pos = get_pos();
    if (!start_of_block()) {
        return false;
    }
    goto_pos(curr_pos);
    auto c = cursors[cursor];
    auto end = contents.end();
    if (c.view.has_value()) {
        end = contents.begin() + c.view->upper;
    }
    int depth{};
    auto it = contents.begin() + c.point;
    while (it != end) {
        bool found_end{false};
        switch (*it) {
        case L'{':
            ++depth;
            break;
        case L'}':
            if (depth > 0) {
                --depth;
            } else {
                found_end = true;
            }
            break;
        }
        ++it;
        if (found_end) {
            break;
        }
    }
    if (*(it - 1) != L'}') {
        return false;
    }
    c.point = it - contents.begin();
    cursors = cursors.set(cursor, c);
    return true;
}

bool buffer::narrow_to_block() {
    auto curr_pos = get_pos();
    if (!start_of_block()) {
        return false;
    }
    set_mark();
    goto_pos(curr_pos);
    if (!end_of_block()) {
        return false;
    }
    return narrow_to_region();
}

bool buffer::narrow_to_region() {
    if (!cursors[cursor].mark.has_value()) {
        // no mark => no region.
        return false;
    }
    auto c = cursors[cursor];
    auto mark = *c.mark;
    auto point = c.point;
    if (mark > point) {
        std::swap(mark, point);
    }
    c.view = bounds{mark, point};
    c.mark.reset();
    c.point = mark; // reset point to beginning of region.
    cursors = cursors.set(cursor, c);
    return true;
}

void buffer::widen() {
    auto c = cursors[cursor];
    c.view.reset();
    cursors = cursors.set(cursor, c);
}

std::string to_string(const text &t) {
    std::wstring wret{t.begin(), t.end()};
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    return cvt.to_bytes(wret);
}

regex_t regex(const std::string &r) {
    if (r.empty()) {
        return {.empty = true};
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
    return {std::wregex(cvt.from_bytes(r)), false};
}
} // namespace mime
