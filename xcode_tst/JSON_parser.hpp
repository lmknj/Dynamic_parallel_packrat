//
//  JSON_parser.hpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#ifndef JSON_parser_hpp
#define JSON_parser_hpp


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
#include "parser.hpp"


//添字
constexpr auto JSON     = 0;
constexpr auto VALUE    = 1;
constexpr auto OBJ      = 2;
constexpr auto MEMBER   = 3;
constexpr auto ARR      = 4;
constexpr auto NUMBER   = 5;
constexpr auto INTEGER  = 6;
constexpr auto FRAC     = 7;
constexpr auto EXP      = 8;
constexpr auto STR      = 9;
constexpr auto CHARS    = 10;
constexpr auto UNESCAPE = 11;
constexpr auto WS       = 12;


constexpr auto NUM_OF_NONTERMINAL = 13;

int Json(int& pos);
int Value(int& pos);
int Obj(int& pos);
int Member(int& pos);
int Arr(int& pos);
int Number(int& pos);
int Integer(int& pos);
int Frac(int& pos);
int Exp(int& pos);
int Str(int& pos);
int Chars(int& pos);
int Unescape(int& pos);
int Ws(int& pos);

void JSON_setup(const int parsing_order);



typedef int (*FUNCPTR)(int& pos);
namespace parser_info {
    /*
     extern std::unordered_map<int, std::string> synbol_sellect_order;
     extern std::unordered_map<std::string, FUNCPTR>funcptr_map;

     */
    extern int readable_line;

    extern std::vector<std::string> funcnames;
    extern std::vector<FUNCPTR> FUNCPTRs;

    //extern std::unordered_map<int, int> sellect_order_to_index;
    extern std::unordered_map<int, FUNCPTR> sellect_order_to_FUNCPTR;

    extern FUNCPTR start_synbol_funcptr;
    extern int start_synbol_line;



}

#endif /* JSON_parser_hpp */
