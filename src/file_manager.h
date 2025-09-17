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
    str_t buffer_data;                   // buffer for new data
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