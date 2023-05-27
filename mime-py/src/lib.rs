use pyo3::prelude::*;

#[pyclass]
struct Window(::mime::Window);

#[pyclass]
#[derive(Clone)]
struct Text(::mime::Text);

#[pymethods]
impl Text {
    fn __str__(&self) -> String {
        self.0.to_string()
    }
}

#[derive(FromPyObject)]
enum StringOrText {
    String(String),
    Text(Text),
}

#[pymethods]
impl Window {
    #[staticmethod]
    pub fn new() -> Self {
        Self(::mime::Window::new())
    }

    #[staticmethod]
    pub fn open(filename: &str) -> Self {
        Self(::mime::Window::open(filename))
    }

    pub fn clone(&self) -> Self {
        Window(self.0.clone())
    }

    pub fn empty(&self) -> bool {
        self.0.empty()
    }

    pub fn size(&self) -> usize {
        self.0.size()
    }

    pub fn narrowed(&self) -> bool {
        self.0.narrowed()
    }

    pub fn save_as(&self, filename: &str) {
        self.0.save_as(filename)
    }

    pub fn set_mark(&self) {
        self.0.set_mark()
    }

    pub fn get_mark(&self) -> Option<i64> {
        self.0.get_mark()
    }

    pub fn get_contents(&self) -> Text {
        Text(self.0.get_contents().into())
    }

    pub fn find(&self, text: &str) -> Option<i64> {
        self.0.find(text)
    }

    pub fn rfind(&self, text: &str) -> Option<i64> {
        self.0.rfind(text)
    }

    pub fn replace(&self, from: &str, to: &str, n: usize) -> i32 {
        self.0.replace(from, to, n)
    }

    pub fn copy(&self) -> Text {
        Text(self.0.copy().into())
    }

    pub fn cut(&self) -> Text {
        Text(self.0.cut().into())
    }

    pub fn paste(&self, text: StringOrText) {
        match text {
            StringOrText::String(ref string) => self.0.paste(string.as_str()),
            StringOrText::Text(text) => self.0.paste(text.0),
        }
        // self.0.paste(t)
    }

    pub fn erase_region(&self) {
        self.0.erase_region()
    }

    pub fn clear(&self) {
        self.0.clear()
    }

    pub fn get_pos(&self) -> usize {
        self.0.get_pos()
    }

    pub fn goto_pos(&self, pos: i64) -> bool {
        self.0.goto_pos(pos)
    }

    pub fn del_backward(&self, n: usize) -> usize {
        self.0.del_backward(n)
    }

    pub fn del_forward(&self, n: usize) -> usize {
        self.0.del_forward(n)
    }

    pub fn backward(&self, n: usize) -> usize {
        self.0.backward(n)
    }

    pub fn forward(&self, n: usize) -> usize {
        self.0.forward(n)
    }

    pub fn prev_line(&self, n: usize) -> usize {
        self.0.prev_line(n)
    }

    pub fn next_line(&self, n: usize) -> usize {
        self.0.next_line(n)
    }

    pub fn start_of_buffer(&self) {
        self.0.start_of_buffer()
    }

    pub fn end_of_buffer(&self) {
        self.0.end_of_buffer()
    }

    pub fn start_of_line(&self) {
        self.0.start_of_line()
    }

    pub fn end_of_line(&self) {
        self.0.end_of_line()
    }

    pub fn start_of_block(&self) -> bool {
        self.0.start_of_block()
    }

    pub fn end_of_block(&self) -> bool {
        self.0.end_of_block()
    }

    pub fn narrow_to_block(&self) -> bool {
        self.0.narrow_to_block()
    }

    pub fn narrow_to_region(&self) -> bool {
        self.0.narrow_to_region()
    }

    pub fn widen(&self) {
        self.0.widen()
    }
}

#[pyfunction]
fn new() -> Window {
    Window::new()
}

#[pyfunction]
fn open(name: &str) -> Window {
    Window::open(name)
}

/// A Python module implemented in Rust.
#[pymodule]
fn mime(_py: Python, m: &PyModule) -> PyResult<()> {
    m.add_function(wrap_pyfunction!(new, m)?)?;
    m.add_function(wrap_pyfunction!(open, m)?)?;
    Ok(())
}
