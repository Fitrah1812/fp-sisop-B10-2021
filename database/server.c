#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <dirent.h>
#include <time.h>
#define DATA_BUFFER 300
int curr_fd = -1;
int curr_id = -1;
char curr_db[DATA_BUFFER] = {0};
const int SIZE_BUFFER = sizeof(char) * DATA_BUFFER;
const char *currDir = "/home/fitrah1812/fp-sisop-B10-2021/databaseku/databases";
int create_tcp_server_socket();
void *routes(void *argv);
bool login(int fd, char *username, char *password);
void regist(int fd, char *username, char *password);
void useDB(int fd, char *db_name);
void grantDB(int fd, char *db_name, char *username);
void createDB(int fd, char *db_name);
void createTable(int fd, char parsed[20][DATA_BUFFER]);
int getInput(int fd, char *prompt, char *storage);
int getUserId(char *username, char *password);
int getLastId(char *db_name, char *table);
void explode(char *string, char storage[20][DATA_BUFFER], const char *delimiter);
void changeCurrDB(int fd, const char *db_name);
FILE *getTable(char *db_name, char *table, char *cmd);
bool dbExist(int fd, char *db_name, bool printError);
FILE *getOrMakeTable(char *db_name, char *table, char *cmd, char *collumns);
bool tableExist(int fd, char *db_name, char *table, bool printError);
void insert(int fd, char parsed[20][DATA_BUFFER]);
char *temp_databaseName;
void dropTabel(int fd,char *databaseName,char *tableName);


int main()

{   
    system("mkdir -p databases");
    system("mkdir -p databases/config");
    pid_t pid, sid;        // Variabel untuk menyimpan PID

    pid = fork();     // Menyimpan PID dari Child Process

    /* Keluar saat fork gagal
    * (nilai variabel pid < 0) */
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    /* Keluar saat fork berhasil
    * (nilai variabel pid adalah PID dari child process) */
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    while(1){
        socklen_t addrlen;
        struct sockaddr_in new_addr;
        pthread_t tid;
        char buf[DATA_BUFFER];
        int server_fd = create_tcp_server_socket();
        int new_fd;
 
        while (1) 
        {
            new_fd = accept(server_fd, (struct sockaddr *)&new_addr, &addrlen);
            if (new_fd >= 0) 
            {
                printf("Accepted a new connection with fd: %d\n", new_fd);
                pthread_create(&tid, NULL, &routes, (void *) &new_fd);
            } else 
            {
                fprintf(stderr, "Accept failed [%s]\n", strerror(errno));
            }
        }
        sleep(30);
    }
}
 
// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }
void dropDatabase(int fd, char* databaseName)
{
    char path_temp[100] = "";
    strcpy(path_temp, databaseName);

    pid_t child_id;
    child_id = fork();

    int status;

    if (child_id < 0) {
        printf("Data base failed to drop\n\n");
        exit(EXIT_FAILURE); // Jika gagal membuat proses baru, program akan berhenti
    }

    if (child_id == 0) {
        char *argv[] = {"rm", "-r", path_temp, NULL};
        execv("/bin/rm", argv);
    }
    else{
        while ((wait(&status)) > 0)
        {
            write(fd, "Data base succesfully dropped\n\n", SIZE_BUFFER);
            return;
        }
        
    }
}
// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

void dropTabel(int fd, char *database, char *table) {
    char file[1024];
    sprintf(file, "%s/%s", database, table);
    if (access(file, F_OK) != 0) {
        write(fd, "Table doesn't exists!\n\n", SIZE_BUFFER);
        return;
    }
    remove(file);
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }
void dropTable(int fd, char *table){
    if(curr_db[0] == '\0'){
        write(fd, "no database used\n", SIZE_BUFFER);
        return;
    }
    if(tableExist(fd, curr_db, table, true)){
        char path[DATA_BUFFER];
        sprintf(path, "./%s/$s.csv", curr_db, table);
        remove(path);
        write(fd, "table dropped", SIZE_BUFFER);
    }
}



// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }
void insert(int fd, char parsed[20][DATA_BUFFER]){
    if(strlen(curr_db)==0){
        write(fd, "Error::No database used\n\n", SIZE_BUFFER);
        return;
    }
    char *table = parsed[2];
    FILE *fp = getTable(curr_db,table,"r");
    if(fp==NULL){
        write(fd, "Error::Table doesn't exist\n\n", SIZE_BUFFER);
        return;
    }
    char data[DATA_BUFFER];
    int seq=1;
    char value[DATA_BUFFER];
    char first[DATA_BUFFER];
    strcpy(first,parsed[3]+1);
    sprintf(value, "\'value%d\',", seq);
    while(seq){
        if (strlen(first) == 0) {
            break;
        }
        if(strcmp(first,value)==0){
            if(seq==1) strcpy(data,parsed[4]);
            else strcat(data,parsed[4]);
            seq++;
            sprintf(value, "\'value%d\',", seq);
            break;
        }
        else{
            printf("masuk\n");
            if(seq==1) strcpy(data, " ,");
            else strcat(data, " ,");
            seq++;
            sprintf(value, "\'value%d\',", seq);
        }
    }
    for(int i=5;i<20;i+=2){
        if (strlen(parsed[i]) == 0) {
            break;
        }
        while(seq){
            if (strlen(parsed[i]) == 0) {
                break;
            }
            if(strcmp(parsed[i],value)==0){
                strcat(data,parsed[i+1]);
                seq++;
                sprintf(value, "\'value%d\',", seq);
                break;
            }
            else{
                seq++;
                sprintf(value, "\'value%d\',", seq);
                strcat(data, " ,");
            }
        }
    }
    char *hasil;
    hasil=strtok(data,")");
    fp = getOrMakeTable(curr_db, table, "a", hasil);
    fprintf(fp, "%s\n", hasil);
    fclose(fp);
    write(fd, "Data inserted\n\n", SIZE_BUFFER);
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }


// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }
void *routes(void *argv)
{
    chdir(currDir); // TODO:: comment on final
    int fd = *(int *) argv;
    char query[DATA_BUFFER], buf[DATA_BUFFER];
    char parsed[20][DATA_BUFFER];
 
    while (read(fd, query, DATA_BUFFER) != 0) {
        puts(query);
        explode(query, parsed, " ");
 
        if (strcmp(parsed[0], "LOGIN") == 0) {
            if (!login(fd, parsed[1], parsed[2]))
                break;
        }
        else if (strcmp(parsed[0], "CREATE") == 0) {
            if (strcmp(parsed[1], "USER") == 0) {
                currDir=("/home/fitrah1812/fp-sisop-B10-2021/databaseku/databases/config");
                regist(fd, parsed[2], parsed[5]);
            }
            else if (strcmp(parsed[1], "DATABASE") == 0) {
                createDB(fd, parsed[2]);
            }
            else if (strcmp(parsed[1], "TABLE") == 0) {
                createTable(fd, parsed);
            }
            else write(fd, "Invalid query on CREATE command\n\n", SIZE_BUFFER);
        }
        else if (strcmp(parsed[0], "USE") == 0) {
            // strcpy(temp_databaseName,parsed[1]);
            useDB(fd, parsed[1]);
        }
        else if (strcmp(parsed[0], "GRANT") == 0) {
            grantDB(fd, parsed[2], parsed[4]);
        }
        else if (strcmp(parsed[0], "DROP") == 0)
        {
            if(strcmp(parsed[1], "DATABASE") == 0){
                dropDatabase(fd, parsed[2]);
            }
            else if(strcmp(parsed[1], "TABLE") == 0)
            {
                dropTabel(fd,temp_databaseName,parsed[2]);
            }
        }
        else if(strcmp(parsed[0], "INSERT")==0){
            insert(fd, parsed);
        }
        else write(fd, "Invalid query\n\n", SIZE_BUFFER);
    }
    if (fd == curr_fd) {
        curr_fd = curr_id = -1;
        memset(curr_db, '\0', sizeof(char) * DATA_BUFFER);
    }
    printf("Close connection with fd: %d\n", fd);
    close(fd);
}



// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

void createTable(int fd, char parsed[20][DATA_BUFFER])
{
    if (strlen(curr_db) == 0) {
        write(fd, "Error::No database used\n\n", SIZE_BUFFER);
        return;
    }
    char *table = parsed[2];
 
    // Make sure that table doesn't exist in the current database
    FILE *fp = getTable(curr_db, table, "r");
    if (fp != NULL) {
        fclose(fp);
        write(fd, "Error::Table already exists\n\n", SIZE_BUFFER);
        return;
    }

    char cols[DATA_BUFFER];
    strcpy(cols, parsed[3] + 1);
    for (int i = 5; i < 20; i+=2) {
        if (strlen(parsed[i]) == 0) {
            break;
        }
        strcat(cols, ",");
        strcat(cols, parsed[i]);
    }
 
    fp = getOrMakeTable(curr_db, table, "a", cols);
    fclose(fp);
    write(fd, "Table created\n\n", SIZE_BUFFER);
}


// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }
void createDB(int fd, char *db_name)
{
    if (dbExist(fd, db_name, false)) {
        write(fd, "Error::Database already exists\n\n", SIZE_BUFFER);
        return;
    }
    if (mkdir(db_name, 0777) == -1) {
        write(fd, 
            "Error::Unknown error occurred when creating new database\n\n", 
            SIZE_BUFFER);
        return;
    }
    if (curr_id != 0) {
        FILE *fp = getOrMakeTable("config", "permissions", "a", "id,db_name");
        fprintf(fp, "%d,%s\n", curr_id, db_name);
        fclose(fp);
    }
    write(fd, "Database created\n\n", SIZE_BUFFER);
}
 

 // void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

