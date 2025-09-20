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

command_t command;                      // global command
extern const char * all_path[];         // global path file
extern FileManager_t file_manager;      // global file manager

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
        if (EQ_STR(path, all_path[day])) {
            return day;  
        }
    }
    return INVALID_;
}

/*
    convert argument get from program to list of command
*/
int convert_argument_to_pipeline(int argc,const char *argv[]){
    for(uint16_t i = 0; i < argc; i++){
        const char *arg = normalize_arg(argv[i]);
        command = INIT_COMMAND();
        // unit test
        if(EQ_STR(arg,"utest")){     
            command.argv = UNIT_TEST;
            // process
            continue; // END
        }
        // init file
        if(EQ_STR(arg,"init")){      
            command.argv = INIT_FILE;
            // process
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

            continue; // END
            invalid_a:
            WARNING("Invalid argument");
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

            continue; // END
            invalid_d:
            WARNING("Invalid argument");
            INFO("-d [mon..sun] [index]");
        }
        // clean task in that day
        if(EQ_STR(arg,"c")){         
            //[mon..sun]
            command.argv = CLEAN_DAY;
            // check incrase index
            if(!NOT_OVERFLOW(i,argc)) goto invalid_c;
            int bitmask = setpath(argv[++i]);
            // check valid day index
            if(is_invalid(bitmask)) goto invalid_c;
            command.days = 1 << bitmask;

            // process

            continue; // END
            invalid_c:
            WARNING("Invalid argument");
            INFO("-d [mon..sun] [index]");
        }
        // show commmand argument
        if(EQ_STR(arg,"help")){   
            command.argv = HELP;
            // process
            continue; // END
        }
        invalid_cmd:
        WARNING("Invalid argument");
        INFO("please using -help to see detail");
    }
}

// TODO: impiment process fucntion

#endif