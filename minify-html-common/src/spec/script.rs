use ahash::AHashSet;
use once_cell::sync::Lazy;

pub static JAVASCRIPT_MIME_TYPES: Lazy<AHashSet<&'static [u8]>> = Lazy::new(|| {
  let mut s = AHashSet::<&'static [u8]>::default();
  s.insert(b"application/ecmascript");
  s.insert(b"application/javascript");
  s.insert(b"application/x-ecmascript");
  s.insert(b"application/x-javascript");
  s.insert(b"text/ecmascript");
  s.insert(b"text/javascript");
  s.insert(b"text/javascript1.0");
  s.insert(b"text/javascript1.1");
  s.insert(b"text/javascript1.2");
  s.insert(b"text/javascript1.3");
  s.insert(b"text/javascript1.4");
  s.insert(b"text/javascript1.5");
  s.insert(b"text/jscript");
  s.insert(b"text/livescript");
  s.insert(b"text/x-ecmascript");
  s.insert(b"text/x-javascript");
  s
});