void grantDB(int fd, char *db_name, char *username)
{
    if (curr_id != 0 || strcmp(db_name, "config") == 0) {
        write(fd, "Error::Forbidden action\n\n", SIZE_BUFFER);
        return;
    }
    if (!dbExist(fd, db_name, true)) {
        return;
    }
    int target_id = getUserId(username, NULL);
    if (target_id == -1) {
        write(fd, "Error::User not found\n\n", SIZE_BUFFER);
        return;
    }
    bool alreadyExist = false;
 
    FILE *fp = getOrMakeTable("config", "permissions", "r", "id,db_name");
    char db[DATA_BUFFER], input[DATA_BUFFER];
    sprintf(input, "%d,%s", target_id, db_name);
    while (fscanf(fp, "%s", db) != EOF) {
        if (strcmp(input, db) == 0) {
            alreadyExist = true;
            break;
        }
    }
    fclose(fp);
 
    if (alreadyExist) {
        write(fd, "Info::User already authorized\n\n", SIZE_BUFFER);
    } else {
        FILE *fp = getTable("config", "permissions", "a");
        fprintf(fp, "%d,%s\n", target_id, db_name);
        fclose(fp);
        write(fd, "Permission added\n\n", SIZE_BUFFER);
    }
}


// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }
void useDB(int fd, char *db_name)
{
    // strcpy(temp_databaseName,db_name);

    if (!dbExist(fd, db_name, true)) {
        return;
    }

    DIR *dp = opendir(db_name);
    bool authorized = false;
 
    if (curr_id != 0) {
        FILE *fp = getTable("config", "permissions", "r");
        if (fp != NULL) {
            char db[DATA_BUFFER], input[DATA_BUFFER];
            sprintf(input, "%d,%s", curr_id, db_name);
            while (fscanf(fp, "%s", db) != EOF) {
                if (strcmp(input, db) == 0) {
                    authorized = true;
                    break;
                }
            }
            fclose(fp);
        } else {
            authorized = false;
        }  
    } else {
        authorized = true;
    }
 
    if (authorized) {
        changeCurrDB(fd, db_name);
    } else {
        write(fd, "Error::Unauthorized access\n\n", SIZE_BUFFER);
    }
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }


FILE *getTable(char *db_name, char *table, char *cmd)
{
    FILE *fp = NULL;
    if (tableExist(-1, db_name, table, false)) {
        char path[DATA_BUFFER];
        sprintf(path, "./%s/%s.csv", db_name, table);
        fp = fopen(path, cmd);
    }
    return fp;
}


// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

