#ifndef _PROCESS_HEADER_
#define _PROCESS_HEADER_

#include "file_manager.h"
#include "_unit_test.h"
#include "_macro.h"

typedef enum{                           // Agument list:  
    INIT_FILE,                          //     [init|-init]
    UNIT_TEST,                          //     [utest|-utest]
    ADD_TASK,                           //     [a|-a]
    DONE_TASK,                          //     [d|-d]
    CLEAN_DAY,                          //     [clean|-clean]
    HELP,                               //     [help|-help]
    SIZE                                // SIZE using for init stage of argument command
}argument_command;


typedef struct{
    argument_command argv;              // enum argument command         
    int8_t days;                        // bits mask for days on week [mon = 1 << 0 ... sun = 1 << 6]
}command_t;

typedef struct{
    union 
    {
        int16_t index;
        const char* str;
    };
}process_payload_t;

command_t command;                      // global command
extern FileManager_t file_manager;      // global file manager



const char* days[] = {
    "mon", "tue", "wed",
    "thu", "fri", "sat",
    "sun"
};
// init command
command_t INIT_COMMAND(){
    return (command_t){
        .argv = SIZE,
        .days = 0
    };
}

/*
    ignore case --[arg] -[arg] 
*/
const char* normalize_arg(const char* arg){
    while(*arg == '-') arg++;
    return arg;
}

/*
    set path bit masks
*/
int setpath(const char* path){
    for (int day = mon; day <= sun; day++) {
        if (EQ_STR(path, days[day])) {
            return day;  
        }
    }
    return INVALID_;
}
/*
    get day index with bit mask
*/
int bitmasks_day(int bit){
    for(int i = mon; i <= sun; i++){
        if(bit & (1 << i)) return i;
    }
    return INVALID_;
}

int process(process_payload_t payload){
    switch(command.argv){
        case HELP:{
            return fo_show("../data/help.bin");
        }
        case INIT_FILE:{
            return fo_write(payload.str,sign_init_,WRITE_END);
        }
        case ADD_TASK:{
            int index_path = bitmasks_day(command.days);
            const char* path = all_path[index_path];
            if(is_invalid(fo_load(path,sign_normal_))){
                return INVALID_;
            }
            size_t len = strlen(payload.str);
            char buffer_[len+2];
            sprintf(buffer_,"\"%s\"", payload.str);
            fi_load_str(buffer_);
            if(is_invalid(fo_write(path,sign_normal_,WRITE_END))){
                fo_reset();
                return INVALID_;
            }
            return 0;
        }
        case DONE_TASK:{
            int index_path = bitmasks_day(command.days);
            const char* path = all_path[index_path];
            if(is_invalid(fo_load(path,sign_normal_))){
                return INVALID_;
            }
            bypass_load_donetask:
            if(file_manager.raw_data.capacity <= 3){ // 3 is size of magic bype
                WARNING("File is Empty");
                return 0;
            } 
            int count = 0;
            file_manager.start = file_manager.raw_data.buffer + 3; // skip magic bype
            file_manager.end = file_manager.start + 1;  // make end ignore first " 
            int64_t count_overflow = 0;
            do{
                while(*(file_manager.end) != '\"'){ 
                    if(*(file_manager.end) == '\0'){
                        not_found:
                        WARNING("Not found out of length index");
                        return INVALID_;
                    }
                    file_manager.end++;
                    count_overflow++;
                }
                count++;
                #ifdef _DEBUG_
                    DEBUG("Cut Process in [%s] on count=%d with index=%d", file_manager.end,count,payload.index);
                #endif
                if(count == payload.index){
                    file_manager.end++;  // +1 for ignore last " of message
                    file_manager.stage_file = EDITED;
                    if(is_invalid(fo_write(path,sign_normal_,WRITE_NOT_POINT))){
                        fo_reset();
                        return INVALID_;
                    }
                    break;
                }
                file_manager.end++; // +1 for point first " of next massage
                count_overflow++;
                if(*(file_manager.end) == '\0') goto not_found;
                file_manager.start = file_manager.end; // set start to first "
                file_manager.end++; // +1 for travel ignore first "
            }while(count_overflow < file_manager.raw_data.capacity);
            return 0;
        }
        case UNIT_TEST:{
            return is_invalid(unit_test());
        }
        case CLEAN_DAY:{
            return fo_init(bitmasks_day(command.days));
        }
        default:{
            return INVALID_;
        }
    }
    return 0;
}

