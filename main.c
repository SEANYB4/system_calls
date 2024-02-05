#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include <sys/inotify.h>

// #include <libnotify/notify.h>

#define EXT_SUCCESS 0
#define EXT_ERR_TOO_FEW_ARGS 1
#define EXT_ERR_INIT_INOTIFY 2
#define EXT_ERR_ADD_WATCH 3
#define EXT_ERR_BASE_PATH_NULL 4
#define EXT_ERR_READ_INOTIFY 5

int IeventQueue = -1;
int IeventStatus = -1;

int main(int argc, char** argv) {


    char *basePath = NULL;
    char *token = NULL;
    char *notificationMessage = NULL;




    char buffer[4096];
    int readLength;


    const struct inotify_event* watchEvent;


    const uint32_t watchMask = IN_CREATE | IN_DELETE | IN_ACCESS | IN_CLOSE_WRITE | IN_MODIFY | IN_MOVE_SELF;

    if(argc < 2) {

        fprintf(stderr, "USAGE: system_calls PATH\n");
        exit(EXT_ERR_TOO_FEW_ARGS);
    }


    basePath = (char *)malloc(sizeof(char)*(strlen(argv[1]) + 1));
    strcpy(basePath, argv[1]);



    token = strtok(basePath, "/");
    while (token != NULL) {

        basePath = token;
        token = strtok(NULL, "/");
    }


    if (basePath == NULL) {
        fprintf(stderr, "Error getting base path!\n");
        exit(EXT_ERR_BASE_PATH_NULL);

    }


    IeventQueue = inotify_init();
    if(IeventQueue == -1) {
        fprintf(stderr, "Error inititalising inotify instance!\n");
        exit(EXT_ERR_INIT_INOTIFY);
    }




    IeventStatus = inotify_add_watch(IeventQueue, argv[1], watchMask);

    if(IeventStatus == -1) {
        fprintf(stderr, "Error adding file to watch instance!\n");
        exit(EXT_ERR_ADD_WATCH);
    }

    while(true) {


        printf("Waiting for ievent...\n");


        readLength = read(IeventQueue, buffer, sizeof(buffer));



        if (readLength == -1) {
 
            fprintf(stderr, "Error reading from inotify instance\n");
            exit(EXT_ERR_READ_INOTIFY);

        }

        for (char *bufferPointer = buffer; bufferPointer < buffer + readLength; bufferPointer += sizeof(struct inotify_event) + watchEvent->len) {



            notificationMessage = NULL;
            watchEvent = (const struct inotify_event *) bufferPointer;


            if (watchEvent->mask & IN_CREATE) {

                notificationMessage = "File created.";

            }

            if(watchEvent->mask & IN_DELETE) {
                notificationMessage = "File deleted.\n";

            }


            if (watchEvent->mask & IN_ACCESS) {
                notificationMessage = "File accessed.\n";
            }


            if (watchEvent->mask & IN_CLOSE_WRITE) {
                notificationMessage = "File written and closed.\n";
            }


            if (watchEvent->mask & IN_MODIFY) {


                notificationMessage = "File Modified\n";
            }


            if (watchEvent->mask & IN_MOVE_SELF) {
                notificationMessage = "File moved.\n";
            }



            if (notificationMessage == NULL) {

                continue;
            }


            printf("%s\n", notificationMessage);

        }


    }

    

    
}