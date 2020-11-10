#include <string>

#include "chai_init.hh"

std::string chai_init() {
    return R"(
class buffer {
  var b;
  def buffer(x) { this.b = x; }
  def search(text) {
    var r = search_impl(this.b, "default", text);
    this.b = r.meme_buffer_bool_b;
    r.meme_buffer_bool_success;
  }
  def set_mark() { this.b = set_mark_impl(this.b, "default"); }
  def copy() { copy_impl(this.b, "default"); }
  def cut() {
    var r = cut_impl(this.b, "default");
    this.b = r.meme_buffer_text_b;
    r.meme_buffer_text_t;
  }
  def flag_found() { this.b.meme_buffer_flags.meme_flag_found; }
};

def open_file(string name) {
    buffer(open_file_impl(name));
}
)";
}
