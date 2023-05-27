use crate::text::_TextImpl;
use crate::{ffi::cpp, Text};

use cxx::{let_cxx_string, UniquePtr};
use std::sync::{Arc, Mutex};

unsafe impl Send for cpp::buffer {}

pub struct Window {
    buffer: Arc<Mutex<UniquePtr<cpp::buffer>>>,
    cursor: usize,
}

impl Clone for Window {
    /// Returns a new window for the same buffer, effectively adding a new
    /// cursor to the existing buffer.
    fn clone(&self) -> Self {
        let buffer = self.buffer.clone();
        let cursor = buffer.lock().unwrap().as_mut().unwrap().new_cursor();
        Self { buffer, cursor }
    }
}

impl Window {
    pub fn new() -> Self {
        let buffer = Arc::new(Mutex::new(cpp::new_buffer()));
        let cursor = buffer.lock().unwrap().as_mut().unwrap().new_cursor();
        Self { buffer, cursor }
    }

    pub fn open(filename: &str) -> Self {
        let_cxx_string!(filename = filename);
        let buffer = Arc::new(Mutex::new(cpp::open_buffer(&filename)));
        let cursor = buffer.lock().unwrap().as_mut().unwrap().new_cursor();
        Self { buffer, cursor }
    }

    pub fn empty(&self) -> bool {
        self.buffer.lock().unwrap().empty()
    }

    pub fn size(&self) -> usize {
        self.buffer.lock().unwrap().size()
    }

    pub fn narrowed(&self) -> bool {
        self.update_cursor();
        self.buffer.lock().unwrap().narrowed()
    }

    pub fn save_as(&self, filename: &str) {
        let_cxx_string!(filename = filename);
        self.buffer.lock().unwrap().save_as(&filename);
    }

    pub fn set_mark(&self) {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().set_mark();
    }

    // TODO: add bookmark interface
    pub fn get_mark(&self) -> Option<i64> {
        self.update_cursor();
        let pos = self.buffer.lock().unwrap().get_mark();
        (pos >= 0).then(|| pos)
    }

    pub fn get_contents(&self) -> Text {
        self.buffer.lock().unwrap().get_contents_box().into()
    }

    pub fn find(&self, text: &str) -> Option<i64> {
        self.update_cursor();
        let_cxx_string!(text = text);
        let pos = self.buffer.lock().unwrap().as_mut().unwrap().find(&text);
        (pos >= 0).then(|| pos)
    }

    pub fn rfind(&self, text: &str) -> Option<i64> {
        self.update_cursor();
        let_cxx_string!(text = text);
        let pos = self.buffer.lock().unwrap().as_mut().unwrap().rfind(&text);
        (pos >= 0).then(|| pos)
    }

    pub fn replace(&self, from: &str, to: &str, n: usize) -> i32 {
        self.update_cursor();
        let_cxx_string!(from = from);
        let_cxx_string!(to = to);
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .replace(&from, &to, n)
    }

    pub fn copy(&self) -> Text {
        self.update_cursor();
        self.buffer.lock().unwrap().copy_box().into()
    }

    pub fn cut(&self) -> Text {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .cut_box()
            .into()
    }

    pub fn paste<T: Into<Text>>(&self, text: T) {
        self.update_cursor();
        match &*text.into().0 {
            _TextImpl::Text(ref text) => self
                .buffer
                .lock()
                .unwrap()
                .as_mut()
                .unwrap()
                .paste_text(text),
            _TextImpl::Str(ref text) => {
                let_cxx_string!(text = text);
                self.buffer.lock().unwrap().as_mut().unwrap().paste(&text)
            }
        }
    }

    pub fn erase_region(&self) {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().erase_region()
    }

    pub fn clear(&self) {
        self.buffer.lock().unwrap().as_mut().unwrap().clear()
    }

    pub fn get_pos(&self) -> usize {
        self.update_cursor();
        self.buffer.lock().unwrap().get_pos()
    }

    pub fn goto_pos(&self, pos: i64) -> bool {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().goto_pos(pos)
    }

    pub fn del_backward(&self, n: usize) -> usize {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .del_backward(n)
    }

    pub fn del_forward(&self, n: usize) -> usize {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().del_forward(n)
    }

    pub fn backward(&self, n: usize) -> usize {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().backward(n)
    }

    pub fn forward(&self, n: usize) -> usize {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().forward(n)
    }

    pub fn prev_line(&self, n: usize) -> usize {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().prev_line(n)
    }

    pub fn next_line(&self, n: usize) -> usize {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().next_line(n)
    }

    pub fn start_of_buffer(&self) {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .start_of_buffer()
    }

    pub fn end_of_buffer(&self) {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .end_of_buffer()
    }

    pub fn start_of_line(&self) {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .start_of_line()
    }

    pub fn end_of_line(&self) {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().end_of_line()
    }

    pub fn start_of_block(&self) -> bool {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .start_of_block()
    }

    pub fn end_of_block(&self) -> bool {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().end_of_block()
    }

    pub fn narrow_to_block(&self) -> bool {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .narrow_to_block()
    }

    pub fn narrow_to_region(&self) -> bool {
        self.update_cursor();
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .narrow_to_region()
    }

    pub fn widen(&self) {
        self.update_cursor();
        self.buffer.lock().unwrap().as_mut().unwrap().widen()
    }
}

impl Default for Window {
    fn default() -> Self {
        Self::new()
    }
}

// Private functions
impl Window {
    fn update_cursor(&self) {
        self.buffer
            .lock()
            .unwrap()
            .as_mut()
            .unwrap()
            .use_cursor(self.cursor);
    }
}
