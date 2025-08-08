pub mod attr;
pub mod bang;
pub mod comment;
pub mod content;
#[cfg(feature = "lightningcss")]
pub mod css;
pub mod doctype;
pub mod element;
pub mod instruction;
#[cfg(feature = "minify-js")]
pub mod js;
pub mod rcdata;
#[cfg(test)]
mod tests;
