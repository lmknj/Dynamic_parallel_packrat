//
//  memoize_table.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/10/31.
//


#include "dynamic_parallel_packrat.hpp"
#include "memoize_table.hpp"
#include "thread_manager.hpp"


Memoize_table::Memoize_table(){
    numof_column = int(global::input.length());
    numof_line = int(parser_info::funcnames.size());
    
    chunk_address_list.resize(global::input.length()+1, NULL);//NULLはなんか別の要素である必要がある
    
    line_from_funcname = {};
    int line = 0;
    for (const auto& funcname : parser_info::funcnames) {
        line_from_funcname[funcname] = line;  //関数と行の対応
        ++line;
    }
}





int Memoize_table::read_cell(const int pos, const std::string& funcname){
    
    if(chunk_address_list[pos] == NULL)return cell_value::INITIAL_CELL_VALUE;
    
    return *(chunk_address_list[pos] +  get_line_from_funcname(funcname));
}

int Memoize_table::read_cell(const int pos, const int funcline){
    if(chunk_address_list[pos] == NULL)return cell_value::INITIAL_CELL_VALUE;
    return *(chunk_address_list[pos] + funcline);
}


int Memoize_table::read_cell_from_slctOrder(const int thread_id, const int slct_order){
    /*
    if (funcname_from_line.find(funcline) == funcname_from_line.end()) {
           return NOT_REGISTERED;  //解析関数が登録されていない
    }
    */
    
    int pos = local::pos[thread_id];
    if(chunk_address_list[pos] == NULL)return cell_value::INITIAL_CELL_VALUE;
  
    int line =local::sellect_order_to_index[thread_id][slct_order];
    return *(chunk_address_list[pos] + line);
}



int Memoize_table::write_cell(const int& pos, const std::string& funcname, const int value){
    
    if(chunk_address_list[pos] == NULL)alloc_chunk(pos);
    
    
    int* ptr =  chunk_address_list[pos] +  get_line_from_funcname(funcname);
    *ptr = value;
    return 0;
}




int Memoize_table::check_cell(int& pos, const std::string funcname){
    int cell_val = read_cell(pos, funcname);
    switch (cell_val) {
        case cell_value::NOT_REGISTERED:
        case cell_value::INCOLLECT_ACCESS:
            abort();
        case cell_value::INITIAL_CELL_VALUE:
            return parsing_res::YET_PARSE;
        case cell_value::FAILURE:
            return parsing_res::FAIL;
        default:
            pos = cell_val;
            return parsing_res::ALREADY_SUCCEEDED;
    }
}


int Memoize_table::get_line_from_funcname(const std::string funcname){
   return line_from_funcname[funcname];
}

int Memoize_table::get_numof_column(){return numof_column;}
int Memoize_table::get_numof_line(){return numof_line;}

    





//メモリ割り当て処理を統括
void Memoize_table::alloc_chunk(const int pos){

    //割り当て範囲は、start~end-1
    int start = pos;
    int end = start + CHUNK_SIZE;
    
    //割り当て範囲を決定
    if(!searchFor_unallocated_range(start, end))return;//startが割り当てずみ

    
    int chunk_size = end - start;
    int range = NUM_OF_NONTERMINAL * (chunk_size);
 
    int* first_ptr = (int *)malloc(sizeof(int) *  range);
    if(first_ptr == NULL)abort();
    
    //割り当てた領域の値初期化と記録
    for(int i = start; i < end; ++i){//１行
        if(chunk_address_list[i] == NULL){//割り付けが安全ならこの判定いらないはず
            chunk_address_list[i] = first_ptr;
            for(int j = 0; j <NUM_OF_NONTERMINAL; ++j){ //１列
                *first_ptr = -1;
                ++first_ptr;
            }
        }
    }

    allocated_range_push_back(start, end);
}

//alloc_chunkの中で実際に割り当てる領域を決定
bool Memoize_table::searchFor_unallocated_range(const int start, int& end){
    std::lock_guard<std::mutex> lk(std::mutex mallocated_range_mtx);
    //start  ~ end-1が範囲
    //mallocated_rangeに含まれるのはstart~end
    
    //入力サイズ超えないように

    end = (global::input.length() + 1 < CHUNK_SIZE + start) ?
        int(global::input.length() + 1) : CHUNK_SIZE + start;
    

    for(auto&  pair: mallocated_range){
        //startを含むブロックがすでにある
        if(pair.first <= start && start < pair.second)return false;
        if(start < pair.first && pair.first < end)end = pair.first;
    }

    
    return true;

}


//pushback処理
void Memoize_table::allocated_range_push_back(const int start, const int end){
    std::lock_guard<std::mutex> lk(std::mutex mallocated_range_mtx);
    mallocated_range.emplace_back(std::make_pair(start, end));
}


void Memoize_table::print_all(){
    //割り当てた領域リスト、メモか表くらいはまあ・・

    std::cout<< "mallocated_range:" << std::endl;
    std::cout<< "size :" << mallocated_range.size() << std::endl;
    
    print_sorted_mallocated_range();
    /*
    std::cout<< "memoize_table result:" << std::endl;
    for(auto& ptr: chunk_address_list){
        //1列表示
        for(int i = 0; i < NUM_OF_NONTERMINAL; ++i){
            std::cout<< ptr[i] << ",";
        }
        std::cout<<std::endl;
    }
     */
    
}



std::vector <std::pair<int, int>>
Memoize_table::print_sorted_mallocated_range(){
    std::vector<std::pair<int, int>> sorted_range = {};
    
    for(auto& pair : mallocated_range)sorted_range.push_back(pair);

    for(int i = 0; i < sorted_range.size(); i++){
        for(int j = 0; j < sorted_range.size() - 1; j++){
            if(sorted_range[j].first > sorted_range[j+1].first){
                auto tmp = sorted_range[j];
                sorted_range[j] = sorted_range[j+1] ;
                sorted_range[j+1] = tmp;
            }
        }
    }

    
    for(auto& pair : sorted_range){
        std::cout << "<" << pair.first << "," << pair.second << ">" <<std::endl;
    }
    
    
    return sorted_range;
}


