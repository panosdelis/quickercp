#include "basicFunctions.h"
#include "recursuveCopyDelete.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>


int copyDir(char* origindir , char* destdir , int deleted , int links , int verbose , int sameDisk, unsigned long startingTime){
    int done=0;
    int directoryExists=1;

    if( creationTime(origindir, startingTime) == 0 ){
        printf("\nInfinitive loop detected. EXITING\n");
        exit(EXIT_FAILURE);
    }

    DIR *dr = opendir(origindir); // Pointer for directory entry
    if (dr == NULL){// open returns NULL if couldn't open directory
        perror("\n");
        printf("Could not open current directory\n" );
        return 0;
    }

    DIR *dr2 = opendir(destdir); // Pointer for directory entry

    if (ENOENT == errno) {
        /* Directory does not exist. */
        //printf("Directory does not exist" );
        //directoryExists=0;
        mkdir(destdir, 0777);
        counter++;
        char* word= strrchr(destdir, '/');
        //printf("created directory %s\n",destdir);
        if (verbose) {
            if (word == NULL)
                printf("%s/\n", destdir);
            else
                printf("%s/\n", word + 1);
        }
        dr2 = opendir(destdir);
    } else if (dr == NULL){// open returns NULL if couldn't open directory
        perror("\n");
        printf("Could not open current directory\n" );
        closedir(dr); // close the first opened dir
        return 0;
    }

    struct dirent *nextDir;
    struct dirent *nextDir2;
    struct dirent *self=NULL;
    char theRealPath[PATH_MAX + 1];
    done=0;
    int isDir=0;
    int isLink=0;

    char path[PATH_MAX + 1];
    char path2[PATH_MAX + 1];
    while ((nextDir = readdir(dr)) != NULL) {
         if (strcmp(nextDir->d_name, ".") == 0 || strcmp(nextDir->d_name, "..") == 0)
            continue;
        isDir=0;
        //create the path to dirent
        realpath(origindir, path);
        strcat(path,"/");
        strcat(path,nextDir->d_name);

        if(nextDir->d_type == DT_DIR) {
            //printf("DIR: ");
            isDir=1;
            //todo checks if dir exists in dest , if exists recursive if not
        }
        isLink= (nextDir->d_type == DT_LNK);

        done=0;
        if (dr2 != NULL)
            rewinddir(dr2);
        while ((nextDir2 = readdir(dr2)) != NULL) {
            if (strcmp(nextDir2->d_name, ".") == 0 || strcmp(nextDir2->d_name, "..") == 0)
                continue;

            //create the path to dirent
            realpath(destdir,path2);
            strcat(path2,"/");
            strcat(path2,nextDir2->d_name);

            if(strcmp(nextDir->d_name , nextDir2->d_name)==0 ){
                //if that name exists already
                //printf("Same name");
                //check if one is dir and the other is not
                if (nextDir2->d_type == DT_LNK) {

                    if( (isLink || isDir)  && isSameInode(path,path2) ){
                        //printf("this is a right link.\n");
                    }else {
                        //printf("This is a wrong link fixed it.\n");
                        remove(path2);
                        break;
                    }
                }else if (isDir != (nextDir2->d_type == DT_DIR) ){
                    int isDir2=0;
                    if(nextDir2->d_type == DT_DIR) {
                        //printf("DIR: ");
                        isDir2=1;
                    }

                    if(isLink) {
                        if(isDir2){
                            copyDir(path,path2, deleted , links , verbose , sameDisk, startingTime);
                        }else {
                            remove(path2);
                            break;
                        }
                    }else {
                        if (isDir2)
                            deleteDirectory(path2);
                        remove(path2);
                        break;
                    }
                }else if (isDir){
                    //printf("Same folder.\n");
                    if (!directoryExists) { // it will never get in here because it would have openen the folder
                        mkdir(path2, 0777);
                        counter++;
                    }
                    copyDir(path,path2, deleted , links , verbose , sameDisk, startingTime);
                }else { // else its not directory so both files
                    //check if files have the same size
                    if(nextDir->d_type == DT_LNK) {
                        remove(path2);
                        break;
                    }
                    if ( (getSize(path) != getSize(path2)) || (getLastModificationTime(path) > getLastModificationTime(path2)) ){
                        bytes += OSCopyFile(path,path2);
                        counter++;
                        if (verbose)
                            printf("%s\n",path2);
                    }

                }

                done=1;
                break;
            }
        }
        if(done==0){
            realpath(destdir,path2);
            strcat(path2,"/");
            strcat(path2,nextDir->d_name);

            if ( (isDir)|| isLink){ //if its a directory
                if (links && sameDisk) { // if links are allowed
                    if (symlink(path, path2) == -1)
                        perror("\nlink");
                    if (verbose)
                        printf("%s (soft link)\n",path2);
                    counter++;
//                    if( isSameInode(path,path2) ){
//                        counter--;
//                        remove(path2);
//                    }

                }else{
                    if (!directoryExists) {
                        mkdir(path2, 0644);
                        counter++;
                    }
                    realpath(origindir,theRealPath);
                    if (isSameInode(theRealPath,path)==0)
                        copyDir(path,path2, deleted , links , verbose , sameDisk, startingTime);
                }
            }else{ //if its a common file
                if (links && sameDisk) {
                    if (link(path, path2) == -1)
                        perror("\nlink");
                    if (verbose)
                        printf("%s (hard link)\n",path2);
                    counter++;
                }else{
                    bytes += OSCopyFile(path,path2);
                    counter++;
                    if (verbose)
                        printf("%s\n",path2);
                }
            }
        }
        isDir=0;
    }

    closedir(dr);
    closedir(dr2);
}