/*
    convert argument get from program to list of command
*/
int convert_argument_to_pipeline(int argc,const char *argv[]){
    process_payload_t process_payload = (process_payload_t){.index = 0,.str = NULL};
    for(uint16_t i = 1; i < argc; i++){
        const char *arg = normalize_arg(argv[i]);
        command = INIT_COMMAND();
        // unit test
        if(EQ_STR(arg,"utest")){     
            command.argv = UNIT_TEST;
            // process
            if(is_invalid(process(process_payload))) return INVALID_;
            SUCCESS("Call utest done");
            continue; // END
        }
        // init file
        if(EQ_STR(arg,"init")){      
            command.argv = INIT_FILE;
            // process
            if(is_invalid(process(process_payload))) return INVALID_;
            SUCCESS("Call init done");
            continue; // END
        }
        // add task
        if(EQ_STR(arg,"a")){
            // [mon..sun] [task message]
            command.argv = ADD_TASK;
            // check incrase index
            if(!NOT_OVERFLOW(i,argc)) goto invalid_a;
            int bitmask = setpath(argv[++i]);
            // check valid day index
            if(is_invalid(bitmask)) goto invalid_a;
            command.days = 1 << bitmask;
            // check incrase index
            if(!NOT_OVERFLOW(i,argc)) goto invalid_a;
            // process
            ++i;
            size_t len = strlen(argv[i]);
            char* temp_str = (char*)malloc(len+1);
            sprintf(temp_str, "%s", argv[i]);
            temp_str[len] = '\0';
            temp_str[strcspn(temp_str,"\n\r")] = '\0';
            process_payload.str = temp_str;
            if(is_invalid(process(process_payload))){
                free(temp_str);
                return INVALID_;
            }
            free(temp_str);
            SUCCESS("Call add task done");
            continue; // END
            invalid_a:
            WARNING("Invalid argument %s", argv[i]);
            INFO("-a [mon..sun] [taskmassage]");
            continue;
        }
        // done task
        if(EQ_STR(arg,"d")){         
            //[mon..sun] [index]
            command.argv = DONE_TASK;
            // check incrase index
            if(!NOT_OVERFLOW(i,argc)) goto invalid_d;
            int bitmask = setpath(argv[++i]);
            // check valid day index
            if(is_invalid(bitmask)) goto invalid_d;
            command.days = 1 << bitmask;
            // check incrase index
            if(!NOT_OVERFLOW(i,argc)) goto invalid_d;
            int index = 0;
            // process
            const char* temp = argv[++i];
            for(int i = 0  ; i < strlen(temp); i++){
                #ifdef _DEBUG_
                    DEBUG("check [%c] is digit ?", temp[i]);
                #endif
                if(!isdigit(temp[i])){
                    WARNING("index should is neural number");
                    goto invalid_d;
                }
            }
            int number = atoi(temp);
            if(temp <= 0){
                WARNING("index should'n negative number");
                continue;
            }
            
            process_payload.index = number;
            if(is_invalid(process(process_payload))) return INVALID_;
            SUCCESS("Call done task done");
            continue; // END
            invalid_d:
            WARNING("Invalid argument %s", argv[i]);
            INFO("-d [mon..sun] [index]");
        }
        // clean task in that day
        if(EQ_STR(arg,"clean")){         
            //[mon..sun]
            command.argv = CLEAN_DAY;
            // check incrase index
            if(!NOT_OVERFLOW(i,argc)) goto invalid_c;
            int bitmask = setpath(argv[++i]);
            // check valid day index
            if(is_invalid(bitmask)) goto invalid_c;
            command.days = 1 << bitmask;

            // process
            if(is_invalid(process(process_payload))) return INVALID_;
            SUCCESS("Call clean done");
            continue; // END
            invalid_c:
            WARNING("Invalid argument %s", argv[i]);
            INFO("-d [mon..sun] [index]");
        }
        // show commmand argument
        if(EQ_STR(arg,"help")){   
            command.argv = HELP;
            // process
            if(is_invalid(process(process_payload))) return INVALID_;
            continue; // END
        }
        invalid_cmd:
        WARNING("Invalid argument %s", argv[i]);
        INFO("please using -help to see detail");
    }
}

char* string_slide(char stop,char** origin){
    char *start = *origin, *temp = NULL, *end = NULL;

    while (*start == stop) {
        if (*start == '\0') {
            return NULL;
        }
        start++;
    }

    end = start;

    while (*end != stop && *end != '\0') {
        end++;
    }
    if(*end == '\0') return NULL;
    size_t len = end - start;
    temp = (char*)malloc(len + 1);
    if (!temp) return NULL;

    strncpy(temp, start, len);
    temp[len] = '\0';

    if (*end == stop)
        *origin = end + 1;
    else
        *origin = NULL;

    return temp;
}

int display_task_unit(int day){
    fo_load(all_path[day],sign_normal_);
    if(file_manager.raw_data.capacity <= 3){ // 3 is size of magic bype
        EMPTY();
        return 0;
    } 
    char *save_ptr = file_manager.raw_data.buffer + 3,*temp = NULL;
    while(save_ptr != NULL){
        temp = string_slide('\"', &save_ptr);
        if(temp == NULL) return 0;
        TASK("%s", temp);
        free(temp);
    }
    return 0;
}

const char* fulldays[] = {
    "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday",
    "Sunday"
};

int display_task(){
    for(int i = mon; i <= sun; i++){
        printf("[%s]\n", fulldays[i]);
        display_task_unit(i);
        fo_reset();
    }
    return 0;
}

#endif