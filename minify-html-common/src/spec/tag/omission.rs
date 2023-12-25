// Rules sourced from https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission.
// TODO Opening tags

use ahash::AHashMap;
use ahash::AHashSet;
use once_cell::sync::Lazy;

enum ClosingTagOmissionRuleIfLast {
  // Closing tag can always be omitted if it's the last node of its parent's children.
  Always,
  // Closing tag can never be omitted if it's the last node of its parent's children.
  Never,
  // Closing tag can be omitted if it's the last node of its parent's children and the parent tag name is not one of these.
  ParentIsNot(AHashSet<&'static [u8]>),
}

// What this means in effect while parsing:
// - Given we are processing the content of some element B, which itself is inside A (e.g. <A><B>):
//   - If we see `</C` and B != C:
//     - If C == A and C is compatible with is_last, B is closed implicitly.
//   - If we see `<C` and maybe B == C:
//     - If C is in followed_by, B is closed implicitly.
struct ClosingTagOmissionRule {
  // Closing tag can be omitted if immediately followed by an element node with one of these tag names.
  followed_by: AHashSet<&'static [u8]>,
  // Closing tag can be omitted if it's the last node of its parent's children.
  is_last: ClosingTagOmissionRuleIfLast,
}

static HTML_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: AHashSet::default(),
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static HEAD_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"body");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static BODY_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: AHashSet::default(),
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static LI_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"li");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static DT_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"dt");
      s.insert(b"dd");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Never,
  });

static DD_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"dd");
      s.insert(b"dt");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static P_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> = Lazy::new(|| {
  let mut followed_by = AHashSet::<&'static [u8]>::default();
  followed_by.insert(b"address");
  followed_by.insert(b"article");
  followed_by.insert(b"aside");
  followed_by.insert(b"blockquote");
  followed_by.insert(b"details");
  followed_by.insert(b"div");
  followed_by.insert(b"dl");
  followed_by.insert(b"fieldset");
  followed_by.insert(b"figcaption");
  followed_by.insert(b"figure");
  followed_by.insert(b"footer");
  followed_by.insert(b"form");
  followed_by.insert(b"h1");
  followed_by.insert(b"h2");
  followed_by.insert(b"h3");
  followed_by.insert(b"h4");
  followed_by.insert(b"h5");
  followed_by.insert(b"h6");
  followed_by.insert(b"header");
  followed_by.insert(b"hgroup");
  followed_by.insert(b"hr");
  followed_by.insert(b"main");
  followed_by.insert(b"menu");
  followed_by.insert(b"nav");
  followed_by.insert(b"ol");
  followed_by.insert(b"p");
  followed_by.insert(b"pre");
  followed_by.insert(b"section");
  followed_by.insert(b"table");
  followed_by.insert(b"ul");

  let mut is_last_tags = AHashSet::<&'static [u8]>::default();
  is_last_tags.insert(b"a");
  is_last_tags.insert(b"audio");
  is_last_tags.insert(b"del");
  is_last_tags.insert(b"ins");
  is_last_tags.insert(b"map");
  is_last_tags.insert(b"noscript");
  is_last_tags.insert(b"video");

  ClosingTagOmissionRule {
    followed_by,
    is_last: ClosingTagOmissionRuleIfLast::ParentIsNot(is_last_tags),
  }
});

static RT_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"rt");
      s.insert(b"rp");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static RP_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"rt");
      s.insert(b"rp");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static OPTGROUP_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"optgroup");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static OPTION_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"option");
      s.insert(b"optgroup");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static THEAD_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"tbody");
      s.insert(b"tfoot");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Never,
  });

static TBODY_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"tbody");
      s.insert(b"tfoot");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static TFOOT_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: AHashSet::<&'static [u8]>::default(),
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static TR_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"tr");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static TD_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"td");
      s.insert(b"th");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static TH_CLOSING_TAG_OMISSION_RULE: Lazy<ClosingTagOmissionRule> =
  Lazy::new(|| ClosingTagOmissionRule {
    followed_by: {
      let mut s = AHashSet::<&'static [u8]>::default();
      s.insert(b"td");
      s.insert(b"th");
      s
    },
    is_last: ClosingTagOmissionRuleIfLast::Always,
  });

static CLOSING_TAG_OMISSION_RULES: Lazy<AHashMap<&'static [u8], &'static ClosingTagOmissionRule>> =
  Lazy::new(|| {
    let mut m = AHashMap::<&'static [u8], &'static ClosingTagOmissionRule>::default();
    m.insert(b"html", &HTML_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"head", &HEAD_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"body", &BODY_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"li", &LI_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"dt", &DT_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"dd", &DD_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"p", &P_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"rt", &RT_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"rp", &RP_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"optgroup", &OPTGROUP_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"option", &OPTION_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"thead", &THEAD_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"tbody", &TBODY_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"tfoot", &TFOOT_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"tr", &TR_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"td", &TD_CLOSING_TAG_OMISSION_RULE);
    m.insert(b"th", &TH_CLOSING_TAG_OMISSION_RULE);
    m
  });

// Use an empty slice for `parent` if no parent.
pub fn can_omit_as_last_node(parent: &[u8], child: &[u8]) -> bool {
  CLOSING_TAG_OMISSION_RULES
    .get(child)
    .filter(|r| match &r.is_last {
      ClosingTagOmissionRuleIfLast::Always => true,
      ClosingTagOmissionRuleIfLast::Never => false,
      ClosingTagOmissionRuleIfLast::ParentIsNot(parents) => !parents.contains(parent),
    })
    .is_some()
}

// Use an empty slice for `before` or `after` if no previous/next sibling element.
pub fn can_omit_as_before(before: &[u8], after: &[u8]) -> bool {
  CLOSING_TAG_OMISSION_RULES
    .get(before)
    .filter(|r| r.followed_by.contains(after))
    .is_some()
}
