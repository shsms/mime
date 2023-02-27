#[cxx::bridge(namespace = "mime")]
pub(crate) mod cpp {

    unsafe extern "C++" {
        include!("mime/mime.hh");

        type text;
        type buffer;

        fn new_buffer() -> UniquePtr<buffer>;
        fn open_buffer(name: &CxxString) -> UniquePtr<buffer>;

        fn empty(self: &buffer) -> bool;
        fn size(self: &buffer) -> usize;
        fn narrowed(self: &buffer) -> bool;

        fn save_as(self: &buffer, name: &CxxString);

        fn set_mark(self: Pin<&mut buffer>);
        fn get_mark(self: &buffer) -> i64;

        fn get_contents_box(self: &buffer) -> UniquePtr<text>;

        fn find(self: Pin<&mut buffer>, t: &CxxString) -> i64;
        // fn find_text(self: Pin<&mut buffer>, t: &text) -> i64;

        fn rfind(self: Pin<&mut buffer>, t: &CxxString) -> i64;
        // fn rfind_text(self: Pin<&mut buffer>, t: &text) -> i64;

        fn replace(self: Pin<&mut buffer>, from: &CxxString, to: &CxxString, n: usize) -> i32;
        // fn replace_all(self: Pin<&mut buffer>, from: &CxxString, to: &CxxString) -> i32;

        fn copy_box(self: &buffer) -> UniquePtr<text>;
        fn cut_box(self: Pin<&mut buffer>) -> UniquePtr<text>;

        fn paste(self: Pin<&mut buffer>, t: &CxxString);
        fn paste_text(self: Pin<&mut buffer>, t: &text);

        fn erase_region(self: Pin<&mut buffer>);
        fn clear(self: Pin<&mut buffer>);

        fn del_backward(self: Pin<&mut buffer>, n: usize) -> usize;
        fn del_forward(self: Pin<&mut buffer>, n: usize) -> usize;

        fn new_cursor(self: Pin<&mut buffer>) -> usize;
        fn use_cursor(self: Pin<&mut buffer>, c: usize);
        fn get_pos(self: &buffer) -> usize;
        fn goto_pos(self: Pin<&mut buffer>, pos: i64) -> bool;

        fn forward(self: Pin<&mut buffer>, n: usize) -> usize;
        fn backward(self: Pin<&mut buffer>, n: usize) -> usize;
        fn next_line(self: Pin<&mut buffer>, n: usize) -> usize;
        fn prev_line(self: Pin<&mut buffer>, n: usize) -> usize;

        fn start_of_buffer(self: Pin<&mut buffer>);
        fn end_of_buffer(self: Pin<&mut buffer>);
        fn start_of_line(self: Pin<&mut buffer>);
        fn end_of_line(self: Pin<&mut buffer>);

        fn start_of_block(self: Pin<&mut buffer>) -> bool;
        fn end_of_block(self: Pin<&mut buffer>) -> bool;

        fn narrow_to_block(self: Pin<&mut buffer>) -> bool;
        fn narrow_to_region(self: Pin<&mut buffer>) -> bool;
        fn widen(self: Pin<&mut buffer>);

        fn text_to_string(t: &text) -> UniquePtr<CxxString>;
    }
}
