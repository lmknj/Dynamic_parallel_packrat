//
//  thread_manager.hpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#ifndef thread_manager_hpp
#define thread_manager_hpp
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
#include <mutex>
#include <utility>
#include <queue>

#include "parser.hpp"


constexpr auto NO_WORKER_THREAD = 0;
constexpr auto ONE_OR_MORE_WORKER_THREAD = 1;


constexpr auto CHUNK_SIZE= 10000;

constexpr auto FIRST_SELLECTION = 0;


constexpr auto INITIAL_POS_VAL = -1;


void set_numof_idle_thread(int val);
void incl_numof_idle_thread();
void decl_numof_idle_thread();

int waitFor_myBlock(const int thread_id);
int block_deq(const int thread_id);
int block_enq(const int producer_id);

void init_local(const int numof_tread);
void allocate_1st_block(const int consumer_id);


namespace exclusion{

    extern std::queue<std::pair<int, int>>
                            unallocated_queue;

    extern int numof_idle_thread;
    extern std::mutex unallocated_queue_mtx;
    extern std::mutex numof_idle_thread_mtx;
    extern std::condition_variable not_empty;





}


namespace local{
    extern std::vector<int> start_pos;
    extern std::vector<int> end_pos;
    extern std::vector<int> pos;
    extern std::vector<bool> is_block_allocated;

    extern std::vector<int> readable_line;
    extern std::vector<std::unordered_map<int, int>> sellect_order_to_index;
    extern std::vector<std::unordered_map<int, FUNCPTR>>sellect_order_to_FUNCPTR;
}




#endif /* thread_manager_hpp */
