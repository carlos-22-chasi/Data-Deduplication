#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./nonleaf_process <directory_path> <pipe_write_end> \n");
        return 1;
    }
    //TODO(overview): fork the child processes(non-leaf process or leaf process) each associated with items under <directory_path>

    //TODO(step1): get <file_path> <pipe_write_end> of type file descirptor # from argv[]
    char *directory_path = argv[1];
    int pipe_write = atoi(argv[2]); 

    //TODO(step2): malloc buffer for gathering all data transferred from child process as in root_process.c
     char *data = (char*)malloc(4098);
     memset(data,0,4098);

    //TODO(step3): open directory
    // printf("===============\nEntered Nonleaf Process, dirpath: =%s=====\n",directory_path);
    DIR *dir = opendir (directory_path);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }
    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process
    //array size of 10 with each index for each ret pipe
    int read_end[10];
    int index = 0;
    struct dirent *entry;
    // Traverse through all entries
    while((entry = readdir(dir)) != NULL){
         if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..") ){
            continue;
        }
        // create pipe
        int fd[2];
        int ret = pipe(fd); 
        if(ret == -1){
            printf("Error creating pipe...\n");
            exit(-1);
        }
        //get entry path
        char current_entry[1024] = "";
        strcat(current_entry, directory_path);
        strcat(current_entry, "/");
        strcat(current_entry, entry->d_name);

        //fork will connect pipe between child and parent process
        pid_t pid;
        pid = fork();

        // process is parent
        if(pid > 0){ 
            //closes write end for parent
            close(fd[1]);
            //adds read end to array
            read_end[index] = fd[0];
            index++;
        }

        //process is the child
        else{ 
            //closes read end for the child
            close(fd[0]); 
            // if entry is a directory then child process should go non-leaf process with directory path and child's write end as parameters 
            if(entry->d_type == DT_DIR){
                char write_end[50];
                sprintf(write_end, "%d", fd[1]);
                char *args[] = {"./nonleaf_process", current_entry, write_end, NULL};
                execv("./nonleaf_process", args);
            }
            // if entry is a file then child process should go leaf process with file path and child's write end as parameters
            else if(entry->d_type == DT_REG){
                char write_end[16];
                sprintf(write_end, "%d", fd[1]);
                char *args[] = {"./nonleaf_process", current_entry, write_end, NULL};
                execv("./leaf_process", args);
            }   
        } 
        //wait for child process to gather all data before parent continues
        // printf("waiting for child process to terminate\n");
        waitpid(pid, NULL, 0);
        // printf("finished waiting for child, back in dirpath: %s\n", directory_path);   
    }
    closedir(dir);

    //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process

    //reads every pipe into data buffer 
    // printf("dirpath: %s has # of read ends: %d \n", directory_path, index);
    for(int i = 0; i < index; i++){
        // printf("inside for loop reading index: %d\n", i);
        char buf[PATH_MAX] ;
        int nbytes = 0;
        while((nbytes = read(read_end[i], buf, sizeof(char) * 100)) != 0){
                // printf("read %d bytes\n", nbytes);
                strcat(data, buf);
                // printf("going to read child pipe buffer:%s into data\n", buf);
                //need to reseet buf
                memset(buf,0,strlen(buf));
               
            }
        // printf("closing read_end\n");
        close(read_end[i]);
    }

    // printf("going to write pipe buffer: \n%s into parent pipe\n", data);

    //writes data into the pipe constructed for parent process and closes the write end pipe
    write(pipe_write, data, strlen(data));
    close(pipe_write);
    //frees malloced spaced
    free(data);
    // printf("exit in nonleaf_process\n");
    exit(0);
}
