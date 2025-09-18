#include "_include.h"

#define SHOW_TASK 1

int main(int argc,char *argv[]){
    file_manager = INIT_FILE_MANAGER();
    if(argc == SHOW_TASK){
        // run program
        fo_load("../data/mon.bin", 0);
        INFO("%s", file_manager.raw_data.buffer);
        fo_reset();
        fo_write("temp", 0, WRITE_END);
    }else {
        // case handle argv
        printf("Run and End\n");
    }
    return 0;
}