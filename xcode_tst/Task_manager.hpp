//
//  Task_manager.hpp
//  xcode_tst
//
//  Created by 吉田昂太 on 2020/11/05.
//

#ifndef Task_manager_hpp
#define Task_manager_hpp

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

class Task_manager{
    
public:
    Task_manager(const int numof_thread);
    };



#endif /* Task_manager_hpp */
