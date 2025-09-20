#include "_include.h"

#define SHOW_TASK 1


int main(int argc,char *argv[]){
    #ifdef _DEBUG_
        INFO("argument count is %d", argc);
        for(uint16_t i = 0; i < argc; i++){
            INFO("[%s]", argv[i]);
        }
    #endif
    file_manager = INIT_FILE_MANAGER();
    if(argc == SHOW_TASK){
        // run program
    }else {
        // case handle argv
    }
    return 0;
}