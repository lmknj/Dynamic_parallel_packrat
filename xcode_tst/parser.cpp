//
//  parser.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#include "parser.hpp"

int Parsing_success(const int start_pos, const std::string funcname, const int value){
    global::memoize_table->write_cell(start_pos, funcname, value);
    
    return parsing_res::SUCCEED;
}

int  Parsing_failure(const int start_pos, const std::string funcname, int& pos){
    global::memoize_table->write_cell(start_pos, funcname, cell_value::FAILURE);
    
    pos = start_pos;
    return parsing_res::FAIL;
}



bool str_match(std::string str, int& pos){
    int start_pos = pos;
    int i = 0;
    
    while(global::input[pos] == str[i]){
        ++pos;
        ++i;
        if(i >= str.length())return true;
    }
    
    pos = start_pos;
    return false;
}


bool char_match(char c, int& pos){
    if(global::input[pos] != c)return false;
    ++pos;
    return true;
}


bool digits_match(int& pos){
    if(char_match('0', pos));
    else if(one_to_nine_match(pos)){
        while(zero_to_nine_match(pos));
    }else return false;

    return true;
}


bool one_to_nine_match(int& pos){
    if(!('1' <= global::input[pos] && global::input[pos] <= '9'))return false;

    ++pos;
    return true;
}

bool zero_to_nine_match(int& pos){
    if(!('0' <= global::input[pos] && global::input[pos] <= '9'))return false;

    ++pos;
    return true;
}

bool digit_or_AtoF(int& pos){
    if(!(
         ('0' <= global::input[pos] && global::input[pos] <= '9') ||
        ('a' <= global::input[pos] && global::input[pos] <= 'f') ||
        ('F' <= global::input[pos] && global::input[pos] <= 'F')
       ))return false;
    
    ++pos;
    return true;

}

bool char_range_match(char lower, char upper, int& pos){
    if(!(lower <= global::input[pos] && global::input[pos] <= upper))return false;

    ++pos;
    return true;

}

