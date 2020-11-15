#include <string>

#include "chai_init.hh"

std::string chai_init() {
    return R"(
class buffer {
  var b;
  var curr_cursor;
  var max_cursor;
  var flag_is_open;
  def buffer(x, o) { this.b = x; this.flag_is_open = o; this.curr_cursor = 0; }
  def is_open() { return this.flag_is_open; }
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
    this.b = r.mime_buffer_bool_get_buffer();
    return r.mime_buffer_bool_get_bool();
  }
  def set_mark() { this.b = set_mark_impl(this.b, this.curr_cursor); }
  def copy() { copy_impl(this.b, this.curr_cursor); }
  def cut() {
    var r = cut_impl(this.b, this.curr_cursor);
    this.b = r.mime_buffer_text_get_buffer();
    return r.mime_buffer_text_get_text();
  }
  def paste(t) { this.b = paste_impl(this.b, this.curr_cursor, t); }
  def insert(t) { this.b = insert_impl(this.b, this.curr_cursor, t); }
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
  def get_cursor_pos() { return get_cursor_pos_impl(this.b, this.curr_cursor); }
};

def open(string name) {
    var r = open_impl(name);
    return buffer(r.mime_buffer_bool_get_buffer(), r.mime_buffer_bool_get_bool());
}
)";
}
