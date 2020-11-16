//
//  memoize_table.hpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/10/31.
//

#ifndef memoize_table_hpp
#define memoize_table_hpp

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cassert>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <thread>
#include <list>
#include <algorithm>

namespace cell_value{
    constexpr auto FAILURE = -2;
    constexpr auto INITIAL_CELL_VALUE = -1;
    constexpr auto NOT_REGISTERED = -100;
    constexpr auto INCOLLECT_ACCESS = -150;
}


class Memoize_table{
    std::vector<int*> chunk_address_list;
    
    std::unordered_map<std::string, int> line_from_funcname;
    int numof_line;
    int numof_column;
    
        
    std::list<std::pair<int, int>>mallocated_range;
    //↑２つは同じ添字で対応させとけ
    //てことはこの２つへの追加は排他制御必要ですよね？⇨うん

    std::mutex mallocated_range_mtx;
  
public:
    Memoize_table();
    
    int read_cell(const int pos, const std::string& funcname);
    int read_cell(const int pos, const int funcline);
    int read_cell_from_slctOrder(const int thread_id,
                                    const int sellectOrder);
    
    int write_cell(const int& pos, const std::string& funcname, const int value);
    
    int check_cell(int& pos, const std::string funcname);
    int get_line_from_funcname(const std::string funcname);
    
    int get_numof_column();
    int get_numof_line();
    
    void print_all();
    
private:
    //メモリ割り当て処理を統括
    void alloc_chunk(const int pos);

    //alloc_chunkの中で実際に割り当てる領域を決定
    bool searchFor_unallocated_range(const int start, int& end);

    //上を受けて必要範囲を割り付ける
    void set_ptr(const int start, const int end);
    
    //pushback処理
    void allocated_range_push_back(const int start, const int end);
    
    std::vector<std::pair<int, int>> print_sorted_mallocated_range();
   
};



#endif /* memoize_table_hpp */
