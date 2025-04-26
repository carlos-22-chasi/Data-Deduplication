#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/utils.h"
#include <sys/wait.h>

#define WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define PERM (S_IRUSR | S_IWUSR)
char *output_file_folder = "output/final_submission/";

void redirection(char **dup_list, int size, char* root_dir){
    printf("Entered redirect of root process\n");
    // TODO(overview): redirect standard output to an output file in output_file_folder("output/final_submission/")
    // TODO(step1): determine the filename based on root_dir. e.g. if root_dir is "./root_directories/root1", the output file's name should be "root1.txt"
    char *fname = extract_root_directory(root_dir);
    strcat(fname, ".txt");

    //TODO(step2): redirect standard output to output file (output/final_submission/root*.txt)
    char output_buffer[PATH_MAX];
    sprintf(output_buffer,"output/final_submission/%s",fname);
    // creates output/final_submission/root*.txt file
    int TEMP_STDOUT_FILENO = dup(STDOUT_FILENO);
    int fd2 = open(output_buffer, WRITE, PERM);
    //redirects standart output to fd2
    if(dup2(fd2, STDOUT_FILENO) == -1){
        perror("Failed to redirect output\n");
        exit(-1);
    }
    close(fd2);
    fflush(stdout);

    //TODO(step3): read the content each symbolic link in dup_list, write the path as well as the content of symbolic link to output file(as shown in expected);
    for(int i = 0; i < size; i++){
        char buf[PATH_MAX];
        memset(buf, 0, sizeof(buf));
        readlink(dup_list[i], buf, sizeof(buf));
        printf("[<path of symbolic link> --> <path of retained file>] : [%s --> %s]\n", dup_list[i], buf);
        
    }
    
    if(dup2(TEMP_STDOUT_FILENO , STDOUT_FILENO) == -1){
        perror("Failed to restore output\n");
        exit(-1);
    }
    fflush(stdout);
    close(TEMP_STDOUT_FILENO );
}

void create_symlinks(char **dup_list, char **retain_list, int size) {
    //TODO(): create symbolic link at the location of deleted duplicate file
    //TODO(): dup_list[i] will be the symbolic link for retain_list[i]
    // int symlink(const char *target_file_path, const char *symb_link_path);
    printf("inside create_symlink\n");
    for (int i = 0; i < size; i++){
        // printf("inside create symlink function\n");
        // printf("dup_list[%d] = %s\n", i, dup_list[i]);
        // printf("retain_list[%d] = %s\n", i, retain_list[i]);
        if(symlink(retain_list[i], dup_list[i]) != 0){ // symbolic link
            fprintf(stderr, "Unable to create sym link\n");
            exit(-1);
        }
    }
}

void delete_duplicate_files(char **dup_list, int size) {
    //TODO(): delete duplicate files, each element in dup_list is the path of the duplicate file
    printf("inside deleting duplicate files function\n");
    for (int i = 0; i < size; i++){
        remove(dup_list[i]);
    }
}

// ./root_directories <directory>
int main(int argc, char* argv[]) {
    printf("=====================\n");
    printf("entered root process\n");
    if (argc != 2) {
        // dir is the root_directories directory to start with
        // e.g. ./root_directories/root1
        printf("Usage: ./root <dir> \n");
        return 1;
    }

    //TODO(overview): fork the first non_leaf process associated with root directory("./root_directories/root*")

    char* root_directory = argv[1];
    char all_filepath_hashvalue[4098]; //buffer for gathering all data transferred from child process
    memset(all_filepath_hashvalue, 0, sizeof(all_filepath_hashvalue));// clean the buffer

    //TODO(step1): construct pipe
    int fd[2];
    int ret = pipe(fd); 
    if(ret == -1){
        printf("Error creating pipe...\n");
        exit(-1);
    }
    //TODO(step2): fork() child process & read data from pipe to all_filepath_hashvalue
    pid_t pid;
    pid = fork();

    if(pid == 0){
        close(fd[0]);
        char write_end[16];
        sprintf(write_end, "%d", fd[1]);
        char *args[] = {"./nonleaf_process", root_directory, write_end, NULL};
        execv("./nonleaf_process", args);
        
    }

    if(pid != 0){
    close(fd[1]);
    int nbytes = 0;
    char buf[PATH_MAX];
    while((nbytes = read(fd[0], buf, sizeof(char) * 100)) != 0){
                strcat(all_filepath_hashvalue, buf);
                memset(buf,0,strlen(buf));
            }
    }
    waitpid(pid, NULL, 0);
    printf("back in root_process\n");
    printf("all_filepath_hashvalue: \n%s\n", all_filepath_hashvalue);

    //TODO(step3): malloc dup_list and retain list & use parse_hash() in utils.c to parse all_filepath_hashvalue
    // dup_list: list of paths of duplicate files. We need to delete the files and create symbolic links at the location
    // retain_list: list of paths of unique files. We will create symbolic links for those files
    char **dup_list = (char **)malloc(sizeof(char ) * 10 * 128); //not sure about size so I just put 10
    char **retain_list = (char **)malloc(sizeof(char ) * 10 * 128);
    memset(dup_list, 0, sizeof(char ) * 10 * 128);
    memset(retain_list, 0, sizeof(char ) * 10 * 128);
    int size = parse_hash(all_filepath_hashvalue, dup_list, retain_list);
    //TODO(step4): implement the functions
    delete_duplicate_files(dup_list,size);
    create_symlinks(dup_list, retain_list, size);
    printf("root_directory for redirection is: %s\n", root_directory);
    redirection(dup_list, size, root_directory);

    //TODO(step5): free any arrays that are allocated using malloc!!
    printf("freeing dup_list and retain_list\n");
    // for(int i = 0; i < size; i++){
    //     free(dup_list[i]);
    //     free(retain_list[i]);
    // }
    free(dup_list);
    free(retain_list);
    dup_list = NULL;
    retain_list = NULL;
}