FILE *getOrMakeTable(char *db_name, char *table, char *cmd, char *collumns)
{
    FILE *fp = getTable(db_name, table, cmd);
    if (fp == NULL) {
        char path[DATA_BUFFER];
        sprintf(path, "./%s/%s.csv", db_name, table);
        
        FILE *_fp = fopen(path, "w");
        if (collumns != NULL) fprintf(_fp, "%s\n", collumns);
        fclose(_fp);
        fp = fopen(path, cmd);
    }
    return fp;
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

void regist(int fd, char *username, char *password)
{
    FILE *fpi = fopen("users.csv","a");
    char *cols = NULL;
    if (curr_id != 0) {
        write(fd, "Error::Forbidden action\n\n", SIZE_BUFFER);
        return;
    }
    FILE *fp = getOrMakeTable("config", "users", "a", "id,username,password");
    int id = getUserId(username, password);
 
    if (id != -1) {
        write(fd, "Error::User is already registered\n\n", SIZE_BUFFER);
    } else {
        id = getLastId("config", "users") + 1;
        fprintf(fp, "%d,%s,%s\n", id, username, password);
        write(fd, "Register success\n\n", SIZE_BUFFER);
    }
    fclose(fp);
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

bool login(int fd, char *username, char *password)
{
    if (curr_fd != -1) {
        write(fd, "Server is busy, wait for other user to logout.\n", SIZE_BUFFER);
        return false;
    }
 
    int id = -1;
    if (strcmp(username, "root") == 0) {
        id = 0;
    } else { // Check data in DB
        FILE *fp = getTable("config", "users", "r");
        if (fp != NULL) {
            id = getUserId(username, password);
            fclose(fp);
        }
    }
 
    if (id == -1) {
        write(fd, "Error::Invalid id or password\n", SIZE_BUFFER);
        return false;
    } else {
        write(fd, "Login success\n", SIZE_BUFFER);
        curr_fd = fd;
        curr_id = id;
    }
    return true;
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }


void changeCurrDB(int fd, const char *db_name)
{
    write(fd, "change type", SIZE_BUFFER);
    if (db_name == NULL) {
        memset(curr_db, '\0', sizeof(char) * DATA_BUFFER);
        write(fd, (curr_id == 0) ? "root" : "user", SIZE_BUFFER);
    } 
    else {
        strcpy(curr_db, db_name);
        write(fd, db_name, SIZE_BUFFER);
    }
}
 
// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

bool dbExist(int fd, char *db_name, bool printError)
{
    struct stat s;
    int err = stat(db_name, &s);
    if (err == -1 || !S_ISDIR(s.st_mode)) {
        if (printError) write(fd, "Error::Database not found\n\n", SIZE_BUFFER);
        return false;
    }
    return true;
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

bool tableExist(int fd, char *db_name, char *table, bool printError)
{
    struct stat s;
    char path[DATA_BUFFER];
    sprintf(path, "./%s/%s.csv", db_name, table);
    int err = stat(path, &s);
    if (err == -1 || !S_ISREG(s.st_mode)) {
        if (printError && fd != -1) write(fd, "Error::Table not found\n\n", SIZE_BUFFER);
        return false;
    }
    return true;
}

// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

int getUserId(char *username, char *password)
{
    int id = -1;
    FILE *fp = getTable("config", "users", "r");
 
    if (fp != NULL) {
        char db[DATA_BUFFER], input[DATA_BUFFER];
        if (password != NULL) {
            sprintf(input, "%s,%s", username, password);
        } else {
            sprintf(input, "%s", username);
        }
        
        while (fscanf(fp, "%s", db) != EOF) {
            char *temp = strstr(db, ",") + 1; // Get username and password from db
            
            if (password == NULL) {
                temp = strtok(temp, ",");
            }
            if (strcmp(temp, input) == 0) {
                id = atoi(strtok(db, ","));  // Get id from db
                break;
            }
        }
        fclose(fp);
    }
    return id;
}
 
// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }


int getLastId(char *db_name, char *table)
{
    int id = 1;
    FILE *fp = getTable(db_name, table, "r");
 
    if (fp != NULL) {
        char db[DATA_BUFFER];
        while (fscanf(fp, "%s", db) != EOF) {
            id = atoi(strtok(db, ","));  // Get id from db
        }
    }
    return id;
}
 
// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

void explode(char string[], char storage[20][DATA_BUFFER], const char *delimiter)
{
    char *buf = string;
    char *temp = NULL;
    memset(storage, '\0', sizeof(char) * 20 * DATA_BUFFER);
 
    int i = 0;
    while ((temp = strtok(buf, delimiter)) != NULL && i < 20) {
        if (buf != NULL) {
            buf = NULL;
        }
        strcpy(storage[i++], temp);
    }
    char *ptr = strchr(storage[--i], ';');
    if (ptr != NULL) {
        *ptr = '\0';
    }
}
 
// void storeToLogs(char *username, char *command)
// {
//     time(&my_time);
// 	timeinfo = localtime(&my_time);

// 	char day[10], month[10], year[10], hour[10], minute[10], second[10];

// 	sprintf(day, "%d", timeinfo->tm_mday);
// 	if(timeinfo->tm_mday < 10)
// 		sprintf(day, "0%d", timeinfo->tm_mday);

// 	sprintf(month, "%d", timeinfo->tm_mon+1);
// 	if(timeinfo->tm_mon+1 < 10)
// 		sprintf(month, "0%d", timeinfo->tm_mon+1);

// 	sprintf(year, "%d", timeinfo->tm_year+1900);

// 	sprintf(hour, "%d", timeinfo->tm_hour);
// 	if(timeinfo->tm_hour < 10)
// 		sprintf(hour, "0%d", timeinfo->tm_hour);

// 	sprintf(minute, "%d", timeinfo->tm_min);
// 	if(timeinfo->tm_min < 10)
// 		sprintf(minute, "0%d", timeinfo->tm_min);

// 	sprintf(second, "%d", timeinfo->tm_sec);
// 	if(timeinfo->tm_sec < 10)
// 		sprintf(second, "0%d", timeinfo->tm_sec);

//     char content[arraySize];
//     sprintf(content, "%s-%s-%s %s:%s:%s:%s:%s", year, month, day, hour, minute, second, username, command);

//     FILE* fptr_logs = fopen(logPath, "a");
//     fprintf(fptr_logs, "%s\r\n", content);
//     fclose(fptr_logs);
// }

int create_tcp_server_socket()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    int opt = 1;
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("Created a socket with fd: %d\n", fd);
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7000);
    saddr.sin_addr.s_addr = INADDR_ANY;
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        exit(EXIT_FAILURE);
    }
    return fd;
}
 
