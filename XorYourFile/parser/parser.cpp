#include "parser.h"
#define XORSTR_MACRO "xorstr_"
#define XORSTR_START "("
#define XORSTR_END   ")"
#define STRING(x)    std::string(x)
#include <string>

std::string wrap_xorstr(std::string str_to_wrap) {
    return STRING(XORSTR_MACRO) + STRING(XORSTR_START) + str_to_wrap + STRING(XORSTR_END);
}

size_t get_new_offset() {
    return STRING(XORSTR_MACRO).size() + STRING(XORSTR_START).size() + STRING(XORSTR_END).size();
}

bool is_quote_shielded(const std::string& string, size_t quote_index) {
    size_t backslash_count = 0;
    size_t j = quote_index - 1;
    while (j >= 0 && string[j] == '\\') {
        backslash_count++;
        if (j == 0) {
            break;
        }
        j--;
    }
    return backslash_count % 2 == 0;
}

std::vector<std::pair<size_t, size_t>> find_comment_ranges(const std::string& str) {
    std::vector<std::pair<size_t, size_t>> result;
    size_t i = 0;

    while (i < str.size()) {
        if (str[i] == '/' && i + 1 < str.size()) {
            if (str[i + 1] == '/') {
                size_t start = i;
                size_t end = str.find('\n', i);
                if (end == std::string::npos) end = str.size();
                result.emplace_back(start, end);
                i = end;
                continue;
            }
            if (str[i + 1] == '*') {
                size_t start = i;
                size_t end = str.find("*/", i + 2);
                if (end == std::string::npos) end = str.size();
                else end += 2;
                result.emplace_back(start, end);
                i = end;
                continue;
            }
        }
        i++;
    }

    return result;
}

bool is_inside_comment(size_t index, const std::vector<std::pair<size_t, size_t>>& comment_ranges) {
    for (auto& range : comment_ranges) {
        if (index >= range.first && index < range.second) {
            return true;
        }
    }
    return false;
}

bool is_quote_after_literal(const std::string& string, size_t quote_index) {
    if (quote_index == 0) {
        return false;
    }

    const char prev_char = string[quote_index - 1];
    bool is_default_literal = prev_char == 'L' || prev_char == 'R';
    if (is_default_literal) {
        return true;
    }

    if (quote_index >= 2) {
        if (prev_char == '8' && string[quote_index - 2] == 'u') {
            return true;
        }
    }

    return false;
}

bool is_preprocessor_line(const std::string& str, size_t quote_index) {
    size_t line_start = str.rfind('\n', quote_index);
    if (line_start == std::string::npos)
        line_start = 0;
    else
        line_start += 1;

    while (line_start < str.size() && isspace(str[line_start])) {
        line_start++;
    }

    return str[line_start] == '#';
}

bool is_define_line(const std::string& str, size_t quote_index) {
    size_t line_start = str.rfind('\n', quote_index);
    if (line_start == std::string::npos)
        line_start = 0;
    else
        line_start += 1;

    while (line_start < str.size() && isspace(str[line_start])) {
        line_start++;
    }

    return str.compare(line_start, 7, "#define") == 0;
}

std::vector<std::pair<size_t, size_t>> index_quotes(const std::string& str_to_index) {
    std::vector<std::pair<size_t, size_t>> result = {};
    std::vector<std::pair<size_t, size_t>> comment_ranges = find_comment_ranges(str_to_index);
    bool is_inside_string = false;
    size_t start_position = 0;

    for (size_t i = 0; i < str_to_index.size(); i++) {
        const char target_char = str_to_index[i];
        if (target_char == '"' && is_quote_shielded(str_to_index, i)) {
            if (!is_inside_string) {
                start_position = i;
                if (is_quote_after_literal(str_to_index, start_position)) {
                    start_position--;
                }
                is_inside_string = true;
            }
            else {
                is_inside_string = false;
                if (is_inside_comment(start_position, comment_ranges) ||
                    is_inside_comment(i, comment_ranges)) {
                    continue;
                }
                if (is_preprocessor_line(str_to_index, i) && !is_define_line(str_to_index, i)) {
                    continue;
                }
                result.push_back({start_position, i});
            }
        }
    }

    return result;
}

void handle_content(std::string& str_to_handle, std::vector<std::pair<size_t, size_t>>& indices) {
    size_t total_offset = 0;
    printf("Xored Strings: \n");
    for (auto& indice : indices) {
        size_t start_original = indice.first;
        size_t count = indice.second - indice.first + 1;

        size_t start = start_original + total_offset;
        std::string target_string = str_to_handle.substr(start, count);
        printf("%s\n", target_string.c_str());

        std::string new_string = wrap_xorstr(target_string);
        str_to_handle.replace(start, count, new_string);
        total_offset += new_string.size() - count;
    }
}