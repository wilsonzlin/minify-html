use crate::{ast::RcdataContentType, tag::{TAG_TEXTAREA_END, TAG_TITLE_END}, entity::encode::encode_entities};

pub fn minify_rcdata(out: &mut Vec<u8>, typ: RcdataContentType, text: &[u8]) {
    // Encode entities, since they're still decoded by the browser.
    let html = encode_entities(text, false);

    // Since the text has been decoded, there may be unintentional matches to end tags that we must escape.
    let html = match typ {
      RcdataContentType::Textarea => &*TAG_TEXTAREA_END,
      RcdataContentType::Title => &*TAG_TITLE_END,
    }.replace_all_bytes(&html, &[match typ {
        RcdataContentType::Textarea => b"&LT/textarea".as_slice(),
        RcdataContentType::Title => b"&LT/title".as_slice(),
    }]);

    out.extend_from_slice(&html);
}
