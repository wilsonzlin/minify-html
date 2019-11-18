cmake_minimum_required(VERSION 3.14)
project(hyperbuild-cli C)

set(CMAKE_C_STANDARD 11)

# TODO Include submodule config, don't hardcode submodule's dependencies
include_directories(lib src ext/hyperbuild/lib)

add_executable(hyperbuild-cli
        src/hbcli/err.c
        src/hbcli/opt.c
        src/hbcli/arg/suppress.c
        src/hbcli/main.c src/hbcli/arg/tags.c)
