#include "_include.h"

#define PAYLORD 1

int main(int argc,char *argv[]){
    if(argc == PAYLORD){
        // run program
        printf("Paylord\n");
        file_manager = INIT_FILE_MANAGER();
        printf("%d %d %d", file_manager.buffer_data.buffer == NULL ? 0:1,
            file_manager.raw_data.buffer == NULL ? 0:1,
            file_manager.stage_file
        );
    }else {
        // case handle argv
        printf("Run and End\n");
    }
    return 0;
}