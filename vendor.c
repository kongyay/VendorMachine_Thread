#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_CONSUMER 2
#define NUM_SUPPLIER 2
#define MAX_NAME 256
#define MAX_PRODUCT 100

int stock[NUM_SUPPLIER];
char stock_name[NUM_SUPPLIER][MAX_NAME];

// for thread's function parameter
struct Param {
    int i;
};

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* supplier_activity(void* params) {
    struct Param *param = (struct Param*)params;
    char name[MAX_NAME];
    int interval;
    int current_interval;
    int repeat;
    int current_repeat = 0;
    int current_increase = 1;
    int i = param->i;
    FILE *fp;

    // Read file
    char filename[] = "SupplierX.txt";
    filename[8] = '1'+i;
    fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open failed : %s \x1B[0m\e[0m\n",filename);
        exit(1);
    } else {
        fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open successful : %s \x1B[0m\e[0m\n",filename);
    }
    fgets(name,MAX_NAME,fp);
    name[strlen(name)-2] = '\0'; // remove \n
    strcpy(stock_name[i],name);
    fscanf(fp,"%d %d",&interval,&repeat);
    fprintf(stderr,"\t%s\tInterval:%d\tRepeat:%d\n",name,interval,repeat);
    current_interval = interval; // set start interval = default
    fclose(fp);

    // Work Loop
    while(1) {
        // DateTime Getter
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char dateStr[64];
        strftime(dateStr, sizeof(dateStr), "%c", tm);

        if(stock[i] < MAX_PRODUCT) { // Not full
            pthread_mutex_lock(&mutex);
            stock[i]++;
            printf("++++++++ %s || %s supplied 1 unit. stock after = %d.....\n",dateStr,name,stock[i]);
            pthread_mutex_unlock(&mutex);
            // Restart interval & repeat
            current_interval = interval;
            current_repeat = 0;

        } else { // Full
            if((++current_repeat)%repeat == 0) { // Wait x times until it reach repeat variable
                current_repeat = 0;
                current_interval = interval * (++current_increase); // Increase interval
                if(current_interval>60) // Max cap = 60s.
                    current_interval = 60;
            }
            printf("\t %s || %s supplier going to wait (%d s./%d rnd.).....\n",dateStr,name,current_interval,current_repeat);
        }
        fflush(stdout);
        sleep(current_interval);
    }
    exit(0);
}

void* consumer_activity(void* params) {
    struct Param *param = (struct Param*)params;
    char name[MAX_NAME];
    int interval;
    int current_interval;
    int repeat;
    int current_repeat = 0;
    int current_increase = 1;
    int i=0,j,k = param->i;
    FILE *fp;

    // Read file (same as supplier_activity function)
    char filename[] = "ConsumerX.txt";
    filename[8] = '1'+k;
    fp = fopen(filename, "r");
    if(!fp) {
        fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open failed : %s \x1B[0m\e[0m\n",filename);
        exit(1);
    } else {
        fprintf(stderr, "\x1B[32m\x1B[32m ✓ Open successful : %s \x1B[0m\e[0m\n",filename);
    }
    fgets(name,MAX_NAME,fp);
    
    name[strlen(name)-2] = '\0'; // remove \n
    fscanf(fp,"%d %d",&interval,&repeat);
    current_interval = interval;
    fprintf(stderr,"\t%s\tInterval:%d\tRepeat:%d\n",name,interval,repeat);
    fclose(fp);

    // Loop check if consumer wants to buy one of the available product supplied or not
    for(j=0;j<NUM_SUPPLIER;j++) {
        while(strlen(stock_name[j])<1);
        if(strcmp(name,stock_name[j])==0) { // Available
            i = j;
            j = NUM_SUPPLIER + 1; //break;
        }
        if(j==NUM_SUPPLIER-1) { // Not Available (None of supplier supplies this product)
            fprintf(stderr,"ERROR: No Item To Buy = %s ",name);
            exit(1);
        }   
    }
    
    // Work Loop (same as supplier_activity function)
    while(1) {
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        char dateStr[64];
        strftime(dateStr, sizeof(dateStr), "%c", tm);
        if(stock[i]>0) {
            pthread_mutex_lock(&mutex);
            stock[i]--;
            printf("-------- %s || %s consumed 1 unit. stock after = %d.....\n",dateStr,name,stock[i]);
            pthread_mutex_unlock(&mutex);
            current_interval = interval;
            current_repeat = 0;
        } else {  
            if((++current_repeat)%repeat == 0) {
                current_repeat = 0;
                current_interval = interval * (++current_increase);
                if(current_interval>60)
                    current_interval = 60;
            }
            printf("\t %s || %s consumer going to wait (%d s./%d rnd.).....\n",dateStr,name,current_interval,current_repeat);
        }
        fflush(stdout);
        sleep(current_interval);
    }
    exit(0);
}

void sigint_handler(int sig)
{
    // Ctrl+C interrupt
    fprintf(stderr, "\e[1mGood Bye 😃\n");
    exit(0);
}

int main(int argc, char **argv)
{
    struct Param consumer_param[NUM_CONSUMER];
    struct Param supplier_param[NUM_SUPPLIER];

    pthread_t consumer_threads[NUM_CONSUMER];
    pthread_t supplier_threads[NUM_SUPPLIER];

    FILE *fp;
    int i;

    signal(SIGINT, sigint_handler);

    system("clear");
    fprintf(stderr, "\x1B[32m========Vending Machine=========\n\x1B[0m");

    for(i=0;i<NUM_SUPPLIER;i++) {
        supplier_param[i].i = i;
        pthread_create(&supplier_threads[i], NULL, supplier_activity, (void *)&supplier_param[i]);
    }

    for(i=0;i<NUM_CONSUMER;i++) {
        consumer_param[i].i = i;
        pthread_create(&consumer_threads[i], NULL, consumer_activity, (void *)&consumer_param[i]);
    }

    while(1); // Loop forever until Ctrl+C interrupt

    exit(0);
}