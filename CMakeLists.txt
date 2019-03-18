cmake_minimum_required(VERSION 3.13)
project(hyperbuild C)

set(CMAKE_C_STANDARD 11)

include_directories(src lib)

add_executable(hyperbuild
        src/rule/__main__.c
        src/rule/tag/contenttags.c
        src/rule/tag/specifictags.c
        src/rule/tag/voidtags.c
        src/rule/tag/svgtags.c
        src/rule/tag/sectioningtags.c
        src/rule/tag/layouttags.c
        src/rule/tag/mediatags.c
        src/rule/tag/tags.c
        src/rule/tag/htmltags.c
        src/rule/tag/formattingtags.c
        src/rule/tag/headingtags.c
        src/rule/tag/wsstags.c
        src/rule/tag/contentfirsttags.c
        src/rule/char/attrname.c
        src/rule/char/whitespace.c
        src/rule/char/digit.c
        src/rule/char/unquotedattrval.c
        src/rule/char/c0.c
        src/rule/char/hex.c
        src/rule/char/attrvalquote.c
        src/rule/char/tagname.c
        src/rule/char/lcalpha.c
        src/rule/char/ucalpha.c
        src/rule/entity/entityrefs.c
        src/rule/relation/blacklistchildren.c
        src/rule/relation/whitelistchildren.c
        src/rule/relation/blacklistparents.c
        src/rule/relation/whitelistparents.c
        src/hb/rune.h
        src/hb/proc.h
        src/hb/proc/matches.c
        src/hb/proc/peek.c
        src/hb/unit/attr.c
        src/hb/unit/tag/name.c
        src/hb/unit/tag.c
        src/hb/unit/attr/val/unquoted.c
        src/hb/unit/attr/val/quoted.c
        src/hb/unit/attr/val.c
        src/hb/unit/content/style.c
        src/hb/unit/content/html.c
        src/hb/unit/content/script.c
        src/hb/unit/bang.c
        src/hb/unit/entity.c
        src/hb/unit/comment.c
        src/hb/unit.h
        src/hb/err.h
        src/hb/cfg.h
        src/hb/proc/view.c)
