use crate::ast::RcdataContentType;
use crate::entity::encode::encode_entities;
use crate::tag::TAG_TEXTAREA_END;
use crate::tag::TAG_TITLE_END;
use crate::Cfg;

pub fn minify_rcdata(cfg: &Cfg, out: &mut Vec<u8>, typ: RcdataContentType, text: &[u8]) {
  // Encode entities, since they're still decoded by the browser.
  let html = encode_entities(text, false, !cfg.allow_optimal_entities);

  // Since the text has been decoded, there may be unintentional matches to end tags that we must escape.
  let html = match typ {
    RcdataContentType::Textarea => &*TAG_TEXTAREA_END,
    RcdataContentType::Title => &*TAG_TITLE_END,
  }
  .replace_all_bytes(&html, &[match typ {
    RcdataContentType::Textarea => {
      if cfg.allow_optimal_entities {
        b"&LT/textarea".as_slice()
      } else {
        b"&lt;/textarea".as_slice()
      }
    }
    RcdataContentType::Title => {
      if cfg.allow_optimal_entities {
        b"&LT/title".as_slice()
      } else {
        b"&lt;/title".as_slice()
      }
    }
  }]);

  out.extend_from_slice(&html);
}
