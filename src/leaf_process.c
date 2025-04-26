#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/hash.h"
#include "../include/utils.h"

char *output_file_folder = "output/inter_submission/";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: Inter Submission --> ./leaf_process <file_path> 0\n");
        printf("Usage: Final Submission --> ./leaf_process <file_path> <pipe_write_end>\n");
        return -1;
    }


    //TODO(): get <file_path> <pipe_write_end> of type int for file descriptor # from argv[]
    char *file_path = argv[1];
    int pipe_write = atoi(argv[2]); 

    // printf("===============\nEntered leaf Process, filepath: =%s=====\n",file_path);
    //TODO(): create the hash of given file
    
    char result_hash[SHA256_BLOCK_SIZE * 2 + 1];
    hash_data_block(result_hash, file_path);



    //TODO(): construct string write to pipe. The format is "<file_path>|<hash_value>"
    char *buffer = malloc(1000); //3 for null terminator and |
    sprintf(buffer, "%s|%s|", file_path, result_hash);

    if(pipe_write == 0){
        // printf("=====\nLeaf Process: entered pipe write = 0========\n");
        //TODO(inter submission)
        //TODO(overview): create a file in output_file_folder("output/inter_submission/root*") and write the constructed string to the file
        //TODO(step1): extract the file_name from file_path using extract_filename() in utils.c
        char *fname = extract_filename(file_path);
        //TODO(step2): extract the root directory(e.g. root1 or root2 or root3) from file_path using extract_root_directory() in utils.c
        char *root=  extract_root_directory(file_path);
        //TODO(step3): get the location of the new file (e.g. "output/inter_submission/root1" or "output/inter_submission/root2" or "output/inter_submission/root3")
        char new_file[PATH_MAX];
        sprintf(new_file,"%s%s%s",output_file_folder, root, fname);
        //TODO(step4): create and write to file, and then close file
        FILE *fp = fopen(new_file,"w");
        // printf("%s",new_file);
            if (fp == NULL){
                perror("ERROR");
                return -1;
            }
        fprintf(fp,"%s",buffer);
        fclose(fp);
        //TODO(step5): free any arrays that are allocated using malloc!! Free the string returned from extract_root_directory()!! It is allocated using malloc in extract_root_directory()
        free(root);
        
    }else{
        // printf("=====\nLeaf Process: entered pipe write: buffer: %s with length %d========\n",buffer, (int)strlen(buffer));
        //TODO(final submission): write the string to pipe
        write(pipe_write, buffer, strlen(buffer));
        close(pipe_write);
        free(buffer);
        // printf("leaf exiting\n");
        exit(0);

    }
    free(buffer);
    exit(0);
}
