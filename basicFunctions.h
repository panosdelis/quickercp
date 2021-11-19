//
// Created by panos on 19/01/2021.
//

#ifndef PROJECT4_BASICFUNCTIONS_H
#define PROJECT4_BASICFUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

extern long long bytes;
extern int counter;

int areOnSameDisk(char* name, char* name2);
int countDirectoryEntities(char* origindir );

#endif //PROJECT4_BASICFUNCTIONS_H
