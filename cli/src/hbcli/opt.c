#include <hb/cfg.h>
#include <hbcli/opt.h>
#include <stddef.h>
#include <hb/proc.h>
#include <getopt.h>
#include <hbcli/err.h>

hb_cfg hbcli_options_parse(int argc, char** argv) {
    // Prepare config
    char *input_path = NULL;
    char *output_path = NULL;

    hb_cfg cfg;
    hb_proc_result result;

    bool nondefault_ex_collapse_whitespace = false;
    bool nondefault_ex_destroy_whole_whitespace = false;
    bool nondefault_ex_trim_whitespace = false;


    struct option long_options[] = {
            {"input", required_argument, NULL, 'i'},
            {"output", required_argument, NULL, 'o'},
            {"suppress", required_argument, NULL, 's'},

            {"!collapse-ws", optional_argument, NULL, 40},
            {"!destroy-whole-ws", optional_argument, NULL, 41},
            {"!trim-ws", optional_argument, NULL, 42},

            {"!trim-class-attr", no_argument, &(cfg.trim_class_attr), false},
            {"!decode-ent", no_argument, &(cfg.decode_entities), false},
            {"!remove-attr-quotes", no_argument, &(cfg.decode_entities), false},
            {"!remove-comments", no_argument, &(cfg.remove_comments), false},
            {"!remove-tag-ws", no_argument, &(cfg.remove_tag_whitespace), false},

            {0, 0, 0, 0}
    };

    // Parse arguments
    while (1) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "+i:o:s:", long_options, &option_index);

        if (c == -1) {
            if (optind != argc) {
                hbcli_err("Too many options");
            }
            break;
        }

        switch (c) {
            case 'i':
                input_path = optarg;
                break;

            case 'o':
                output_path = optarg;
                break;

            case 's':
                HBE_CATCH_F(hbu_streamoptions_parse_and_add_errors_to_suppress, config_stream->suppressed_errors, optarg);
                break;

            case 40:
                nondefault_ex_collapse_whitespace = 1;
                config_stream->ex_collapse_whitespace = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, optarg);
                break;

            case 41:
                nondefault_ex_destroy_whole_whitespace = 1;
                config_stream->ex_destroy_whole_whitespace = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, optarg);
                break;

            case 42:
                nondefault_ex_trim_whitespace = 1;
                config_stream->ex_trim_whitespace = HBE_CATCH_F(hbu_streamoptions_parse_list_of_tags, optarg);
                break;

            default:
                cli_error("Internal error: unknown option %c");
        }
    }

    if (!nondefault_ex_collapse_whitespace) config_stream->ex_collapse_whitespace = hbu_streamoptions_default_ex_collapse_whitespace();
    if (!nondefault_ex_destroy_whole_whitespace) config_stream->ex_destroy_whole_whitespace = hbu_streamoptions_default_ex_destroy_whole_whitespace();
    if (!nondefault_ex_trim_whitespace) config_stream->ex_trim_whitespace = hbu_streamoptions_default_ex_trim_whitespace();
}
