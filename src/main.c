#include "_include.h"

#define SHOW_TASK 1


int main(int argc,char *argv[]){
    #ifdef _DEBUG_
        DEBUG("argument count is %d", argc);
        for(uint16_t i = 0; i < argc; i++){
            DEBUG("[%s]", argv[i]);
        }
    #endif
    file_manager = INIT_FILE_MANAGER();
    if(argc == SHOW_TASK){
        // run program
        display_task();
    }else {
        // case handle argv
        if(is_invalid(convert_argument_to_pipeline(argc,(const char**)argv))){
            WARNING("it have error program will shutdown");
            return -1;
        }
    }
    fo_reset();
    return 0;
}