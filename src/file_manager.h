#include "_macro.h"
/*
    Stage for update file checking
*/
typedef enum{
    NOTTHING = 0xFF,                // init stage
    LOADED   = 0xF1,                // load file from old file
    EDITED   = 0xF2,                // edit file in program 
    WRITED   = 0xF3                 // delete old file and write new file same name
}stage_file_t;

typedef struct {
    char* buffer;                   // pointer of data (string)
    int32_t capacity;               // size of data
}str_t;

typedef struct {
    str_t raw_data;                 // raw data load from old file (do not edit it's using for referent pointer)
    str_t buffer_data;              // buffer for new data
    stage_file_t stage_file;        // stage of file manager
}FileManager_t;

/*
    initailize file manager
*/
FileManager_t INIT_FILE_MANAGER(){  
    return (FileManager_t){
        .buffer_data = (str_t){
                .buffer = NULL,
                .capacity = -1
            },
        .raw_data = (str_t){
                .buffer = NULL,
                .capacity = -1
            },
        .stage_file = NOTTHING
    };
}

FileManager_t file_manager;         // global file manager

/*
    reset and initailize
*/
int fo_reset(){
    if(file_manager.stage_file == NOTTHING){
        INFO("Dont have anything yet to reset");
        return 1;
    }

    if(file_manager.buffer_data.buffer){
        free(file_manager.buffer_data.buffer);
    }

    if(file_manager.raw_data.buffer){
        free(file_manager.raw_data.buffer);
    }

    file_manager = INIT_FILE_MANAGER();
    return 1;
}

/*
    if sign force == 1 it will force to valid and it will reset all data in file manager 
*/
int fo_load(const char *path,int8_t sign_force){
    if(sign_force){
        // reset file manager
        fo_reset();
        goto valid;
    }
    switch (file_manager.stage_file)
    {
        case LOADED:{
            WARNING("file manager already loaded");
            INFO("If you want to load another file please reset");
            return 0;
        }
        case EDITED:{
            WARNING("Your data in file manager still in edit part (dont save yet)");
            INFO("If you want to load another file please write it or reset");
            return 0;
        }
        case NOTTHING:
        case WRITED :{
            goto valid;
            break;
        }
        default:{
            WARNING("Invalid file manager please checking");
            return 0;
        }
    }
    valid:
    FILE* fp = fopen(path, "rb");
    if(!fp){
        WARNING("Cant open your file, please check your file in folder data");
        WARNING("Error path: %s", path);
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if(size <= 0){
        WARNING("File is Empty");
        WARNING("Error path: %s", path);
        return 0;
    }
    file_manager.raw_data.buffer = (char*)malloc(size +1); // +1 for null (\0) pointer
    file_manager.raw_data.capacity = size;

    fread(file_manager.raw_data.buffer,BYPE,size,fp);
    file_manager.raw_data.buffer[size] = '\0';

    file_manager.stage_file = LOADED;
    fclose(fp);
    return 1;
}