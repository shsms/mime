#include "buffer.hh"
#include <immer/box.hpp>
#include <locale>
#include <codecvt>
#include <string>
#include <fstream>
#include <iostream>
#include <iterator>

namespace meme {

const static int chunk_size = 1e6;

immer::box<std::wstring> read_file_chunk(std::wfstream &file, uint64_t max_chunk_size) {
    std::wstring chunk;
    chunk.resize(max_chunk_size);

    file.read(&chunk[0], chunk.size());
    if (file.gcount() < max_chunk_size) {
        chunk.resize(file.gcount());
    }
    if (!file.eof()) {
        std::wstring line;
        std::getline(file, line);
        if (line.length() > 0) {
            chunk += line;
        }
    }
    return chunk;
}

buffer open_file(std::string name) {
    auto file = std::wfstream(name, std::ios::in | std::ios::binary);
    auto vv = text{};

    while (!file.eof()) {
	auto chunk = read_file_chunk(file, chunk_size);
	text tt{chunk->begin(), chunk->end()};
	vv = vv + tt;
    }

    buffer b{
	.contents = vv,
	.file_name = name,
    };
    b.cursors = b.cursors.set("default", cursor{});

    return b;
}

buffer_bool search(buffer b, std::string cursor_name, std::string text) {
    auto  start_pos = b.cursors.at(cursor_name).point;
    auto start_iter = b.contents.begin() + start_pos;
    // TODO: validate cursor, check input size

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);
 
    std::size_t offset = 0;
    int matching_char_pos = 0;
    for (auto it = start_iter; it != b.contents.end(); ++it) {
	++offset;
	if (wtext[matching_char_pos] == *it) {
	    ++matching_char_pos;
	    if (matching_char_pos == text.length()) {
		auto c =b.cursors[cursor_name];
		c.point += offset;
		b.cursors = b.cursors.set(cursor_name, c);
		return buffer_bool{b, true};
	    }
	    continue;
	}
	matching_char_pos = 0;
    }
    return buffer_bool{b, false};
}

buffer set_mark(buffer b, std::string cursor_name) {
    // TODO: validate cursor
    auto c = b.cursors[cursor_name];
    c.mark = c.point;
    b.cursors = b.cursors.set(cursor_name, c);
    return b;
}

std::string get_string(text t) {
    std::wstring wret{t.begin(), t.end()};
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.to_bytes(wret);
}

text copy(buffer b, std::string cursor_name) {
    // TODO: validate cursor
    auto c = b.cursors[cursor_name];
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

buffer_text cut(buffer b, std::string cursor_name) {
    auto t = copy(b, cursor_name);
    if (t == text{}) {
	return buffer_text{b, t};
    }
    auto c = b.cursors[cursor_name];
    
    b.contents = b.contents.erase(c.mark.value(), c.point);
    return buffer_text{b, t};
}
}
