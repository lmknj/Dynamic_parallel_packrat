//
//  thread_manager.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//
#include "dynamic_parallel_packrat.hpp"
#include "thread_manager.hpp"

std::vector<int> local::start_pos;
std::vector<int> local::end_pos;
std::vector<int> local::pos;
std::vector<bool> local::is_block_allocated;
std::vector<int> local::readable_line;

std::vector<std::unordered_map<int, int>>
                local::sellect_order_to_index;

std::vector<std::unordered_map<int, FUNCPTR>>
                local::sellect_order_to_FUNCPTR;
 


int exclusion::numof_idle_thread;
std::queue<std::pair<int, int>> exclusion::unallocated_queue;
std::mutex exclusion::unallocated_queue_mtx;
std::mutex exclusion::numof_idle_thread_mtx;
std::condition_variable exclusion::not_empty;





void set_numof_idle_thread(int val){
    std::lock_guard<std::mutex> lk(exclusion::numof_idle_thread_mtx);
    exclusion::numof_idle_thread = val;
}

void incl_numof_idle_thread(){
    std::lock_guard<std::mutex> lk(exclusion::numof_idle_thread_mtx);
    ++exclusion::numof_idle_thread;
}

void decl_numof_idle_thread(){
    std::lock_guard<std::mutex> lk(exclusion::numof_idle_thread_mtx);
    --exclusion::numof_idle_thread;
}

int waitFor_myBlock(const int thread_id){
    //自分のブロックがない
    if(!local::is_block_allocated[thread_id]){
        block_deq(thread_id);
    }
    
    block_enq(thread_id);
    return 0;
}

int block_deq(const int thread_id){
    {//排他制御開始
        std::unique_lock<std::mutex> lk(exclusion::unallocated_queue_mtx);
        exclusion::not_empty.wait(lk, [&]{
            return exclusion::unallocated_queue.size() >= 1;
        });

        local::start_pos[thread_id] = exclusion::unallocated_queue.front().first;
        local::end_pos[thread_id] = exclusion::unallocated_queue.front().second;
        exclusion::unallocated_queue.pop();
    
    }//排他制御終了
    
    local::pos[thread_id] = local::start_pos[thread_id];
    
    decl_numof_idle_thread();
    return 0;
}


int block_enq(const int producer_id){
    if(local::end_pos[producer_id] - local::pos[producer_id] <
       BLOCK_PARTITION_THRESHOLD)return 0;
    
    //ブロックのストックが十分なら
    //暇なスレッドに全部のブロックを割り当ててもまだ１余るように
    if(exclusion::unallocated_queue.size() >
       exclusion::numof_idle_thread)return 0;
        
    
    
    int end_pos = local::end_pos[producer_id];
    local::end_pos[producer_id] = local::pos[producer_id] +
                    int((local::end_pos[producer_id] -
                            local::pos[producer_id]) / 2);
        
    int start_pos =  local::end_pos[producer_id] + 1;

    {//排他制御
        std::lock_guard<std::mutex> lk(exclusion::unallocated_queue_mtx);
        // 共有データの更新
        exclusion::unallocated_queue.emplace(std::make_pair(start_pos, end_pos));
        exclusion::not_empty.notify_all();
    }//排他制御終了
        
    
    return 0;
}


void init_local(const int numof_thread){
    local::start_pos = {};
    local::end_pos = {};
    local::pos = {};
    local::is_block_allocated = {};
    local::readable_line = {};
    local::sellect_order_to_index = {};

    for(int i = 0; i < numof_thread; ++i){
        local::start_pos.push_back(INITIAL_POS_VAL);
        local::end_pos.push_back(INITIAL_POS_VAL);
        local::pos.push_back(INITIAL_POS_VAL);
        local::is_block_allocated.push_back(false);
        local::readable_line.push_back(parser_info::readable_line);
        local::sellect_order_to_index.push_back({});
        local::sellect_order_to_FUNCPTR.push_back(parser_info::sellect_order_to_FUNCPTR);

        for(int j = 0; j < parser_info::funcnames.size(); ++j){
            //最初は順番通りでよい
            local::sellect_order_to_index[i][j] = j;
        }
        
    }
    bool isThrere_Wthread = (numof_thread >= 2 ) ? ONE_OR_MORE_WORKER_THREAD : NO_WORKER_THREAD ;
    allocate_1st_block(isThrere_Wthread);
    
}

void allocate_1st_block(const int consumer_id){
    if (consumer_id == NO_WORKER_THREAD)return;
    
    
    local::start_pos[1] = int(global::input.length() / global::numof_thread);   //0~CHUNK_SIZEはメインスレッドがやれ
    local::end_pos[1] = int(global::input.length()) - 1;
    local::pos[1] = local::start_pos[1];
    local::is_block_allocated[1] = true;
    
    set_numof_idle_thread(global::numof_thread - 2);
    

}
