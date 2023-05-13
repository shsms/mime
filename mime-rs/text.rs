use std::fmt::Display;
use std::sync::Arc;

use cxx::UniquePtr;

use crate::ffi::cpp;

unsafe impl Send for cpp::text {}
unsafe impl Sync for cpp::text {}
unsafe impl Send for _TextImpl {}
unsafe impl Sync for _TextImpl {}

#[derive(Clone)]
pub struct Text(pub(crate) Arc<_TextImpl>);

impl From<UniquePtr<cpp::text>> for Text {
    fn from(value: UniquePtr<cpp::text>) -> Self {
        Text(Arc::new(_TextImpl::Text(value)))
    }
}

impl From<String> for Text {
    fn from(value: String) -> Self {
        Text(Arc::new(_TextImpl::Str(value)))
    }
}

impl From<&str> for Text {
    fn from(value: &str) -> Self {
        Text(Arc::new(_TextImpl::Str(value.to_string())))
    }
}

pub(crate) enum _TextImpl {
    Text(UniquePtr<cpp::text>),
    Str(String),
}

impl Display for Text {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match &*self.0 {
            _TextImpl::Text(t) => f.write_str(&cpp::text_to_string(&t).to_string()),
            _TextImpl::Str(t) => f.write_str(&t),
        }
    }
}

impl Text {
    pub fn to_string(&self) -> String {
        match &*self.0 {
            _TextImpl::Text(t) => cpp::text_to_string(&t).to_string(),
            _TextImpl::Str(t) => t.to_owned(),
        }
    }
}
