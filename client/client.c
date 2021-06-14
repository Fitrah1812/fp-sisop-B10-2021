#include <stdio.h>
#include <string.h>
// #include <cstring>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#define cuk 0.001
#define gas 512-(212*1)

const int cekisi = sizeof(char) * gas;
const int ceksize = sizeof(int) * gas;
const int input = sizeof(int)* gas * 1;

//pembuatan socket server
int create_socket()
{
    struct sockaddr_in saddr;
    int fd, ret_val;
    int opt = 1;
    struct hostent *local_host; 
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(7000);
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "connect failed [%s]\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return fd;
}
char command[gas];
bool cek = false;
bool posisi = true;
bool cuek = false;
//cek input
//cek input tersebut sudah oke atau belum
void *cekinput(void *fdc)
{
    chdir("/home/fitrah1812/fp-sisop-B10-2021/database");
    int fd = *(int *) fdc;
    char message[gas] = {0};

    while (1) {
        gets(message);
        send(fd, message, cekisi, 0);
        if (cek) {
            strcpy(command, message);
        }
    }
}
void activeserver(int fd, char *input)
{
    if (recv(fd, input, gas, 0) == 0) {
        printf("Server telah meninggal\n");
        exit(EXIT_SUCCESS);
    }
}
//cek apakah output tersebut sesuai atau tidak
void *cekoutput(void *fdc) 
{
    //change directory 
    chdir("/home/fitrah1812/fp-sisop-B10-2021/client");
    int fd = *(int *) fdc;
    char message[gas] = {0};

    while (1) 
    {
        //set pesan menjadi nol
        memset(message, 0, cekisi);
        activeserver(fd, message);
        printf("%s", message);
        //pindahkan path
        if (strcmp(message, "Filepath: ") == 0) {
            cek = true;
        } else if (strcmp(message, "Memulai mengirimkan file\n") == 0) {
            kirim(fd);
            cek = false;
        } else if (strcmp(message, "File yang anda upload sudah ada\n") == 0) {
            cek = false;
        } else if (strcmp(message, "Memulai menerima file\n") == 0) {
            cetak(fd);
        } 
        fflush(stdout);
    }
}
//send file dari client to server
void kirim(int fd)
{
    //jangan lupa send pesan
    printf("Mengirimkan file %s ke server\n", command);
    int ret_val;
    FILE *sends = fopen(command, "r");
    char buf[gas] = {0};

    if (sends) {
        //file ditemukan
        send(fd, "File ditemukan", cekisi, 0);
        fseek(sends, 0L, SEEK_END);
        int size = ftell(sends);
        rewind(sends);
        sprintf(buf, "%d", size);
        send(fd, buf, cekisi, 0);
        while ((ret_val = fread(buf, 1, 1, sends)) > 0) 
        {
            send(fd, buf, 1, 0);
        } 
        printf("File telah dikirimkan\n");
        fclose(sends);
    } 
    else 
    {
        printf("File tidak ada\n");
        send(fd, "File tidak ada", cekisi, 0);
    }
}

void cetak(int fd)
{
    char buff[gas] = {0};
    int ret_val = recv(fd, buff, gas, 0);
    FILE *sends = fopen(buff, "w+");
    recv(fd, buff, gas, 0);
    int size = atoi(buff);
    while (size > 0) {
        ret_val = recv(fd, buff, gas, 0);
        //write 
        fwrite(buff, 1, ret_val, sends);
        memset(buff, 0, cekisi);
        size -= ret_val;
    }
    //masukkan ke client
    puts("File berhasil dikirimkan ke server");
    //send to server
    send(fd, "File berhasil dikirimkan", cekisi, 0);
    fclose(sends);
}
int main(int argc, char const *argv[])
{
    pthread_t tid[2];
    int fdc = create_socket();
    //bisa menghandle client banyak
    //minta masukka username
    pthread_create(&(tid[0]), NULL, &cekoutput, (void *) &fdc);
    //minta masukkan email
    pthread_create(&(tid[1]), NULL, &cekinput, (void *) &fdc);
    //joinkan
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    close(fdc);
    return 0;
}

