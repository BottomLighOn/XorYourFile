#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
std::string wrap_xorstr(std::string str_to_wrap);
std::vector<std::pair<size_t, size_t>> index_quotes(const std::string& str_to_index);
void handle_content(std::string& str_to_handle, std::vector<std::pair<size_t, size_t>>& indices);
#endif // !PARSER_H
