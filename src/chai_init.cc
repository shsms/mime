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
  def find(text) { return this.find(text, 0); }
  def find(text, int lim) {
    var r = find_impl(this.b, this.curr_cursor, text, lim);
    this.b = r.meme_buffer_bool_get_buffer();
    return r.meme_buffer_bool_get_bool();
  }
  def rfind(text) { return this.rfind(text, 0); }
  def rfind(text, int lim) {
    var r = rfind_impl(this.b, this.curr_cursor, text, lim);
    this.b = r.meme_buffer_bool_get_buffer();
    return r.meme_buffer_bool_get_bool();
  }
  def find_fuzzy(text) { return this.find_fuzzy(text, 0); }
  def find_fuzzy(text, int lim) {
    var r = find_fuzzy_impl(this.b, this.curr_cursor, text, lim);
    this.b = r.meme_buffer_bool_get_buffer();
    return r.meme_buffer_bool_get_bool();
  }
  def replace(from, to) { return this.replace(from, to, 0);  }
  def replace(from, to, n) {
    var r = replace_impl(this.b, this.curr_cursor, from, to, n);
    this.b = r.meme_buffer_bool_get_buffer();
    return r.meme_buffer_bool_get_bool();
  }
  def set_mark() { this.b = set_mark_impl(this.b, this.curr_cursor); }
  def copy() { copy_impl(this.b, this.curr_cursor); }
  def cut() {
    var r = cut_impl(this.b, this.curr_cursor);
    this.b = r.meme_buffer_text_get_buffer();
    return r.meme_buffer_text_get_text();
  }
  def paste(t) { this.b = paste_impl(this.b, this.curr_cursor, t); }
  def insert(t) { this.b = insert_impl(this.b, this.curr_cursor, t); }
  def save() { save_impl(this.b); }
  def save_as(name) { save_as_impl(this.b, name); }
};

def open(string name) {
    buffer(open_impl(name));
}
)";
}
