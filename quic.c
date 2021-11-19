#include "basicFunctions.h"
#include "recursuveCopyDelete.h"
#include "outputFunctions.h"
#include <limits.h>       //For PATH_MAX
#include <sys/time.h>

# define PERMS 0644 // set access permissions

long long bytes;
int counter;

int main(int argc , char** argv)
{
    unsigned long startingTime=time(NULL);
    //printf ("TIME: %lu\n",startingTime);

    int done=0, verbose=0 , deleted=0 , links=0 ;
    char* origindir;
    char* destdir;
    //execute: ./quic -v -d -l origindir destdir

    if ( (argc >= 2) && (argc <= 6) ) {
        for(int i=1 ; i<argc ; i++) {
            if( strcmp(argv[i],"-v")==0 ) {
                verbose=1;
            }else if ( strcmp(argv[i],"-d")==0 ) {
                deleted=1;
            }else if ( strcmp(argv[i],"-l")==0 ) {
                links=1;
            }else{
                if (done==0){
                    origindir=(char*)malloc(strlen(argv[i])+1);
                    strcpy(origindir,argv[i]);
                    done=1;
                    continue;
                }
                destdir=(char*)malloc(strlen(argv[i])+1);
                strcpy(destdir,argv[i]);
            }
        }
    } else {
        printf("\nToo many arguments passed.\n");
        exit(EXIT_FAILURE);
    }

    struct timeval stop, start;
    gettimeofday(&start, NULL);

    //printInode(origindir);
    //printInode(destdir);

    DIR* dir = opendir(origindir);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
    } else {
        printf("Origin directory does not exist.\n");
        exit(EXIT_FAILURE);
    }


    dir = opendir(destdir);
    if (dir) {
        /* Directory exists. */
        closedir(dir);
    } else if (ENOENT == errno) {
        /* Directory does not exist. */
        if (verbose)
            printf("created directory /%s/\n",destdir);
        mkdir(destdir, 0777);
    } else {
        /* opendir() failed for some other reason. */
        printf("Destination directory has some problem.\n");
        exit(EXIT_FAILURE);
    }

    bytes=0;
    counter=0;
    int sameDisk;
    sameDisk=areOnSameDisk(origindir,destdir);

    // if flag deleted -d was included delete from dest anything that does not exist to origin
    if (deleted)
        deleteNotExistent(origindir, destdir , links ,  0 );

    copyDir( origindir , destdir , deleted , links , verbose , sameDisk, startingTime);

    //get end time and calculate statistics
    gettimeofday(&stop, NULL);
    unsigned long long sec = (stop.tv_sec - start.tv_sec);
    unsigned int milliseconds=getFirst3Digits(stop.tv_usec - start.tv_usec);
    //count speed bytes per seconds
    long double sum = ((long double)sec + (long double)((long double)milliseconds/(long double)1000));
    long double speed= (long double)(bytes)/(long double)sum;
    unsigned long long speedInt=(unsigned long long)speed;
    char str[100];
    sprintf(str,"%.2Lf",speed);
    char* decimals= strchr(str,'.');

    //printing
    printf("\nthere are %d files/directories in the hierarchy\n",countDirectoryEntities(origindir));
    printf("number of entities copied is %d\n",counter);

    printf("copied ");
    printfcomma(bytes);
    printf(" bytes in ");
    printf("%llu,", sec);
    printf("%d sec at ",milliseconds );
    printfcomma(speedInt);
    printf("%s bytes/sec\n", decimals );

    return 0;
}
