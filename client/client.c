#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#define DATA_BUFFER 300
const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;
char username[DATA_BUFFER] = {0};
char type[DATA_BUFFER] = {0};
bool wait = false;
void *handleInput(void *client_sockets);
void *checkOutput(void *client_sockets);
void serverOutput(int fd, char *input);
 


int createSocket(){

    struct sockaddr_in saddr;
    int fd, ret_val;
    int opt = 1;
    struct hostent *local_host;
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fd == -1){
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    printf("Created a socket with fd: %d\n", fd); 
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7000);
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) 
    {
        fprintf(stderr, "connect failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}

//void storeToLogs(char *username, char *command)
//{
//    time(&my_time);
//	timeinfo = localtime(&my_time);
//
//	char day[10], month[10], year[10], hour[10], minute[10], second[10];
//
//	sprintf(day, "%d", timeinfo->tm_mday);
//	if(timeinfo->tm_mday < 10)
//		sprintf(day, "0%d", timeinfo->tm_mday);
//
//	sprintf(month, "%d", timeinfo->tm_mon+1);
//	if(timeinfo->tm_mon+1 < 10)
//		sprintf(month, "0%d", timeinfo->tm_mon+1);
//
//	sprintf(year, "%d", timeinfo->tm_year+1900);
//
//	sprintf(hour, "%d", timeinfo->tm_hour);
//	if(timeinfo->tm_hour < 10)
//		sprintf(hour, "0%d", timeinfo->tm_hour);
//
//	sprintf(minute, "%d", timeinfo->tm_min);
//	if(timeinfo->tm_min < 10)
//		sprintf(minute, "0%d", timeinfo->tm_min);
//
//	sprintf(second, "%d", timeinfo->tm_sec);
//	if(timeinfo->tm_sec < 10)
//		sprintf(second, "0%d", timeinfo->tm_sec);
//
//    char content[arraySize];
//    sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);
//
//    FILE* fptr_logs = fopen(logPath, "a");
//    fprintf(fptr_logs, "%s\r\n", content);
//    fclose(fptr_logs);
//}


bool loginStatus(int fd, int argc, char *argv[]){

    char buf[DATA_BUFFER];
    if (geteuid() == 0) { // root
        write(fd, "LOGIN root root", SIZE_BUFFER);
        puts("LOGIN root root");
 
        strcpy(username, "root");
        strcpy(type, "root");
    } 
    else if (argc == 5
        && strcmp(argv[1] , "-u") == 0
        && strcmp(argv[3] , "-p") == 0
    ) 
    { 
        sprintf(buf, "LOGIN %s %s", argv[2], argv[4]);
        write(fd, buf, SIZE_BUFFER);
        puts(buf);
 
        strcpy(username, argv[2]);
        strcpy(type, "user");
    } 
    else 
    {
        puts("Error::Invalid argument");
        return false;
    }
    read(fd, buf, SIZE_BUFFER);
    puts(buf);
    return strcmp(buf, "Login success\n") == 0;
}
 
char* uppercase(char *str)
{
    int i=0, len=strlen(str);
	char* ret = malloc((len+1) * sizeof(char));
    
    while(str[i])
    {
        ret[i] = toupper(str[i]); i++;
    }
    
    return ret;
}

bool isValid(char *message)
{
    if (strcmp(message, "quit") == 0){
        puts("Good bye :3");
        exit(EXIT_SUCCESS);
    }
    else{
        puts("Invalid query");
        return false;
    }
    return true;
}
 
char *trimStr(char *str)
{
//     if(str == NULL) return NULL;

//     //size_t len = strlen(str);

//     while(isspace(str[len - 1])) --len;
//     while(*str && isspace(*str)) ++str, --len;

//     return strndup(str, len);
}

void *handleInput(void *client_sockets){

    int fd = *(int *) client_sockets;
    char message[DATA_BUFFER] = {0};
 
    while (1) {
        if (wait) continue;
        printf("%s@%s: ", username, type);
        fgets(message, DATA_BUFFER, stdin);
        char *tmp = strtok(message, "\n");
        if (tmp != NULL) {
            strcpy(message, tmp);
        }
        if (strcasecmp(message, "quit") == 0) {
            puts("Good bye :3");
            exit(EXIT_SUCCESS);
        }
        send(fd, message, SIZE_BUFFER, 0);
        wait = true;
    }
}

char *getStrBetween(char *str, char *PATTERN1, char *PATTERN2)
{
    if(PATTERN1 == NULL)
    {
        char temp[50]; sprintf(temp, "Temp%s", str);
        return getStrBetween(temp, "Temp", PATTERN2);
    }
    else if(PATTERN2 == NULL)
    {
        char temp[50]; sprintf(temp, "%sTemp", str);
        return getStrBetween(temp, PATTERN1, "Temp");
    }

    char *target = NULL;
    char *start, *end;

    if(start = strstr(str, PATTERN1))
    {
        start += strlen(PATTERN1);
        if (end = strstr(start, PATTERN2))
        {
            target = (char *)malloc(end - start + 1);
            memcpy(target, start, end - start);
            target[end - start] = '\0';
        }
    }

    if(target == NULL)
    {
        return NULL;
    }
    else
    {
        return target;
    }
}


void *checkOutput(void *client_sockets)
{

    int fd = *(int *) client_sockets;
    char message[DATA_BUFFER] = {0};
 
    while (1) {
        memset(message, 0, SIZE_BUFFER);
        serverOutput(fd, message);
        if (strcmp(message, "change type") == 0) {
            serverOutput(fd, message);
            strcpy(type, message);
        } 
        else {
            printf("%s", message);
        }
        fflush(stdout);
        wait = false;
    }
}
 


void serverOutput(int fd, char *input){

    if (recv(fd, input, DATA_BUFFER, 0) == 0) {
        printf("Disconnected from server\n");
        exit(EXIT_SUCCESS);
    }
}



int main(int argc, char *argv[]){

    pthread_t tid[2];
    int client_sockets = createSocket();
 
    if (!loginStatus(client_sockets, argc, argv)) {
        return -1;
    }
 
    pthread_create(&(tid[0]), NULL, &checkOutput, (void *) &client_sockets);
    pthread_create(&(tid[1]), NULL, &handleInput, (void *) &client_sockets);
 
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
 
    close(client_sockets);
    return 0;
}
