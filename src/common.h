#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>

#define PERROR(...) {fprintf(stderr,"[ERROR]: ");fprintf(stderr,__VA_ARGS__);fprintf(stderr,"\n");exit(1);}


#endif