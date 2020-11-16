//
//  main.cpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/10/31.
//

#include <iostream>
#include "dynamic_parallel_packrat.hpp"
#include "memoize_table.hpp"
#include "JSON_parser.hpp"

int main(int argc, const char * argv[]) {
    global::numof_thread = 4;
    int parsing_order = 1;
    const std::string filename = "dummy_3611967.json";
    
    std::cout << "numof thread : " << global::numof_thread << std::endl;
    JSON_setup(parsing_order);
    
    parse(filename);
    return 0;
}
