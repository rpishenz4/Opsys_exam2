/* count-shm.c */

#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/* This constant defines the shared memory segment such that
   multiple processes can attach to this segment */
#define SHM_SHARED_KEY 8999

void first_program(){
    key_t key = SHM_SHARED_KEY;
    int shmid = shmget( key, 1024 , IPC_CREAT | /*IPC_EXCL |*/ 0660 );

    if ( shmid == -1 )
    {
        perror( "shmget() failed" );
    }
    printf("First program completed\n");

}

void second_program(){
    int shmid = shmget( SHM_SHARED_KEY, 1024 , IPC_EXCL );

    if ( shmid == -1 )
    {
        perror( "shmget() failed" );
    }
    unsigned long * data = shmat( shmid, NULL, 0 );
    if ( data == (unsigned long *)-1 )
    {
        perror( "shmat() failed" );
    }
    *data = 0;
    *(data+1) = 1;
    for (int i = 2; i<128; i++){
        *(data+i) = *(data+i-1) + *(data+i-2);
    }
    printf("Second Program completed \n");
    
    
    
}
void third_program(int argc, char** argv){
    int shmid = shmget( SHM_SHARED_KEY, 1024 , IPC_EXCL );

    if ( shmid == -1 )
    {
        perror( "shmget() failed" );
    }
    unsigned long * data = shmat( shmid, NULL, 0 );
    if ( data == (unsigned long *)-1 )
    {
        perror( "shmat() failed" );
    }
    for (int i = 0; i<128; i++){
        printf("%lu ",*(data+i));
    }
    printf("\n");
    if (argc==2 && strcmp(argv[0],"-RM_SHM")){
        int rc = shmdt( data ); 
        printf("check\n");
        if ( rc == -1 )
        {
            perror( "shmdt() failed" );
            exit( EXIT_FAILURE );
        }
        if ( shmctl( shmid, IPC_RMID, 0 ) == -1 )
        {
            perror( "shmctl() failed" );
            exit( EXIT_FAILURE );
        }
        else{
            printf("Delete successful. Program three done\n");
        }

    }
}

int main(int argc, char * argv[])
{
  /* create the shared memory segment with a size of 4 bytes */
  
  /* attach to the shared memory segment */
  first_program();
  second_program();
  third_program(argc, argv);

  return EXIT_SUCCESS;
}
