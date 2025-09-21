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
/*
    index for get path write
*/
typedef enum{
    mon,tue,wed,thu,
    fri,sat,sun
}path_index_t;

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

#define sign_force_ 1
#define sign_init_ 2
#define sign_normal_ 0
/*
    if sign force == 1 it will force to valid and it will reset all data in file manager 
*/
int fo_load(const char *path,int8_t sign_force){
    if(sign_force == sign_force_){
        // reset file manager
        fo_reset();
        goto valid;
    }
    switch (file_manager.stage_file)
    {
        case LOADED:{
            WARNING("file manager already loaded");
            INFO("If you want to load another file please reset");
            return INVALID_;
        }
        case EDITED:{
            WARNING("Your data in file manager still in edit part (dont save yet)");
            INFO("If you want to load another file please write it or reset");
            return INVALID_;
        }
        case NOTTHING:{
            goto valid;
            break;
        }
        case WRITED :{
            fo_reset();
            goto valid;
            break;
        }
        default:{
            WARNING("Invalid file manager please checking");
            return INVALID_;
        }
    }
    valid:
    FILE* fp = fopen(path, "rb");
    if(!fp){
        WARNING("Cant open your file, please check your file in folder data");
        WARNING("Error path: %s", path);
        return INVALID_;
    }
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if(size <= 0){
        WARNING("File is Empty");
        WARNING("Error path: %s", path);
        return INVALID_;
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
    init file for writing
*/
int fo_init(path_index_t index){
    FILE* fp = NULL;
    fp = fopen(all_path[index], "wb");
    if(!fp){
        WARNING("Cant open file path %s", all_path[index]);
        return INVALID_;
    }
    fwrite(magic_bype,BYPE,3,fp); // 3 is size of magic bype
    fclose(fp);
    return 1;
}

#define WRITE_END 1         // write mode 1
#define WRITE_MIDDLE 2      // write mode 2
#define WRITE_NOT_POINT 3 // write mode 3
/*
    if  sign force == 1 it will force to valid and it will reset all data in file manager
        sign force == 2 is init file
*/
/*
    mode argument:
        1 for write to the end (raw data + buffer data)
        2 for write to the middle (raw data to pointer start + buffer data + pointer end to end raw data)
        3 for write only not point at pointer (raw data to pointer start + pointer end to end raw data)
*/
int fo_write(const char* path,int8_t sign_force,int8_t mode){
    if(sign_force == sign_init_){
        // initailize file
        for(int i = mon; i <= sun; i++){
            if(is_invalid(fo_init(i))) return INVALID_;
        }
        return 0;
    }
    if(is_buffer_data_emtpy || is_raw_data_emtpy){
        emtpy:
        WARNING("raw data or new data to write is" ANSI_COLOR_RED " Empty " ANSI_COLOR_RESET);
        return INVALID_;
    }
    if(sign_force == sign_force_){
        goto valid;
    }
    switch (file_manager.stage_file)
    {
        case NOTTHING:
            goto emtpy;
            break;
        case WRITED:{
            WARNING("File is out of date you already write it");
            return INVALID_;
            break;
        }
        case LOADED:{
            WARNING("Nothing change in your File");
            return INVALID_;
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
                return INVALID_;
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
        case WRITE_NOT_POINT:{
            FILE* fp = fopen(path, "wb");
            if(!fp){
                goto error_open_file;
            }
            size_t len = file_manager.start - file_manager.raw_data.buffer;
            fwrite(file_manager.raw_data.buffer, BYPE, len, fp);
            len = file_manager.raw_data.capacity - len - (file_manager.end - file_manager.start);
            fwrite(file_manager.end, BYPE, len, fp);
            fclose(fp);
            break;
        }
        default:
            WARNING("Invalid file write mode is %d", mode);
            return INVALID_;
            break;
    }
    file_manager.stage_file = WRITED;
    return 0;
}

int fo_show(const char* path){
    FILE* fp = fopen(path, "rb");
    if(!fp){
        WARNING("Can't open file with path: %s", path);
        return INVALID_;
    }
    char c;
    while((c = fgetc(fp)) != EOF) printf("%c", c);
    return 0;
}

void fo_reset_buffer(){
    if(is_buffer_data_emtpy) return;
    free(file_manager.buffer_data.buffer);
    file_manager.buffer_data.capacity = -1;
}

int fi_load_str(const char* payload){
    file_manager.buffer_data.buffer = strdup(payload);
    if(is_buffer_data_emtpy){
        WARNING("Maybe heap is full this error from strdup");
        return INVALID_;
    }
    file_manager.buffer_data.capacity = strlen(payload);
    file_manager.stage_file = EDITED;
    return 0;
}