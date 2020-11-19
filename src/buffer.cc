#include "buffer.hh"
#include "u32utils.hh"
#include <codecvt>
#include <fstream>
#include <immer/box.hpp>
#include <iostream>
#include <iterator>
#include <locale>
#include <string>
#include <spdlog/spdlog.h>

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

buffer_bool open(std::string name) {
    if (name.empty()) {
	SPDLOG_INFO("Received empty filename. Creating a new buffer.");
	buffer b{};
	b.cursors = b.cursors.push_back(cursor{});	
        return buffer_bool{b, true}; // new anonymous buffer
    }
    auto file = std::ifstream(name, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
	SPDLOG_INFO("File '{0}' doesn't exist. Creating a new buffer with that name instead.", name);
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

template <typename T> buffer_bool find(buffer b, std::size_t cursor, T t, std::size_t lim) {
    if (t.empty()) {
        return buffer_bool{b, false};
    }

    auto match_iter = t.begin();
    auto offset = b.cursors[cursor].point;

    if (lim <= 0) {
        lim = std::numeric_limits<std::size_t>::max();
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
template buffer_bool find(buffer b, std::size_t cursor, text t, std::size_t lim);
template buffer_bool find(buffer b, std::size_t cursor, std::u32string t, std::size_t lim);
template <> buffer_bool find(buffer b, std::size_t cursor, std::string t, std::size_t lim) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string ustr = cvt.from_bytes(t);
    return find(b, cursor, ustr, lim);
}

template <typename T> buffer_bool rfind(buffer b, std::size_t cursor, T t, std::size_t lim) {
    if (t.empty()) {
        return buffer_bool{b, false};
    }

    std::size_t offset = b.cursors[cursor].point - t.size();
    if (offset < 0) {
        return buffer_bool{b, false};
    }

    if (lim > 0) {
        lim = offset - lim;
    }
    if (lim < 0) {
        lim = 0;
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

template buffer_bool rfind(buffer b, std::size_t cursor, text t, std::size_t lim);
template buffer_bool rfind(buffer b, std::size_t cursor, std::u32string t, std::size_t lim);
template <> buffer_bool rfind(buffer b, std::size_t cursor, std::string t, std::size_t lim) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string ustr = cvt.from_bytes(t);
    return rfind(b, cursor, ustr, lim);
}

template <typename T> buffer_bool find_fuzzy(buffer b, std::size_t cursor, T t, std::size_t lim) {
    u32::trim(t);
    if (t.empty()) {
        return buffer_bool{b, false};
    }

    auto match_iter = t.begin();
    auto offset = b.cursors[cursor].point;

    if (lim <= 0) {
        lim = std::numeric_limits<std::size_t>::max();
    } else {
        lim += offset;
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
template buffer_bool find_fuzzy(buffer b, std::size_t cursor, text t, std::size_t lim);
template buffer_bool find_fuzzy(buffer b, std::size_t cursor, std::u32string t, std::size_t lim);
template <> buffer_bool find_fuzzy(buffer b, std::size_t cursor, std::string t, std::size_t lim) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string ustr = cvt.from_bytes(t);
    return find_fuzzy(b, cursor, ustr, lim);
}

// TODO: maybe replace with buffer_int, same for navigation.
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
        auto found = find(b, cursor, wfrom, 0);
        if (!found.get_bool()) {
            return buffer_int{found.b, ii};
        }
        auto c = found.b.cursors[cursor];
        auto mark = c.point - wfrom.size();
        auto point = c.point;
        b.contents = found.b.contents.erase(mark, point);
        b.contents = b.contents.insert(mark, text_to);

        // update all cursors
        for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
            auto csr = b.cursors[cid];
            csr.mark.reset();
            if (csr.point >= point) {
                csr.point = csr.point - wfrom.size() + wto.size();
            } else if (csr.point > mark) {
                csr.point = mark;
            }
            b.cursors = b.cursors.set(cid, csr);
        }
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

    auto mark = c.mark.value();
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
    // update all cursors
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
        auto csr = b.cursors[cid];
        csr.mark.reset();
        if (csr.point >= point) {
            csr.point += t.size();
        }
        b.cursors = b.cursors.set(cid, csr);
    }
    return b;
}

buffer paste(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wt = cvt.from_bytes(t);

    return paste(b, cursor, text{wt.begin(), wt.end()});
}

buffer erase_region(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];

    auto mark = c.mark.value();
    auto point = c.point;

    if (mark > point) {
        std::swap(mark, point);
    }
    b.contents = b.contents.erase(mark, point);

    // update all cursors
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
        auto csr = b.cursors[cid];
        csr.mark.reset();
        if (csr.point >= point) {
            csr.point = csr.point - (point - mark);
        } else if (csr.point > mark) {
            csr.point = mark;
        }
        b.cursors = b.cursors.set(cid, csr);
    }
    return b;
}

buffer clear(buffer b) {
    b.contents = text{};
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
        auto csr = b.cursors[cid];
        csr.mark.reset();
        csr.point = 0;
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
    if (pos < 0 || pos >= b.contents.size()) {
        return buffer_bool{b, false};
    }
    auto c = b.cursors[cursor];
    c.point = pos;
    b.cursors = b.cursors.set(cursor, c);
    return buffer_bool{b, true};
}

buffer forward(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    c.point += n;
    if (c.point > b.contents.size()) {
        c.point = b.contents.size();
    }
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer backward(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    c.point -= n;
    if (c.point < 0) {
        c.point = 0;
    }
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer_bool next_line(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    auto it = b.contents.begin() + c.point;
    auto success = true;
    for (auto ii = 0; ii < n; ii++) {
        while (it != b.contents.end() && *it != U'\n') {
            ++it;
        }
	if (*it != U'\n') {
	    success = false;
	    break;
	}
        if (it != b.contents.end()) {
            ++it;
        }
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return buffer_bool{b, success};
}

buffer_bool prev_line(buffer b, std::size_t cursor, std::size_t n) {
    auto c = b.cursors[cursor];
    auto it = b.contents.begin() + c.point;
    auto success = true;
    for (auto ii = 0; ii < n; ii++) {
        while (it != b.contents.begin() && *it != U'\n') {
            --it;
        }
	if (*it != U'\n') {
	    success = false;
	    break;
	}
        while (it != b.contents.begin() && *(it - 1) != U'\n') {
            --it;
        }
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return buffer_bool{b, success};
}

buffer start_of_buffer(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.point = 0;
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer end_of_buffer(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.point = b.contents.size();
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer start_of_line(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    auto it = b.contents.begin() + c.point;
    while (it != b.contents.begin() && *(it - 1) != U'\n') {
        --it;
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

buffer end_of_line(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    auto it = b.contents.begin() + c.point;
    while (it != b.contents.end() && *it != U'\n') {
        ++it;
    }
    c.point = it - b.contents.begin();
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

} // namespace mime
