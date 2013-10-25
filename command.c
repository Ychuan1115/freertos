#include "FreeRTOS.h"
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


#define MIN_ALLOC_SIZE 256
#define CIRCBUFSIZE (configTOTAL_HEAP_SIZE/MIN_ALLOC_SIZE)
#define MMTEST_NUM 200

struct slot {
    void *pointer;
    unsigned int size;
    unsigned int lfsr;
};

static struct slot slots[CIRCBUFSIZE];

static unsigned int circbuf_size(unsigned int write_pointer, unsigned int read_pointer)
{
    return (write_pointer + CIRCBUFSIZE - read_pointer) % CIRCBUFSIZE;
}

static unsigned int lfsr = 0xACE1;
// Get a pseudorandom number generator from Wikipedia
static int prng(void)
{
    static unsigned int bit;
    /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    lfsr =  (lfsr >> 1) | (bit << 15);
    return lfsr & 0xffff;
}

void mmtest(char splitInput[][20], int splitNum)
{
    int i,j, size;
    char *p;
    unsigned int write_pointer = 0;
    unsigned int read_pointer = 0;

    for(j=0; j<MMTEST_NUM; j++)
    {
        do{
            size = prng() &  0x7FF;
        }while(size<MIN_ALLOC_SIZE);

        printf("try to allocate %d bytes\r\n", size);
        p = (char *) pvPortMalloc(size);
        printf("malloc returned %d\r\n", p);

        if (p == NULL || (write_pointer+1)%CIRCBUFSIZE == read_pointer) {
            // can't do new allocations until we free some older ones
            while (circbuf_size(write_pointer,read_pointer) > 0) {
                // confirm that data didn't get trampled before freeing
                p = slots[read_pointer].pointer;
                lfsr = slots[read_pointer].lfsr;  // reset the PRNG to its earlier state
                size = slots[read_pointer].size;
                read_pointer++;
                read_pointer %= CIRCBUFSIZE;
                printf("free a block, size %d\r\n", size);
                for (i = 0; i < size; i++) {
                    unsigned char u = p[i];
                    unsigned char v = (unsigned char) prng();
                    if (u != v) {
                        printf("OUCH: u=%02X, v=%02X\r\n", u, v);
                        return;
                    }
                }
                vPortFree(p);
                if ((prng() & 1) == 0) break;
            }
            send_byte('\r');
            send_byte('\n');
        } else {
            printf("allocate a block, size %d\r\n\r\n", size);
            if (circbuf_size(write_pointer, read_pointer) == CIRCBUFSIZE - 1) {
                fio_write(1,"circular buffer overflow\r\n",24);
                return;
            }
            slots[write_pointer].pointer=p;
            slots[write_pointer].size=size;
            slots[write_pointer].lfsr=lfsr;
            write_pointer++;
            write_pointer %= CIRCBUFSIZE;
            for (i = 0; i < size; i++) {
                p[i] = (unsigned char) prng();
            }
        }
    }
    do{
        p = slots[read_pointer].pointer;
        lfsr = slots[read_pointer].lfsr;  // reset the PRNG to its earlier state
        size = slots[read_pointer].size;
        read_pointer++;
        read_pointer %= CIRCBUFSIZE;
        printf("free a block, size %d\r\n", size);
        for (i = 0; i < size; i++) {
            unsigned char u = p[i];
            unsigned char v = (unsigned char) prng();
            if (u != v) {
                printf("OUCH: u=%02X, v=%02X\r\n", u, v);
                return;
            }
        }
        vPortFree(p);
    }while(read_pointer!=write_pointer);
}
