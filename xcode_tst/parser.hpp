//
//  parser.hpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#ifndef parser_hpp
#define parser_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <stdio.h>
#include <thread>

#include "JSON_parser.hpp"
#include "memoize_table.hpp"
#include "dynamic_parallel_packrat.hpp"

namespace parsing_res{
    constexpr auto ALREADY_SUCCEEDED = -2;
    constexpr auto SUCCEED = -3;
    constexpr auto FAIL = 0;
    constexpr auto YET_PARSE = -1;
}



int Parsing_success(const int start_pos, const std::string funcname, const int value);

int  Parsing_failure(const int start_pos, const std::string funcname, int& pos);

bool str_match(std::string str, int&  pos);

bool char_match(char c, int& pos);

bool digits_match(int& pos);
bool one_to_nine_match(int& pos);
bool zero_to_nine_match(int& pos);

bool digit_or_AtoF(int& pos);

bool char_range_match(char lower, char upper, int& pos);






#endif /* parser_hpp */
