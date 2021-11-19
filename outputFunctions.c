#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <time.h>

unsigned int getFirst3Digits(unsigned long long input)
{
    unsigned long long local = input;

    while (local >= 1000)
    {
        local /= 10;
    }

    return (int)local;
}

int creationTime(char* name, unsigned long startingTime){
    struct stat st;

    if( stat(name, &st) != 0 )
        perror(name);

    if( startingTime <= st.st_ctime )
        return 0;
    return 1;
}

void printfcomma2 (unsigned long long n) {
    if (n < 1000) {
        printf ("%lld", n);
        return;
    }
    printfcomma2 (n/1000);
    printf (",%03lld", n%1000);
}

void printfcomma (unsigned long long n) {
    if (n < 0) {
        printf ("-");
        n = -n;
    }
    printfcomma2 (n);
}

int printInode(char* path){
    struct stat statbuf;

    if (stat(path, &statbuf) == -1) {
        perror("Failed to get file status");
        return -1;
    }else {
        printf("Time/Date  : %s",ctime(&statbuf.st_atime));
        printf("---------------------------------\n");
        printf("disk :%lu\n",statbuf.st_dev);
        printf("inode number:%lu\n",statbuf.st_ino);
        printf("entity path: %s\n", path);
        printf("accessed   : %s", ctime(&statbuf.st_atime));
        printf("modified   : %s", ctime(&statbuf.st_mtime));
    }

    return(1);
}