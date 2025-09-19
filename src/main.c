#include "_include.h"

#define SHOW_TASK 1

int unit_test(){
    UTEST("Unit test run");
    // test init
    fo_write("test",sign_init_,WRITE_END);

    // test write end
    UTEST("Write to the end");
    fo_load(all_path[mon], sign_normal_);
    file_manager.buffer_data.buffer = strdup("\"do something\"");
    file_manager.buffer_data.capacity = strlen(file_manager.buffer_data.buffer);
    file_manager.stage_file = EDITED;
    if(fo_write(all_path[mon],sign_normal_,WRITE_END)) SUCCESS("Write to the end");
    // reset data
    fo_reset();

    // test write in the middle
    UTEST("Write to the middle");
    fo_load(all_path[mon],sign_normal_);
    file_manager.buffer_data.buffer = strdup("\"dont do anything\"");
    file_manager.buffer_data.capacity = strlen(file_manager.buffer_data.buffer);
    file_manager.stage_file = EDITED;
    file_manager.start = file_manager.raw_data.buffer + 3;
    file_manager.end = file_manager.start;
    if(fo_write(all_path[mon],sign_normal_,WRITE_MIDDLE)) SUCCESS("Write to the middle");

    // test invalid already write
    UTEST("Invalid already write case");
    if(!fo_write(all_path[mon],sign_normal_,WRITE_MIDDLE)) SUCCESS("Invalid already write case");

    // reset data
    fo_reset();

    // test invalid data empty
    UTEST("Invalid data empty case");
    if(!fo_write(all_path[mon],sign_normal_,WRITE_END)) SUCCESS("Invalid data empty case");

    // test nothing change
    UTEST("Invalid nothing change case");
    fo_load(all_path[mon],sign_normal_);
    if(!fo_write(all_path[mon],sign_normal_,WRITE_END)) SUCCESS("Invalid nothing change case");

    return 1;
}

int main(int argc,char *argv[]){
    file_manager = INIT_FILE_MANAGER();
    if(argc == SHOW_TASK){
        // run program
    }else {
        // case handle argv
        for(uint16_t i = 0; i < argc; i++){
            if(strncmp(argv[i],"-utest",6) == 0){
                unit_test();
            }
            if(strncmp(argv[i],"init",4) == 0){
                fo_write("",sign_init_,WRITE_END);
            }
        }
    }
    return 0;
}