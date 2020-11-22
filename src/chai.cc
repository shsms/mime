#include <string>

#include "buffer.hh"
#include "chai.hh"
#include <chaiscript/chaiscript.hpp>

namespace mime {
void add_methods(chaiscript::ChaiScript &chai) {
    chai.add(chaiscript::user_type<buffer>(), "mime_buffer");
    chai.add(chaiscript::user_type<text>(), "mime_text");
    chai.add(chaiscript::fun(&buffer::contents), "mime_buffer_contents");

    chai.add(chaiscript::fun(&buffer_text::get_buffer), "mime_buffer_text_get_buffer");
    chai.add(chaiscript::fun(&buffer_text::get_text), "mime_buffer_text_get_text");
    chai.add(chaiscript::fun(&buffer_bool::get_buffer), "mime_buffer_bool_get_buffer");
    chai.add(chaiscript::fun(&buffer_bool::get_bool), "mime_buffer_bool_get_bool");
    chai.add(chaiscript::fun(&buffer_int::get_buffer), "mime_buffer_int_get_buffer");
    chai.add(chaiscript::fun(&buffer_int::get_int), "mime_buffer_int_get_int");

    chai.add(chaiscript::fun(&to_string), "to_string");

    chai.add(chaiscript::fun(&open), "open_impl");
    chai.add(chaiscript::fun(&save), "save_impl");
    chai.add(chaiscript::fun(&save_as), "save_as_impl");
    chai.add(chaiscript::fun(&set_mark), "set_mark_impl");
    chai.add(
        chaiscript::fun(
            static_cast<buffer_bool (*)(buffer, std::size_t, std::string, std::size_t)>(&find)),
        "find_impl");
    chai.add(chaiscript::fun(
                 static_cast<buffer_bool (*)(buffer, std::size_t, text, std::size_t)>(&find)),
             "find_impl");
    chai.add(
        chaiscript::fun(
            static_cast<buffer_bool (*)(buffer, std::size_t, std::string, std::size_t)>(&rfind)),
        "rfind_impl");
    chai.add(chaiscript::fun(
                 static_cast<buffer_bool (*)(buffer, std::size_t, text, std::size_t)>(&rfind)),
             "rfind_impl");
    chai.add(
        chaiscript::fun(static_cast<buffer_bool (*)(buffer, std::size_t, std::string, std::size_t)>(
            &find_fuzzy)),
        "find_fuzzy_impl");
    chai.add(chaiscript::fun(
                 static_cast<buffer_bool (*)(buffer, std::size_t, text, std::size_t)>(&find_fuzzy)),
             "find_fuzzy_impl");
    chai.add(chaiscript::fun(&replace), "replace_impl");

    chai.add(chaiscript::fun(&copy), "copy_impl");
    chai.add(chaiscript::fun(&cut), "cut_impl");
    chai.add(chaiscript::fun(static_cast<buffer (*)(buffer, std::size_t, text)>(&paste)),
             "paste_impl");
    chai.add(chaiscript::fun(static_cast<buffer (*)(buffer, std::size_t, std::string)>(&paste)),
             "paste_impl");
    chai.add(chaiscript::fun(&erase_region), "erase_region_impl");
    chai.add(chaiscript::fun(&clear), "clear_impl");

    chai.add(chaiscript::fun(&new_cursor), "new_cursor_impl");
    chai.add(chaiscript::fun(&get_pos), "get_pos_impl");
    chai.add(chaiscript::fun(&goto_pos), "goto_pos_impl");

    chai.add(chaiscript::fun(&forward), "forward_impl");
    chai.add(chaiscript::fun(&backward), "backward_impl");
    chai.add(chaiscript::fun(&next_line), "next_line_impl");
    chai.add(chaiscript::fun(&prev_line), "prev_line_impl");
    chai.add(chaiscript::fun(&start_of_buffer), "start_of_buffer_impl");
    chai.add(chaiscript::fun(&end_of_buffer), "end_of_buffer_impl");
    chai.add(chaiscript::fun(&start_of_line), "start_of_line_impl");
    chai.add(chaiscript::fun(&end_of_line), "end_of_line_impl");

    chai.add(chaiscript::fun([](buffer &lhs, const buffer &rhs) -> buffer & { return lhs = rhs; }),
             "=");
    chai.add(chaiscript::fun([](text &lhs, const text &rhs) -> text & { return lhs = rhs; }), "=");

    chai.add(chaiscript::fun([](const std::string &lhs, const text &rhs) -> text {
                 std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
                 std::u32string u32lhs = cvt.from_bytes(lhs);
                 return text{u32lhs.begin(), u32lhs.end()} + rhs;
             }),
             "+");
    chai.add(chaiscript::fun([](const text &lhs, const std::string &rhs) -> text {
                 std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
                 std::u32string u32rhs = cvt.from_bytes(rhs);
                 return lhs + text{u32rhs.begin(), u32rhs.end()};
             }),
             "+");
    chai.add(chaiscript::fun([](const text &lhs, const text &rhs) -> text { return lhs + rhs; }),
             "+");

    chai.add(chaiscript::fun([](const text &lhs, const text &rhs) -> bool { return lhs == rhs; }),
             "==");
    chai.add(chaiscript::fun([](const std::string &lhs, const text &rhs) -> bool {
                 std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
                 std::u32string u32lhs = cvt.from_bytes(lhs);
                 if (u32lhs.size() != rhs.size()) {
                     return false;
                 }
                 return text{lhs.begin(), lhs.end()} == rhs;
             }),
             "==");
    chai.add(chaiscript::fun([](const text &lhs, const std::string &rhs) -> bool {
                 std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;
                 std::u32string u32rhs = cvt.from_bytes(rhs);
                 if (u32rhs.size() != lhs.size()) {
                     return false;
                 }
                 return text{rhs.begin(), rhs.end()} == lhs;
             }),
             "==");
}

std::string chai_init() {
    return R"(
class buffer {
  var b;
  var curr_cursor;
  var max_cursor;
  var flag_is_new;
  def buffer(x, o) { this.b = x; this.flag_is_new = o; this.curr_cursor = 0; }
  def is_new() { return this.flag_is_new; }
  def find(text) { return this.find(text, 0); }
  def find(text, int lim) {
    var r = find_impl(this.b, this.curr_cursor, text, lim);
    this.b = r.mime_buffer_bool_get_buffer();
    return r.mime_buffer_bool_get_bool();
  }
  def rfind(text) { return this.rfind(text, 0); }
  def rfind(text, int lim) {
    var r = rfind_impl(this.b, this.curr_cursor, text, lim);
    this.b = r.mime_buffer_bool_get_buffer();
    return r.mime_buffer_bool_get_bool();
  }
  def find_fuzzy(text) { return this.find_fuzzy(text, 0); }
  def find_fuzzy(text, int lim) {
    var r = find_fuzzy_impl(this.b, this.curr_cursor, text, lim);
    this.b = r.mime_buffer_bool_get_buffer();
    return r.mime_buffer_bool_get_bool();
  }
  def replace(from, to) { return this.replace(from, to, 0);  }
  def replace(from, to, n) {
    var r = replace_impl(this.b, this.curr_cursor, from, to, n);
    this.b = r.mime_buffer_int_get_buffer();
    return r.mime_buffer_int_get_int();
  }
  def set_mark() { this.b = set_mark_impl(this.b, this.curr_cursor); }
  def copy() { copy_impl(this.b, this.curr_cursor); }
  def cut() {
    var r = cut_impl(this.b, this.curr_cursor);
    this.b = r.mime_buffer_text_get_buffer();
    return r.mime_buffer_text_get_text();
  }
  def erase_region() { this.b = erase_region_impl(this.b, this.curr_cursor); }
  def paste(t) { this.b = paste_impl(this.b, this.curr_cursor, t); }
  def clear() { this.b = clear_impl(this.b); }
  def save() { save_impl(this.b); }
  def save_as(name) { save_as_impl(this.b, name); }
  def new_cursor() {
    var r = new_cursor_impl(this.b);
    this.b = r.mime_buffer_int_get_buffer();
    this.max_cursor = r.mime_buffer_int_get_int();
    return this.max_cursor;
  }
  def use_cursor(int c) {
    if (c > this.max_cursor) {
       print("error: attempt to use cursor" + to_string(c) + " > " + to_string(this.max_cursor));
       return false;
    }
    this.curr_cursor = c;
    return true;
  }
  def get_pos() { return get_pos_impl(this.b, this.curr_cursor); }
  def goto_pos(pos) {
    var r = goto_pos_impl(this.b, this.curr_cursor, pos);
    this.b = r.mime_buffer_bool_get_buffer();
    return r.mime_buffer_bool_get_bool();
  }

  def forward() { this.forward(1); }
  def backward() { this.backward(1); }
  def next_line() { this.next_line(1); }
  def prev_line() { this.prev_line(1); }
  def forward(n) { this.b = forward_impl(this.b, this.curr_cursor, n); }
  def backward(n) { this.b = backward_impl(this.b, this.curr_cursor, n); }
  def next_line(n) {
    var r = next_line_impl(this.b, this.curr_cursor, n);
    this.b = r.mime_buffer_int_get_buffer();
    return r.mime_buffer_int_get_int();    
  }
  def prev_line(n) {
    var r = prev_line_impl(this.b, this.curr_cursor, n);
    this.b = r.mime_buffer_int_get_buffer();
    return r.mime_buffer_int_get_int();
  }
  def start_of_buffer() { this.b = start_of_buffer_impl(this.b, this.curr_cursor); }
  def end_of_buffer() { this.b = end_of_buffer_impl(this.b, this.curr_cursor); }
  def start_of_line() { this.b = start_of_line_impl(this.b, this.curr_cursor); }
  def end_of_line() { this.b = end_of_line_impl(this.b, this.curr_cursor); }
};

def open() {
  return open("");
}
def open(string name) {
  var r = open_impl(name);
  return buffer(r.mime_buffer_bool_get_buffer(), r.mime_buffer_bool_get_bool());
}
)";
}

void run(std::string filename) {
    chaiscript::ChaiScript chai;
    add_methods(chai);
    chai.eval(chai_init());
    chai.eval_file(filename);
}

} // namespace mime
