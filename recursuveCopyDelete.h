#ifndef PROJECT4_RECURSUVECOPYDELETE_H
#define PROJECT4_RECURSUVECOPYDELETE_H

//in file functions
int getSize(char* path);
int getLastModificationTime(char* path);
int isSameInode(char* path , char* path2);
int OSCopyFile(const char* source, const char* destination);
int copyDir(char* origindir , char* destdir  , int del , int links , int verbose , int sameDisk , unsigned long startingTime);
int deleteNotExistent(char* origindir, char* destdir , int links , int sameDisk );
int creationTime(char* name, unsigned long startingTime);
int deleteDirectory(char* origindir );
#endif //PROJECT4_RECURSUVECOPYDELETE_H
