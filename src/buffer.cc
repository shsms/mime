#include "buffer.hh"
#include <codecvt>
#include <fstream>
#include <immer/box.hpp>
#include <iostream>
#include <iterator>
#include <locale>
#include <string>

namespace meme {

const static int chunk_size = 1e6;

std::wstring read_file_chunk(std::ifstream &file, uint64_t max_chunk_size) {
    std::string chunk;
    chunk.resize(max_chunk_size);

    file.read(&chunk[0], chunk.size());
    if (file.gcount() < max_chunk_size) {
        chunk.resize(file.gcount());
    }
    if (!file.eof()) {
        std::string line;
        std::getline(file, line);
        if (line.length() > 0) {
            chunk += line;
        }
    }
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(chunk);
}

buffer open_file(std::string name) {
    auto file = std::ifstream(name, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
	throw std::runtime_error(std::string("unable to open file '") + name + "' for reading.  Does it exist and have the right permissions?");
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

    return b;
}

buffer_bool find(buffer b, std::size_t cursor, std::string text, std::size_t lim) {
    // TODO: validate cursor, check input size

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);

    int matching_char_pos = 0;
    auto offset = b.cursors[cursor].point;

    if (lim <= 0) {
	lim = std::numeric_limits<std::size_t>::max();
    }

    for (auto it = b.contents.begin() + offset; it != b.contents.end() && offset < lim; ++it) {
        ++offset;
        if (wtext[matching_char_pos] != *it) {
            matching_char_pos = 0;
            continue;
        }
        ++matching_char_pos;
        if (matching_char_pos == wtext.length()) {
            auto c = b.cursors[cursor];
            c.point = offset;
            b.cursors = b.cursors.set(cursor, c);
            return buffer_bool{b, true};
        }
    }
    return buffer_bool{b, false};
}

buffer_bool rfind(buffer b, std::size_t cursor, std::string text, std::size_t lim) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);

    // TODO: add debug log for 0 input size
    std::size_t offset = b.cursors[cursor].point - text.length();
    if (offset < 0) {
        return buffer_bool{b, false};
    }

    if (lim > 0) {
	lim = offset - lim;
    }
    if (lim < 0) {
	lim = 0;
    }

    int matching_char_pos = 0;
    auto iter = b.contents.begin() + offset;
    while (offset >= lim) {
	if(wtext[matching_char_pos] != *iter) {
	    if (offset == 0) break;
	    iter -= matching_char_pos;
	    --iter;
	    --offset;
	    matching_char_pos = 0;
	    continue;
	}
	++matching_char_pos;
	++iter;
	if (matching_char_pos == wtext.length()) {
	    auto c = b.cursors[cursor];
	    c.point = offset;
	    b.cursors = b.cursors.set(cursor, c);
	    return buffer_bool{b, true};
	}
    }
    return buffer_bool{b, false};
}

buffer set_mark(buffer b, std::size_t cursor) {
    // TODO: validate cursor
    auto c = b.cursors[cursor];
    c.mark = c.point;
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

std::string get_string(text t) {
    std::wstring wret{t.begin(), t.end()};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wret);
}

text copy(buffer b, std::size_t cursor) {
    // TODO: validate cursor
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

    // auto begin_iter = b.contents.begin() + mark;
    // auto end_iter = b.contents.begin() + point;
    // return text{begin_iter, end_iter};

    return b.contents.drop(mark).take(point - mark);
}

buffer_text cut(buffer b, std::size_t cursor) {
    auto t = copy(b, cursor);
    if (t == text{}) {
        return buffer_text{b, t};
    }
    auto c = b.cursors[cursor];

    auto mark = c.mark.value();
    auto point = c.point;

    if (mark > point) {
        std::swap(mark, point);
    }
    b.contents = b.contents.erase(mark, point);

    c.mark = mark;
    c.point = mark;
    b.cursors = b.cursors.set(cursor, c);
    // TODO: update all cursors
    
    return buffer_text{b, t};
}

buffer paste(buffer b, std::size_t cursor, text t) {
    auto c = b.cursors[cursor];
    b.contents = b.contents.insert(c.point, t);
    c.point += t.size();
    b.cursors = b.cursors.set(cursor, c);
    // TODO: update all cursors
    return b;
}

buffer insert(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wt = converter.from_bytes(t);

    return paste(b, cursor, text{wt.begin(), wt.end()});
}
} // namespace meme
