//
//  dynamic_parallel_packrat.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/10/31.
//

#include "dynamic_parallel_packrat.hpp"
#include "thread_manager.hpp"
#include "JSON_parser.hpp"


std::string global::input;
int global::numof_thread;
std::unique_ptr<Memoize_table> global::memoize_table;
bool global::is_mainthread_finished;


int parse(const std::string& filename){
    
    
    read_input(filename);
    
    
    global::memoize_table = std::make_unique<Memoize_table>();
    global::is_mainthread_finished = false;
    
    std::vector<std::thread> worker_threads;
    
    init_local(global::numof_thread);
    
    auto t0 = std::chrono::high_resolution_clock::now();
    
    //わーかスレッド数から１引いたもの
    for(int i = 1; i < global::numof_thread; ++i){
        worker_threads.emplace_back(worker_thread, i);
    }
    
    main_thread();

    auto t1 = std::chrono::high_resolution_clock::now();

    std::cout << "time : "
        << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
            / 1000. << "s."<< std::endl;
    print_result();
    
    //joinできてねえなこれ
    for (std::thread &th : worker_threads)th.join();
    
    
    return 0;
}

int read_input(const std::string& filename){
    //入力の読み込み、バッファに格納
    //失敗で終了→めんどくさいからあとで
    
    
    
     std::ifstream infile(filename);
     std::string line;
     while (std::getline(infile, line)){
         global::input += line;  //解析可能文字列の追加
    }
    std::cout << global::input.length() << std::endl;
    return 0;
}


int worker_thread(const int thread_id){
    while(!global::is_mainthread_finished){
        waitFor_myBlock(thread_id);
        parse_myBlock(thread_id);
    }
    return 0;
}


int parse_myBlock(const int thread_id){
    int line = FIRST_SELLECTION;
    int pos_before_parse = local::pos[thread_id];
    
    while(local::pos[thread_id] <= local::end_pos[thread_id]){
        //解析（内部でめもかひょうチェック）
        switch (local::sellect_order_to_FUNCPTR
                [thread_id][line](local::pos[thread_id])) {//解析
            
            case parsing_res::SUCCEED:
                block_enq(thread_id);//成功でブロック割り当てを確認
            case parsing_res::ALREADY_SUCCEEDED:
                if(pos_before_parse != local::pos[thread_id]){
                    line = FIRST_SELLECTION; //posは更新済みのはず
                    break;
                }
                //０文字マッチはfailと同等
            case parsing_res::FAIL:
                next_cell(line,thread_id);
                break;
        }
        pos_before_parse = local::pos[thread_id];
   
    }//end of WHILE stmt
    
    //while文終わるまでブロックが残っている保証は？
    local::is_block_allocated[thread_id] = false;//ブロック終わり
    
    return 0;
    
}// end of func PAESE_MYBLOCK()

int init_line(){
    //ここでメモ表が割り当て済みかチェック
    
    //解析が進んで解析関数からあくせすすることになったら結局そこでalloc_shunkちゃうんか
    //　⇨じゃあcheck_memoでチェックすれば？
 
    return FIRST_SELLECTION;
}

void next_cell(int& line, const int thread_id){
    if(line < local::readable_line[thread_id]-1)++line;
    else{
        line = FIRST_SELLECTION;
        ++local::pos[thread_id];
    }
}


int main_thread(){
    int pos = 0;
    
    if(!parser_info::start_synbol_funcptr(pos))abort();
    //入力全体とマッチしたかを判定
    if(pos < global::input.length())abort();
    global::is_mainthread_finished = true;
    
    
    return 0;
}



void print_result(){
    //global::memoize_table->print_all();
    std::cout << " global::inoput.length() : "<<global::input.length() << std::endl;
    
    std::cout<<"fin"<<std::endl;
}
