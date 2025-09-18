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
    char *start,*end;               // pointer working with raw data
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
        .stage_file = NOTTHING,
        .start = NULL,
        .end = NULL
    };
}

FileManager_t file_manager;         // global file manager
const char * all_path[] = {         // global file path
    "../data/mon.bin",
    "../data/tue.bin",
    "../data/wed.bin",
    "../data/thu.bin",
    "../data/fri.bin",
    "../data/sat.bin",
    "../data/sun.bin"
};
const char* magic_bype = "\"*\"";
#define CAPACITY_FILE_PATH 7

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

#define is_raw_data_emtpy file_manager.raw_data.buffer ? 0 : 1
#define is_buffer_data_emtpy file_manager.buffer_data.buffer ? 0 : 1

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

/*
    if  sign force == 1 it will force to valid and it will reset all data in file manager
        sign force == 2 is init file
*/
/*
    mode argument:
        1 for write to the end (raw data + buffer data)
        2 for write to the middle (raw data to pointer start + buffer data + pointer end to end raw data)
*/
#define WRITE_END 1
#define WRITE_MIDDLE 2
int fo_write(const char* path,int8_t sign_force,int8_t mode){ // TODO: test this function
    if(sign_force == 2){
        // initailize file
        FILE* fp = NULL;
        for(int i = 0; i < CAPACITY_FILE_PATH; i++){
            fp = fopen(all_path[i], "wb");
            fwrite(magic_bype,BYPE,3,fp); // 3 is size of magic bype
            fclose(fp);
        }
        return 1;
    }
    if(is_buffer_data_emtpy || is_raw_data_emtpy){
        emtpy:
        WARNING("raw data or new data to write is" ANSI_COLOR_RED " Empty " ANSI_COLOR_RESET);
        return 0;
    }
    if(sign_force == 1){
        goto valid;
    }
    switch (file_manager.stage_file)
    {
        case NOTTHING:
            goto emtpy;
            break;
        case WRITED:{
            WARNING("File is out of date you already write it");
            return 0;
            break;
        }
        case LOADED:{
            WARNING("Nothing change in your File");
            return 0;
            break;
        }
        case EDITED:
            goto valid;
            break;
        default:
            break;
    }
    valid:
    switch (mode)
    {
        case WRITE_END:{
            FILE* fp = fopen(path, "wb");
            if(!fp){
                error_open_file:
                WARNING("Can't open file with path: %s", path);
                return 0;
            }
            fwrite(file_manager.raw_data.buffer, BYPE,
                    file_manager.raw_data.capacity, fp);
            fwrite(file_manager.buffer_data.buffer, BYPE,
                    file_manager.buffer_data.capacity, fp);
            fclose(fp);
            break;
        }
        case WRITE_MIDDLE:{
            FILE* fp = fopen(path, "wb");
            if(!fp){
                goto error_open_file;
            }
            size_t len = file_manager.start - file_manager.raw_data.buffer;
            fwrite(file_manager.raw_data.buffer, BYPE, len, fp);
            fwrite(file_manager.buffer_data.buffer, BYPE,
                    file_manager.buffer_data.capacity, fp);
            // this make sure pointer of end and start is in between file_manger.raw_data.buffer
            // in function manager this both pointer should handle it
            len = file_manager.raw_data.capacity - len - (file_manager.end - file_manager.start);
            fwrite(file_manager.end, BYPE, len, fp);
            fclose(fp);
            break;
        }
        default:
            WARNING("Invalid file write mode is %d", mode);
            return 0;
            break;
    }
    return 1;
}
