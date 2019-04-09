cmake_minimum_required(VERSION 3.13)
project(hyperbuild C)

set(CMAKE_C_STANDARD 11)

include_directories(src lib)

add_executable(hyperbuild
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
        src/hb/proc/view.c
        src/hb/err.c
        src/hb/proc/error.c
        src/hb/proc/bounds.c
        src/hb/proc/accept.c
        src/hb/proc/write.c
        src/hb/proc/require.c
        src/hb/proc/skip.c
        src/hb/rule/attr/name.c
        src/hb/rule/ascii/control.c
        src/hb/rule.h
        src/hb/rule/ascii/digit.c
        src/hb/rule/ascii/hex.c
        src/hb/rule/ascii/lowercase.c
        src/hb/rule/ascii/uppercase.c
        src/hb/rule/ascii/whitespace.c
        src/hb/rule/attr/quote.c
        src/hb/rule/tag/name.c
        src/hb/rule/attr/unquotedvalue.c
        src/hb/rule/entity/reference.c
        src/hb/collection.h)
