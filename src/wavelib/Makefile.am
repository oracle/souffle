# Souffle version 0.0.0
# (c) 2016 by the Souffle Development Team
# Licensed under the Universal Permissive License v 1.0 as shown at:
# - https://opensource.org/licenses/UPL
# - souffle/LICENSE

SUFFIXES = .cpp .h .yy .ll .cc .hh .hpp

EXTRA_DIST = cpp_context.hpp \
             cpp_iteration_context.hpp \
             grammars/cpp_chlit_grammar.hpp \
             grammars/cpp_expression_grammar_gen.hpp \
             grammars/cpp_expression_grammar.hpp \
             grammars/cpp_intlit_grammar.hpp \
             grammars/cpp_predef_macros_grammar.hpp \
             grammars/cpp_grammar_gen.hpp \
             grammars/cpp_expression_value.hpp \
             grammars/cpp_defined_grammar_gen.hpp \
             grammars/cpp_value_error.hpp \
             grammars/cpp_predef_macros_gen.hpp \
             grammars/cpp_grammar.hpp \
             grammars/cpp_defined_grammar.hpp \
             grammars/cpp_literal_grammar_gen.hpp \
             cpp_exceptions.hpp \
             preprocessing_hooks.hpp \
             wave_version.hpp \
             cpplexer/re2clex/cpp_re.hpp \
             cpplexer/re2clex/cpp_re2c_lexer.hpp \
             cpplexer/re2clex/scanner.hpp \
             cpplexer/re2clex/aq.hpp \
             cpplexer/cpplexer_exceptions.hpp \
             cpplexer/validate_universal_char.hpp \
             cpplexer/cpp_lex_iterator.hpp \
             cpplexer/token_cache.hpp \
             cpplexer/cpp_lex_interface.hpp \
             cpplexer/cpp_lex_token.hpp \
             cpplexer/cpp_lex_interface_generator.hpp \
             cpplexer/convert_trigraphs.hpp \
             cpplexer/detect_include_guards.hpp \
             token_ids.hpp \
             cpp_throw.hpp \
             wave_config.hpp \
             language_support.hpp \
             wave.hpp \
             wave_config_constant.hpp \
             util/transform_iterator.hpp \
             util/cpp_ifblock.hpp \
             util/unput_queue_iterator.hpp \
             util/time_conversion_helper.hpp \
             util/interpret_pragma.hpp \
             util/functor_input.hpp \
             util/cpp_iterator.hpp \
             util/filesystem_compatibility.hpp \
             util/iteration_context.hpp \
             util/cpp_macromap_predef.hpp \
             util/cpp_include_paths.hpp \
             util/cpp_macromap.hpp \
             util/symbol_table.hpp \
             util/macro_definition.hpp \
             util/file_position.hpp \
             util/flex_string.hpp \
             util/macro_helpers.hpp \
             util/pattern_parser.hpp \
             util/cpp_macromap_utils.hpp \
             util/insert_whitespace_detection.hpp \
             whitespace_handling.hpp \
             cpplexer/re2clex/cpp_re.inc \
             cpplexer/re2clex/strict_cpp_re.inc

noinst_LIBRARIES = libwave.a

libwave_a_SOURCES = instantiate_cpp_literalgrs.cpp \
                    instantiate_cpp_exprgrammar.cpp \
                    instantiate_predef_macros.cpp \
                    wave_config_constant.cpp \
                    instantiate_re2c_lexer_str.cpp \
                    cpplexer/re2clex/cpp_re.cpp \
                    cpplexer/re2clex/aq.cpp \
                    instantiate_re2c_lexer.cpp \
                    instantiate_cpp_grammar.cpp \
                    instantiate_defined_grammar.cpp \
                    token_ids.cpp

libwave_a_CXXFLAGS = -I@abs_top_srcdir@/src/


