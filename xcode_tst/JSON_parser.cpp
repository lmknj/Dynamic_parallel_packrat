//
//  JSON_parser.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#include "JSON_parser.hpp"
#include "memoize_table.hpp"
#include "dynamic_parallel_packrat.hpp"




/*
 std::unordered_map<int, std::string> parser_info::synbol_sellect_order;
 std::unordered_map<std::string, FUNCPTR> parser_info::funcptr_map;
 */
int parser_info::readable_line;
//std::unordered_map<int, int> parser_info::sellect_order_to_index;
std::unordered_map<int, FUNCPTR> parser_info::sellect_order_to_FUNCPTR;

std::vector<std::string> parser_info::funcnames;
std::vector<FUNCPTR> parser_info::FUNCPTRs;

FUNCPTR parser_info::start_synbol_funcptr;
int parser_info::start_synbol_line;


void JSON_setup(const int parsing_order){
    
    switch (parsing_order) {
        case 0: //全選択
            parser_info::funcnames = {
                "Json","Value","Obj","Member","Arr","Number","Integer","Frac","Exp","Str","Chars","Unescape","Ws"};
            //関数ポインタのセット（funcnamesと同じ順番でないといけない）
            parser_info::FUNCPTRs= {
                &Json,&Value,&Obj,&Member,&Arr,&Number,&Integer,&Frac,&Exp,&Str,&Chars,&Unescape,&Ws};

            parser_info::readable_line = 13;
            parser_info::start_synbol_line = 0;

            break;
        case 1: //Ws,Chars, Value
            parser_info::funcnames = {
                "Ws", "Chars", "Value", "Json","Obj","Member","Arr","Number","Integer","Frac","Exp","Str","Unescape",};
            //関数ポインタのセット（funcnamesと同じ順番でないといけない）
            parser_info::FUNCPTRs= {
               &Ws, &Chars, &Value, &Json,&Obj,&Member,&Arr,&Number,&Integer,&Frac,&Exp,&Str,&Unescape};

            parser_info::readable_line = 3;
            parser_info::start_synbol_line = 3;

            break;

        default:
            abort();
            break;
    }
    
    
    for(int i = 0; i < parser_info::funcnames.size(); ++i){
        parser_info::sellect_order_to_FUNCPTR[i] = parser_info::FUNCPTRs[i];
    }
    parser_info::start_synbol_funcptr = &Json;
    
    }

int Json(int& pos){
    //解析成功したらposはそのまま進める、失敗したら戻す
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;
    
    int start_pos = pos;
    
    Ws(pos);
    if(!Value(pos))goto FAILURE;
    Ws(pos);
    
    return Parsing_success(start_pos, __func__, pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);
}


int Value(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    //順序つき選択
    if(str_match("true", pos));
    else if(str_match("false", pos));
    else if(str_match("null", pos));
    else if(Obj(pos));
    else if(Arr(pos));
    else if(Number(pos));
    else if(Str(pos));
    else goto FAILURE;
    
SUCCESS:
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}
int Obj(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    
    int start_pos = pos;
    
    Ws(pos);
    if(!char_match('{', pos))goto FAILURE;
    Ws(pos);

    {//optional
        if(!Member(pos))goto END_OPTIONAL;
        
        int pos_before_loop = pos;
        while(true){//loop  0 or more
            Ws(pos);
            if(!char_match(',', pos)){
                pos = pos_before_loop;
                break;
            }
            Ws(pos);
            if(!Member(pos)){
                pos = pos_before_loop;
                break;
            }
            pos_before_loop = pos;
        }//END OFloop  0 or more
        
        Ws(pos);
    }//END OF optional
    END_OPTIONAL:
    
    if(!char_match('}', pos))goto FAILURE;
    Ws(pos);
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}


int Member(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    if(!Str(pos))goto FAILURE;
    Ws(pos);
    if(!char_match(':', pos))goto FAILURE;
    Ws(pos);
    if(!Value(pos))goto FAILURE;
    
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}


