//
//  Task_manager.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#include "Task_manager.hpp"


/*
 std::vector<int>task_queue;
 std::vector<std::condition_variable> CVs;
 //cvでタスク持ってるかどうか判定できるっしょ
 std::vector<std::pair<int, int>> block_list;

 */

Task_manager::Task_manager(const int numof_thread){
    task_queue = {};
    CVs = {};
    std::vector<std::pair<int, int>> block_list;
}

bool Task_manager::is_block_already_allocated(const int thread_id){
    return false;
}
int Task_manager::allocate_block(const int thread_id){
    //現在地わからないとダメじゃね？しかも同期必須のやつ
    
    
}

int Task_manager::get_start_pos(const int thread_id){
    
}
int Task_manager::get_end_pos(const int thread_id){
    
}
void Task_manager::set_end_pos(const int thread_id, const int new_pos){
    
}


