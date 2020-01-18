use phf::{Map, phf_map, phf_set, Set};

// Rules sourced from https://html.spec.whatwg.org/multipage/syntax.html#syntax-tag-omission.
// TODO html, head, body
// TODO Opening tags

pub enum ClosingTagOmissionRuleIfLast {
    // Closing tag can always be omitted if it's the last node of its parent's children.
    Always,
    // Closing tag can never be omitted if it's the last node of its parent's children.
    Never,
    // Closing tag can be omitted if it's the last node of its parent's children and the parent tag name is not one of these.
    ParentIsNot(Set<&'static [u8]>),
}

pub struct ClosingTagOmissionRule {
    // Closing tag can be omitted if immediately followed by an element node with one of these tag names.
    followed_by: Set<&'static [u8]>,
    // Closing tag can be omitted if it's the last node of its parent's children.
    is_last: ClosingTagOmissionRuleIfLast,
}

impl ClosingTagOmissionRule {
    pub fn can_omit_as_last_node(&self, parent: Option<&[u8]>) -> bool {
        match &self.is_last {
            ClosingTagOmissionRuleIfLast::Always => true,
            ClosingTagOmissionRuleIfLast::Never => false,
            ClosingTagOmissionRuleIfLast::ParentIsNot(parents) => match parent {
                Some(tag) => !parents.contains(tag),
                None => true,
            },
        }
    }

    pub fn can_omit_as_before(&self, after: &[u8]) -> bool {
        self.followed_by.contains(after)
    }
}

static LI_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"li"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static DT_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"dt", b"dd"),
    is_last: ClosingTagOmissionRuleIfLast::Never,
};

static DD_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"dd", b"dt"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static P_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(
        b"address",
        b"article",
        b"aside",
        b"blockquote",
        b"details",
        b"div",
        b"dl",
        b"fieldset",
        b"figcaption",
        b"figure",
        b"footer",
        b"form",
        b"h1",
        b"h2",
        b"h3",
        b"h4",
        b"h5",
        b"h6",
        b"header",
        b"hgroup",
        b"hr",
        b"main",
        b"menu",
        b"nav",
        b"ol",
        b"p",
        b"pre",
        b"section",
        b"table",
        b"ul",
    ),
    is_last: ClosingTagOmissionRuleIfLast::ParentIsNot(phf_set!(
        b"a",
        b"audio",
        b"del",
        b"ins",
        b"map",
        b"noscript",
        b"video",
    )),
};

static RT_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"rt", b"rp"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static RP_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"rt", b"rp"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static OPTGROUP_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"optgroup"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static OPTION_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"option", b"optgroup"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static THEAD_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"tbody", b"tfoot"),
    is_last: ClosingTagOmissionRuleIfLast::Never,
};

static TBODY_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"tbody", b"tfoot"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static TFOOT_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static TR_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"tr"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static TD_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"td", b"th"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

static TH_CLOSING_TAG_OMISSION_RULE: &ClosingTagOmissionRule = &ClosingTagOmissionRule {
    followed_by: phf_set!(b"td", b"th"),
    is_last: ClosingTagOmissionRuleIfLast::Always,
};

pub static CLOSING_TAG_OMISSION_RULES: Map<&'static [u8], &ClosingTagOmissionRule> = phf_map! {
    b"li" => LI_CLOSING_TAG_OMISSION_RULE,
    b"dt" => DT_CLOSING_TAG_OMISSION_RULE,
    b"dd" => DD_CLOSING_TAG_OMISSION_RULE,
    b"p" => P_CLOSING_TAG_OMISSION_RULE,
    b"rt" => RT_CLOSING_TAG_OMISSION_RULE,
    b"rp" => RP_CLOSING_TAG_OMISSION_RULE,
    b"optgroup" => OPTGROUP_CLOSING_TAG_OMISSION_RULE,
    b"option" => OPTION_CLOSING_TAG_OMISSION_RULE,
    b"thead" => THEAD_CLOSING_TAG_OMISSION_RULE,
    b"tbody" => TBODY_CLOSING_TAG_OMISSION_RULE,
    b"tfoot" => TFOOT_CLOSING_TAG_OMISSION_RULE,
    b"tr" => TR_CLOSING_TAG_OMISSION_RULE,
    b"td" => TD_CLOSING_TAG_OMISSION_RULE,
    b"th" => TH_CLOSING_TAG_OMISSION_RULE,
};