int Arr(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    //セルの値が-1なら解析、そうでなければ終了
    //解析成功済みで
    if(cell_val != parsing_res::YET_PARSE)return cell_val;
    
    int start_pos = pos;
    
    Ws(pos);
    if(!char_match('[', pos))goto FAILURE;
    Ws(pos);

    {//optional
        if(!Value(pos))goto END_OPTIONAL;
        
        int pos_before_loop = pos;
        
        while(true){//loop  0 or more
            Ws(pos);
            if(!char_match(',', pos)){
                pos = pos_before_loop;
                break;
            }
            Ws(pos);
            if(!Value(pos)){
                pos = pos_before_loop;
                break;
            }
            pos_before_loop = pos;
        }
        //end loop 0 or more
        
        Ws(pos);
    }
    END_OPTIONAL:
    
    if(!char_match(']', pos))goto FAILURE;
    Ws(pos);
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}
int Number(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;
    
    int start_pos = pos;
    
    {//optional
        char_match('-', pos);
    }
    
    if(!Integer(pos))goto FAILURE;
    
    {//optional
       Frac(pos);
    }
    
    {//optional
        Exp(pos);
    }
    
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);
}

int Integer(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;

    //順序つき選択
    
     
    
    
    if(char_match('0', pos));
    else if(one_to_nine_match(pos)){
        while(zero_to_nine_match(pos));
    }else goto FAILURE;
    
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}

int Frac(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    if(!char_match('.', pos))goto FAILURE;
    
    //0-9の１回以上の繰り返し
    if(!zero_to_nine_match(pos))goto FAILURE;
    while(zero_to_nine_match(pos));

    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}
int Exp(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    
    
    if(char_match('e', pos));
    else if(char_match('E', pos));
    else goto FAILURE;
    
    if(char_match('+', pos));
    else if(char_match('-', pos));
    else goto FAILURE;
    
    if(!zero_to_nine_match(pos))goto FAILURE;
    while(zero_to_nine_match(pos));
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}
int Str(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    
    
    if(!char_match('\"', pos))goto FAILURE;

    while(Chars(pos));

    if(!char_match('\"', pos))goto FAILURE;
    
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}
int Chars(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    
    //CHOICE1
    
    {
        if(!char_match('\\', pos))goto CHOICE2;
        
        
        if(char_match('\u0022', pos))goto SUCCESS;
        else if(char_match('\u005C', pos))goto SUCCESS;
        else if(char_match('\u002F', pos))goto SUCCESS;
        else if(char_match('\u0062', pos))goto SUCCESS;
        else if(char_match('\u0066', pos))goto SUCCESS;
        else if(char_match('\u006E', pos))goto SUCCESS;
        else if(char_match('\u0072', pos))goto SUCCESS;
        else if(char_match('\u0074', pos))goto SUCCESS;
        else{
            int stump = pos;
            if(!char_match('\u0075', pos))goto CHOICE2;
                
            for(int i = 0; i < 4; ++i){
                if(!digit_or_AtoF(pos)){
                    pos = stump;
                    goto CHOICE2;
                }
            }
            goto SUCCESS;
        }
    }
    
    CHOICE2:
    {
        if(!Unescape(pos))goto FAILURE;
    }
   
SUCCESS:
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}


int Unescape(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    //順序選択
    if(char_range_match('\u0020', '\u0021', pos));
    else if(char_range_match('\u0023', '\u005B', pos));
    else if(char_range_match('\u005D', '\u007F', pos));
    else goto FAILURE;
    
    return Parsing_success(start_pos, __func__ , pos);
FAILURE:
    return Parsing_failure(start_pos, __func__, pos);

}


int Ws(int& pos){
    int cell_val = global::memoize_table->check_cell(pos, __func__);
    if(cell_val != parsing_res::YET_PARSE)return cell_val;

    int start_pos = pos;
    
    
    while(true){
        if(char_match('\u0020', pos));
        else if(char_match('\u0009', pos));
        else if(char_match('\u000A', pos));
        else if(char_match('\u000D', pos));
        else break;
    }
    
    return Parsing_success(start_pos, __func__ , pos);
}
