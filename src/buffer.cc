#include "buffer.hh"
#include "u32utils.hh"
#include <codecvt>
#include <fstream>
#include <immer/box.hpp>
#include <iostream>
#include <iterator>
#include <locale>
#include <string>

namespace meme {

const static int chunk_size = 1e6;

std::u32string read_file_chunk(std::ifstream &file, uint64_t max_chunk_size) {
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
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    return cvt.from_bytes(chunk);
}

buffer open(std::string name) {
    auto file = std::ifstream(name, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::string("unable to open file '") + name +
                                 "' for reading.  Does it exist and have the right permissions?");
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

void save(buffer b) { save_as(b, b.file_name); }

void save_as(buffer b, std::string name) {
    std::ofstream fs;
    fs.open(name, std::ios::trunc | std::ios::out | std::ios::binary);

    auto max_size = chunk_size / 4;
    auto pending = b.contents.size();
    while (pending > 0) {
        auto to_write = b.contents.take(max_size);
        b.contents = b.contents.drop(max_size);
        auto chunk = get_string(to_write);

        fs.write(chunk.c_str(), chunk.length());
        if (fs.bad()) {
            throw std::runtime_error(std::string("unable to write to output file '") + name + "'");
        }
        pending = b.contents.size();
    }
}

buffer_bool find(buffer b, std::size_t cursor, std::string text, std::size_t lim) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wtext = cvt.from_bytes(text);
    if (wtext.length() == 0) {
	return buffer_bool{b, false};
    }

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
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wtext = cvt.from_bytes(text);
    if (wtext.length() == 0) {
	return buffer_bool{b, false};
    }

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
        if (wtext[matching_char_pos] != *iter) {
            if (offset == 0)
                break;
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

buffer_bool find_fuzzy(buffer b, std::size_t cursor, std::string text, std::size_t lim) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wtext = cvt.from_bytes(text);
    u32::trim(wtext);
    if (wtext.length() == 0) {
	return buffer_bool{b, false};
    }

    int matching_char_pos = 0;
    auto offset = b.cursors[cursor].point;

    if (lim <= 0) {
        lim = std::numeric_limits<std::size_t>::max();
    } else {
	lim += offset;
    }
    auto it = b.contents.begin() + offset;
    while (it != b.contents.end() && offset < lim && matching_char_pos < wtext.length()) {
        if (u32::isspace(wtext[matching_char_pos]) && u32::isspace(*it)) {
            while (u32::isspace(wtext[matching_char_pos]) && matching_char_pos < wtext.length()) {
                ++matching_char_pos;
            }
            while (u32::isspace(*it) && it != b.contents.end() && offset < lim) {
                ++offset;
                ++it;
            }
        }
        if (u32::tolower(wtext[matching_char_pos]) != u32::tolower(*it)) {
            // get next steps based on strategy
            matching_char_pos = 0;
            ++offset;
            ++it;
            continue;
        }
        ++offset;
        ++it;
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

// TODO: maybe replace with buffer_int, same for navigation.
buffer_bool replace(buffer b, std::size_t cursor, std::string from, std::string to,
                         std::size_t n) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wfrom = cvt.from_bytes(from);
    if (wfrom.length() == 0) {
	return buffer_bool{b, false};
    }

    std::u32string wto = cvt.from_bytes(to);
    text text_to{wto.begin(), wto.end()};

    if (n <= 0) {
        n = std::numeric_limits<std::size_t>::max();
    }

    // TODO: add overload that takes text{} for all find methods
    for (std::size_t ii = 0; ii < n; ++ii) {
        auto found = find(b, cursor, from, 0);
        if (!found.get_bool()) {
            if (ii > 0) {
                found.success = true;
            }
            return found;
        }
        auto c = found.b.cursors[cursor];
	auto mark = c.point - wfrom.length();
	auto point = c.point;
        b.contents = found.b.contents.erase(mark, point);
        b.contents = b.contents.insert(mark, text_to);
	
	// update all cursors
	for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
	    auto csr = b.cursors[cid];
	    // csr.mark.reset();
	    // if (c.mark.has_value() && *c.mark >= point) {
	    // 	c.mark = *c.mark - wfrom.length() + wto.length();
	    // } else if (c.mark.has_value() && *c.mark > mark) {
	    // 	c.mark = mark;
	    // }
	    if (csr.point >= point) {
		csr.point = csr.point - wfrom.length() + wto.length();
	    } else if (csr.point > mark) {
		csr.point = mark;
	    }
	    b.cursors = b.cursors.set(cid, csr);
	}
    }
    return buffer_bool{b, true};
}

buffer set_mark(buffer b, std::size_t cursor) {
    auto c = b.cursors[cursor];
    c.mark = c.point;
    b.cursors = b.cursors.set(cursor, c);
    return b;
}

std::string get_string(text t) {
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

    // update all cursors
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
	auto csr = b.cursors[cid];
	// csr.mark.reset();
	if (csr.point >= point) {
	    csr.point = csr.point - (point - mark);
	} else if (csr.point > mark) {
	    csr.point = mark;
	}
	b.cursors = b.cursors.set(cid, csr);
    }

    return buffer_text{b, t};
}

buffer paste(buffer b, std::size_t cursor, text t) {
    auto c = b.cursors[cursor];
    auto point = c.point;
    
    b.contents = b.contents.insert(c.point, t);
    // update all cursors
    for (std::size_t cid = 0; cid < b.cursors.size(); ++cid) {
	auto csr = b.cursors[cid];
	// csr.mark.reset();
	if(csr.point >= point) {
	    csr.point += t.size();
	}
	b.cursors = b.cursors.set(cid, csr);
    }
    return b;
}

buffer insert(buffer b, std::size_t cursor, std::string t) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
    std::u32string wt = cvt.from_bytes(t);

    return paste(b, cursor, text{wt.begin(), wt.end()});
}

buffer_int new_cursor(buffer b) {
    int sz = b.cursors.size();
    b.cursors = b.cursors.push_back(cursor{});
    return buffer_int{b, sz};
}

std::size_t get_cursor_pos(buffer b, std::size_t cursor) {
    return b.cursors[cursor].point;
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

} // namespace meme
