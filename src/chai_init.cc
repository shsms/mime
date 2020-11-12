#include <string>

#include "chai_init.hh"

std::string chai_init() {
    return R"(
class text {
  var t;
  def text(x) { this.t = x; }
};

class buffer {
  var b;
  var curr_cursor;
  def buffer(x) { this.b = x; this.curr_cursor = 0; }
  def find(text) {
    var r = find_impl(this.b, this.curr_cursor, text);
    this.b = r.meme_buffer_bool_get_buffer();
    r.meme_buffer_bool_get_bool();
  }
  def rfind(text) {
    var r = rfind_impl(this.b, this.curr_cursor, text);
    this.b = r.meme_buffer_bool_get_buffer();
    r.meme_buffer_bool_get_bool();
  }
  def set_mark() { this.b = set_mark_impl(this.b, this.curr_cursor); }
  def copy() { copy_impl(this.b, this.curr_cursor); }
  def cut() {
    var r = cut_impl(this.b, this.curr_cursor);
    this.b = r.meme_buffer_text_get_buffer();
    r.meme_buffer_text_get_text();
  }
  def paste(t) { this.b = paste_impl(this.b, this.curr_cursor, t); }
};

def open_file(string name) {
    buffer(open_file_impl(name));
}
)";
}