int deleteNotExistent(char* origindir, char* destdir , int links , int sameDisk ){
    int done=0;
    int directoryExists=1;

    //todo if its not directory copy to path

    DIR *dr2 = opendir(destdir); // Pointer for directory entry

    if (dr2 == NULL){// open returns NULL if couldn't open directory
        //perror("\n");
        //printf("Could not open current directory\n" );
        return 0;
    }


    DIR *dr = opendir(origindir); // Pointer for directory entry
    if (ENOENT == errno) {
        /* Directory does not exist. */
        deleteDirectory(destdir);
        remove(destdir);
        //directoryExists=0;
        return 1;
    } else if (dr == NULL){// open returns NULL if couldn't open directory
        //perror("\n");
        deleteDirectory(destdir);
        remove(destdir);
        closedir(dr2); // close the first opened dir
        return 0;
    }

    struct dirent *nextDir;
    struct dirent *nextDir2;
    done=0;
    int isDir=0;

    char path[PATH_MAX + 1];
    char path2[PATH_MAX + 1];
    while ((nextDir2 = readdir(dr2)) != NULL) {
        if (strcmp(nextDir2->d_name, ".") == 0 || strcmp(nextDir2->d_name, "..") == 0)
            continue;
        isDir=0;
        //create the path to dirent
        realpath(destdir, path2);
        strcat(path2,"/");
        strcat(path2,nextDir2->d_name);

        if(nextDir2->d_type == DT_DIR) {
            //printf("DIR: ");
            isDir=1;
        }

        done=0;

        if (dr != NULL)
            rewinddir(dr);
        while ((nextDir = readdir(dr)) != NULL) {
            if (strcmp(nextDir->d_name, ".") == 0 || strcmp(nextDir->d_name, "..") == 0)
                continue;

            //create the path to dirent
            realpath(origindir,path);
            strcat(path,"/");
            strcat(path,nextDir->d_name);

            if(strcmp(nextDir->d_name , nextDir2->d_name)==0 ){
                //if that name exists already
                //check if one is dir and the other is not
                if (nextDir2->d_type == DT_LNK){
                    if( isSameInode(path,path2) ){
                        //printf("this is a right link.\n");
                    }else {
                        remove(path2);
                        if (isDir) { //if its a directory
                            if (links && sameDisk) { // if links are allowed
                                if (symlink(path, path2) == -1)
                                    perror("\nlink");
                            }
                        } else { //if its a common file
                            if (links && sameDisk) {
                                if (link(path, path2) == -1)
                                    perror("\nlink");
                            }
                        }
                    }
                }else if (isDir != (nextDir->d_type == DT_DIR) ){
                    if (nextDir2->d_type == DT_DIR) {
                        deleteNotExistent(path, path2 , links , sameDisk);
                    }
                    remove(path2);
                }else if (isDir){
                    deleteNotExistent(path, path2 , links , sameDisk);
                }else { // else its not directory so both files
                    //bοth files with the same name not your job
                }

                done=1;
                break;
            }
            //else{ //name does not exist here we can change it to deep copy.
            //     }
        }
        if (done==0){
            if (nextDir2->d_type == DT_DIR) {
                deleteNotExistent(path, path2 , links , sameDisk);
            }
            remove(path2);
        }
        isDir=0;
    }

    closedir(dr);
    closedir(dr2);
}