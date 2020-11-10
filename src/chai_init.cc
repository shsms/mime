#include <string>

#include "chai_init.hh"

std::string chai_init() {
    return R"(
class buffer {
  var b;
  def buffer(x) { this.b = x; }
  def find(text) { this.b = find_impl(this.b, "default", text); }
  def set_mark() { this.b = set_mark_impl(this.b, "default"); }
  def copy() { copy_impl(this.b, "default"); }
};

def open_file(string name) {
    buffer(open_file_impl(name));
}
)";
}
