/*
 * CS3600, Spring 2013
 * Project 2 Starter Code
 * (c) 2013 Alan Mislove
 *
 * This program is intended to format your disk file, and should be executed
 * BEFORE any attempt is made to mount your file system.  It will not, however
 * be called before every mount (you will call it manually when you format 
 * your disk file).
 */

#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "3600fs.h"
#include "disk.h"
#include <time.h>
#include <unistd.h>

int get_num_dblocks(int size){
  int fat_plus_db = 101 - size;
  return fat_plus_db * (128 / 129);
}

void myformat(int size) {
  // Do not touch or move this function
  dcreate_connect();

  /* 3600: FILL IN CODE HERE.  YOU SHOULD INITIALIZE ANY ON-DISK
           STRUCTURES TO THEIR INITIAL VALUE, AS YOU ARE FORMATTING
           A BLANK DISK.  YOUR DISK SHOULD BE size BLOCKS IN SIZE. */
  typedef struct vcb_s{
    // description of the disk layout
    int blocksize;
    int de_start;
    int de_length;
    int fat_start;
    int fat_length;
    
    // metadata for the root directory
    uid_t user;
    gid_t group;
    mode_t mode;
    struct timespec access_time;
    struct timespec modify_time;
    struct timespec create_time;
  }vcb;

  
  /* 3600: AN EXAMPLE OF READING/WRITING TO THE DISK IS BELOW - YOU'LL
           WANT TO REPLACE THE CODE BELOW WITH SOMETHING MEANINGFUL. */
  
  // Calculate the number of data blocks
  int num_dblocks = get_num_dblocks(size);

  // set up our Volume Control Block
  vcb myvcb;
  myvcb.blocksize = BLOCKSIZE;
  myvcb.de_start = BLOCKSIZE + 1;
  myvcb.de_length = 100;
  myvcb.fat_start = (BLOCKSIZE * myvcb.de_length) + myvcb.de_start;
  myvcb.fat_length = (int)(num_dblocks / 128);
  myvcb.user = getuid();
  myvcb.group = getgid();

  clock_gettime(CLOCK_REALTIME, &myvcb.access_time);
  clock_gettime(CLOCK_REALTIME, &myvcb.modify_time);
  clock_gettime(CLOCK_REALTIME, &myvcb.create_time);
  
  // copy our VCB to a BLOCKSIZE-d location
  char tmp[BLOCKSIZE];
  memset(tmp, 0, BLOCKSIZE);
  memcpy(tmp, &myvcb, sizeof(vcb));

  // write our VCB to the 0th block
  dwrite(0, tmp);
  
  // Do not touch or move this function
  dunconnect();
}

int main(int argc, char** argv) {
  // Do not touch this function
  if (argc != 2) {
    printf("Invalid number of arguments \n");
    printf("usage: %s diskSizeInBlockSize\n", argv[0]);
    return 1;
  }

  unsigned long size = atoi(argv[1]);
  printf("Formatting the disk with size %lu \n", size);
  myformat(size);
}
