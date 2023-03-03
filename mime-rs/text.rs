use std::fmt::Display;

use cxx::UniquePtr;

use crate::ffi::cpp;

unsafe impl Send for cpp::text {}

pub struct Text(pub(crate) _TextImpl);

impl From<UniquePtr<cpp::text>> for Text {
    fn from(value: UniquePtr<cpp::text>) -> Self {
        Text(_TextImpl::Text(value))
    }
}

impl From<String> for Text {
    fn from(value: String) -> Self {
        Text(_TextImpl::Str(value))
    }
}

impl From<&str> for Text {
    fn from(value: &str) -> Self {
        Text(_TextImpl::Str(value.to_string()))
    }
}

pub(crate) enum _TextImpl {
    Text(UniquePtr<cpp::text>),
    Str(String),
}

impl Display for Text {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match &self.0 {
            _TextImpl::Text(t) => f.write_str(&cpp::text_to_string(&t).to_string()),
            _TextImpl::Str(t) => f.write_str(&t),
        }
    }
}
