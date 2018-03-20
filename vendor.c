#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define NUM_CONSUMER 5
#define NUM_SUPPLIER 5
#define MAX_NAME 256

void sigint_handler(int sig)
{
    //system("clear");
    fprintf(stderr, "\e[1mGood Bye 😃\n");
    exit(0);
}

int main(int argc, char **argv)
{
    char consumer_name[NUM_CONSUMER][MAX_NAME];
    int consumer_interval[NUM_CONSUMER];
    int consumer_repeat[NUM_CONSUMER];

    char supplier_name[NUM_SUPPLIER][MAX_NAME];
    int supplier_interval[NUM_SUPPLIER];
    int supplier_repeat[NUM_SUPPLIER];


    FILE *fp;
    int i;

    signal(SIGINT, sigint_handler);
    //signal(SIGSEGV,sigsegv_handler);

    system("clear");
    fprintf(stderr, "\x1B[32m========Vending Machine=========\n\x1B[0m");

    for(i=0;i<NUM_CONSUMER;i++) {
        char filename[] = "ConsumerX.txt";
        filename[8] = '1'+i;
        fp = fopen(filename, "r");
        if(!fp) {
            fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open failed : %s \x1B[0m\e[0m\n",filename);
            exit(1);
        } else {
            fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open successful : %s \x1B[0m\e[0m",filename);
        }

        fscanf(fp,"%s %d %d",consumer_name[i],&consumer_interval[i],&consumer_repeat[i]);
        printf("\t%s\t%d\t%d\n",consumer_name[i],consumer_interval[i],consumer_repeat[i]);
        fclose(fp);
    }

    for(i=0;i<NUM_SUPPLIER;i++) {
        char filename[] = "SupplierX.txt";
        filename[8] = '1'+i;
        fp = fopen(filename, "r");
        if(!fp) {
            fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open failed : %s \x1B[0m\e[0m\n",filename);
            exit(1);
        } else {
            fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open successful : %s \x1B[0m\e[0m",filename);
        }

        fscanf(fp,"%s %d %d",supplier_name[i],&supplier_interval[i],&supplier_repeat[i]);
        printf("\t%s\t%d\t%d\n",supplier_name[i],supplier_interval[i],supplier_repeat[i]);
        fclose(fp);
    }

    while(1) {

    }

    exit(0);
}