#ifndef MIME_EXCEPT_HH
#define MIME_EXCEPT_HH

namespace mime {

class exit_exception {
  public:
    exit_exception(int code) throw() : code(code) {}
    char const *what() const throw() { return "exit called"; }
    int get_code() const throw() { return code; }

  private:
    int code{};
};

} // namespace mime
#endif /* MIME_EXCEPT_HH */
