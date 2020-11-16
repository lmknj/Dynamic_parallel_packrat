//
//  dynamic_parallel_packrat.hpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/10/31.
//

#ifndef dynamic_parallel_packrat_hpp
#define dynamic_parallel_packrat_hpp

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
#include <condition_variable>
#include <cstdlib>
#include <memory>
#include<queue>
#include <mutex>
#include <chrono>

#include "memoize_table.hpp"

constexpr auto BLOCK_PARTITION_THRESHOLD = 100;

int parse(const std::string& filename);    //generate workerthread and parse
int read_input(const std::string& filename);


int worker_thread(const int thread_id);
int task_enq(const int thread_id);
int parse_myBlock(const int thread_id);
int sellect_cell(const int thread_id);
void next_cell(int& line, const int thread_id);


int main_thread();


void print_result();

namespace global{
    extern std::string input;
    extern int numof_thread;
    extern std::unique_ptr<Memoize_table> memoize_table;
    extern bool is_mainthread_finished;
}

#endif /* dynamic_parallel_packrat_hpp */
