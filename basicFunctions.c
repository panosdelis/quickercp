//
// Created by panos on 19/01/2021.
//
#include "basicFunctions.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>



int isSameInode(char* path , char* path2){
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        perror("Failed to get file inode");
        return -1;
    }
    struct stat statbuf2;

    if (stat(path2, &statbuf2) == -1) {
        perror("Failed to get file inode");
        return -1;
    }

    if( statbuf.st_ino == statbuf2.st_ino)
        return 1;
    return 0;

}

int getSize(char* path){
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        perror("Failed to get file size");
        return -1;
    }else
        return (statbuf.st_size);
}

int getLastModificationTime(char* path){
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        perror("Failed to get file modification time");
        return -1;
    }else
        return (statbuf.st_mtime);
    //TODO check if time can be compares like that
}

int areOnSameDisk(char* name, char* name2){
    struct stat statbuf;
    struct stat statbuf2;

    if (stat(name2, &statbuf2) == -1) {
        perror("Failed to get if on same disk");
        return -1;
    }

    if (stat(name, &statbuf) == -1) {
        perror("Failed to get file status");
        return -1;
    }

    if(statbuf.st_dev == statbuf2.st_dev)
        return 1;
    return 0;
}

int countDirectoryEntities(char* origindir ){
    int count=0;
    DIR *dr = opendir(origindir); // Pointer for directory entry
    if (dr == NULL){// open returns NULL if couldn't open directory
        perror("\n");
        printf("Could not open current directory\n" );
        return 0;
    }

    struct dirent *nextDir;
    int isDir=0;

    char path[PATH_MAX + 1];

    while ((nextDir = readdir(dr)) != NULL) {
        if (strcmp(nextDir->d_name, ".") == 0 || strcmp(nextDir->d_name, "..") == 0)
            continue;
        //create the path to dirent
        realpath(origindir, path);
        strcat(path, "/");
        strcat(path, nextDir->d_name);

        if(nextDir->d_type == DT_DIR) {
            //printf("DIR: ");
            isDir=1;
        }

        if (isDir) { // if it a folder remove all its content then itself
            count+=countDirectoryEntities(path);
            count++;
        }else { // else its not directory so its file files
            count++;
        }
        isDir=0;
    }

    closedir(dr);

    return count;
}


int deleteDirectory(char* origindir ){

    DIR *dr = opendir(origindir); // Pointer for directory entry
    if (dr == NULL){// open returns NULL if couldn't open directory
        perror("\n");
        printf("Could not open current directory\n" );
        return 0;
    }

    struct dirent *nextDir;
    int isDir=0;

    char path[PATH_MAX + 1];

    while ((nextDir = readdir(dr)) != NULL) {
        if (strcmp(nextDir->d_name, ".") == 0 || strcmp(nextDir->d_name, "..") == 0)
            continue;
        //create the path to dirent
        realpath(origindir, path);
        strcat(path, "/");
        strcat(path, nextDir->d_name);

        if(nextDir->d_type == DT_DIR) {
            //printf("DIR: ");
            isDir=1;
        }

        if (isDir) { // if it a folder remove all its content then itself
            deleteDirectory(path);
            remove(path);
        }else { // else its not directory so its file files
            remove(path);
        }
        isDir=0;
    }

    closedir(dr);

}

int OSCopyFile(const char* source, const char* destination){
    int input, output;
    if ((input = open(source, O_RDONLY)) == -1)
    {
        return -1;
    }
    if ((output = creat(destination, 0660)) == -1)
    {
        close(input);
        return -1;
    }

    //Here we use kernel-space copying for performance reasons
    off_t bytesCopied = 0;
    struct stat fileinfo = {0};
    fstat(input, &fileinfo);
    int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);


    close(input);
    close(output);

    return result;
}

