#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "wrapper.h"
#include "error.h"
#include "command.h"
#include "tcputil.h"
#include "user.h"

user_t user;
bool isSyncronized = false;

//thread para receber atualizações de arquivos do server
static void *server_listener(void *arg){
    int sockfd = *(int *)arg;

    //read in fssockfd
    int byteCount, bytesLeft, fileSize;
	FILE* ptrfile;
	char dataBuffer[KBYTE];
	
	while(1){
		if (!isSynchronized){
            char buff[MAXLINE];
            char path[256];

            pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
           
            // recebe tamanho do arquivo
            packet_t packet = recv_packet(sockfd);
            long file_size  = strtol(packet.data, NULL, 10);
            
            //recebe nome do arquivo
            packet_t packet = recv_packet(sockfd);
            char* filename = packet.data;

            strcpy(path, userdir);
            strcat(path, "/");
            strncat(path, filename, strlen(filename) + 1);
            
            Pthread_mutex_lock(&m);

            FILE *fileptr = fopen(path, "wb");
            if (fileptr == NULL)
                err_msg("failed to open file");

            if (file_size > 0 ) // criacao / alteracao do arquivo
                while (file_size > 0)
                {
                    packet = recv_packet(sockfd);
                    fwrite(packet.data, sizeof(char), packet.data_length, fileptr);
                    file_size -= packet.data_length;
                }
            else {  // deleção de arquivo
                
            }

            fclose(fileptr);
            Pthread_mutex_unlock(&m);
            fprintf(stdout, "File download complete: %s\n", arg);

            isSyncronized = true;
		}
	}

    return NULL;
}

// TODO:
// Thread do INotify
static void *file_system_listener(void *arg)
{
    int sockfd = *(int *)arg;
    free(arg);
    Pthread_detach(pthread_self());

    // Code for inotify, etc goes here!
    // Monitor local file system events in sync_dir here!
    while(1){
        if (isSyncronized) {
            // ...
        }
    }


    return NULL;
}


int main(int argc, char* argv[argc+1])
{
    if (argc != 5) 
        err_quit("usage: ./myClient <username> <device_id> <hostname/IPaddress> <service/port#>\n");
    
    int cmdsockfd = Tcp_connect(argv[3], argv[4]);
    send_command(cmdsockfd, argv[1]);   // Send username
    send_command(cmdsockfd, argv[2]);   // Send device_id
    send_command(cmdsockfd, "1");       // Send client thread id
    int has_auth = recv_device_auth(cmdsockfd);

    if (!has_auth)
        err_quit("Too many devices connected for user %s", argv[1]);

    user = save_user(argv[1]);
    get_sync_dir(user);
    // TODO: Fix; Not working
    int *fssockfd = Malloc(sizeof(int));
    *fssockfd = Tcp_connect(argv[3], argv[4]);
    send_command(*fssockfd, argv[1]);   // Send username
    send_command(*fssockfd, argv[2]);   // Send device_id
    send_command(*fssockfd, "2");       // Send client thread id
    pthread_t fstid;
    Pthread_create(&fstid, NULL, &file_system_listener, &fssockfd);

    int *servsockfd = Malloc(sizeof(int));
    *servsockfd = Tcp_connect(argv[3], argv[4]);
    send_command(*servsockfd, argv[1]);   // Send username
    send_command(*servsockfd, argv[2]);   // Send device_id
    send_command(*servsockfd, "3");       // Send client thread id
    pthread_t servtid;
    Pthread_create(&servtid, NULL, &server_listener, &servsockfd);

    // Thread para escutar linha de comando
    while (true) {
        char* cmd = read_command();
        parse_command(cmd, cmdsockfd);
        free(cmd);
    }

    return EXIT_SUCCESS;
}