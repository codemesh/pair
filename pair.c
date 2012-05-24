#define _GNU_SOURCE

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void help_exit(const char* app, const char* msg, int code){
    fprintf(stderr, "%s: %s\n"
        "\tusage: %s --uargs <cmd1 arg1 arg2 ...> --dargs <cmd2 arg1 ...>\n", app, msg, app);
    exit(code);
}

void run(int rd, int wr, int argc, const char** argv){
    close(0);
    close(1);
    dup2(rd, 0);
    dup2(wr, 1);
    fcntl(0, FD_CLOEXEC, 0);
    fcntl(1, FD_CLOEXEC, 0);
    char** exec_argv = (char**)malloc(sizeof(char*) + 1);
    int i = 0;
    for(i = 0; i < argc; ++ i){
        exec_argv[i] = strdup(argv[i]);
    }
    exec_argv[argc] = NULL;
    int err = execvp(argv[0], exec_argv);
    fprintf(stderr, "Failed to exec: %s, err: %s\n", argv[0], strerror(errno));
}
    

int main(int argc, const char** argv){
    if(argc < 5){
        help_exit(argv[0], "not enough arguments", 1);
    }
    int uargc = 0;
    int dargc = 0;
    const char** uargv = NULL;
    const char** dargv = NULL;
    int i = 0;
    #define N 0
    #define U 1
    #define D 2
    int state = N;
    for(i = 0; i < argc; ++ i){
        switch(state){
        case N:
            if(strcmp(argv[i], "--uargs") == 0){
                state = U;
                if(i != argc - 1){
                    uargv = argv + i + 1;
                }
                else{
                    help_exit(argv[0], "not enough u arguments", 1);
                }
            }
            break;
        case U:
            if(strcmp(argv[i], "--dargs") == 0){
                state = D;
                if( i != argc - 1){
                    dargv = argv + i + 1;
                }
                else{
                    help_exit(argv[0], "not enough d arguments", 1);
                }
            }
            else{
                ++ uargc;
            }
            break;
        case D:
            ++ dargc;
            break;
        default:
            break;
        }
    }

    if(uargc == 0){
        help_exit(argv[0], "not enough u arguments", 1);
    }
    if(dargc == 0){
        help_exit(argv[0], "not enough d arguments", 1);
    }
    int forward[2];
    int backward[2];
    int ret = pipe(forward);
    if(ret != 0){
        fprintf(stderr, "Failed to create pipe, err: %s\n", strerror(errno));
        return 1;
    }
    ret = pipe(backward);
    if(ret != 0){
        fprintf(stderr, "Failed to create pipe, err: %s\n", strerror(errno));
        return 1;
    }
    /*
     *         ===forward==>
     * app1                     app2
     *         <==backward==
     */
    ret = fork();
    switch(ret){
    case 0:
        //child
        close(forward[1]);
        close(backward[0]);
        run(forward[0], backward[1], uargc, uargv);
        break;
    case -1:
        fprintf(stderr, "Failed to fork. err: %s\n", strerror(errno));
        break;
    default:
        // parent
        close(forward[0]);
        close(backward[1]);
        run(backward[0], forward[1], dargc, dargv);
        break;
    }
    // Should not reach here
    fprintf(stderr, "Failed to pair apps.\n");
    return 1;
}
        
    
