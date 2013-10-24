
#include "command.h"
#include "fio.h"

void ps(char splitInput[][20], int splitNum)
{
    char infoDescription[]="Name\t\t\b\bState\t\b\b\bPriority\t\bStack\t\bNum";
    fio_write(1,infoDescription,strlen(infoDescription));
    const int taskInfoNum=40;
    char str[taskInfoNum*5];
    vTaskList(str);
    fio_write(1,str,strlen(str));
}


void echo(char splitInput[][20], int splitNum)
{
    int i=1;
    while(splitInput[i][0] == '-' && i < splitNum)i++;
    for(; i < splitNum; i++)
    {
        fio_write(1, splitInput[i], strlen(splitInput[i]));
        fio_write(1, " ", 1);
    }
    fio_write(1, "\n", 1);
}

void cat(char splitInput[][20], int splitNum)
{
    int i=1;
    while(splitInput[i][0] == '-' && i < splitNum)i++;
    char buff[128];
    buff[0]='\0';
    size_t count;
    for(; i < splitNum; i++)
    {
        char path[20]="/romfs/";
        strcat(path,splitInput[i]);
        int fd = fs_open(path, 0, O_RDONLY);
        if(fd<0)
        {
            strcat(buff,"cat: ");
            strcat(buff,splitInput[i]);
            strcat(buff,": No such file or directory\r\n");
            fio_write(1,buff,strlen(buff));
            buff[0]='\0';
            continue;
        }
        do
        {
            //Read from /romfs/test.txt to buffer
            count = fio_read(fd, buff, sizeof(buff));

            //Write buffer to fd 1 (stdout, through uart)
            fio_write(1, buff, count);
        } while (count);
        fio_write(1, "\r", 1);
    }

}

void hello(char splitInput[][20], int splitNum)
{
    char *str = "Hello, World!\n";
    fio_write(1, str, strlen(str));
}

void ls(char splitInput[][20], int splitNum)
{
    char buff[128];
    int fileNum;
    buff[0]='\0';
    fileNum=getAllFileName("/romfs/",buff);
    fio_write(1,buff,strlen(buff));
    fio_write(1,"\r\n",2);
}
