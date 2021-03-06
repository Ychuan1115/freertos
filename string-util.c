#include <stddef.h>
#include <stdint.h>
#include <limits.h>

#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(x) ((x)-ONES & ~(x) & HIGHS)

#define SS (sizeof(size_t))
void *memset(void *dest, int c, size_t n)
{
	unsigned char *s = dest;
	c = (unsigned char)c;
	for (; ((uintptr_t)s & ALIGN) && n; n--) *s++ = c;
	if (n) {
		size_t *w, k = ONES * c;
		for (w = (void *)s; n>=SS; n-=SS, w++) *w = k;
		for (s = (void *)w; n; n--, s++) *s = c;
	}
	return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
	void *ret = dest;

	//Cut rear
	uint8_t *dst8 = dest;
	const uint8_t *src8 = src;
	switch (n % 4) {
		case 3 : *dst8++ = *src8++;
		case 2 : *dst8++ = *src8++;
		case 1 : *dst8++ = *src8++;
		case 0 : ;
	}

	//stm32 data bus width
	uint32_t *dst32 = (void *)dst8;
	const uint32_t *src32 = (void *)src8;
	n = n / 4;
	while (n--) {
		*dst32++ = *src32++;
	}

	return ret;
}

char *strchr(const char *s, int c)
{
	for (; *s && *s != c; s++);
	return (*s == c) ? (char *)s : NULL;
}

char *strcpy(char *dest, const char *src)
{
	const unsigned char *s = src;
	unsigned char *d = dest;
	while ((*d++ = *s++));
	return dest;
}

char *strncpy(char *dest, const char *src, size_t n)
{
	const unsigned char *s = src;
	unsigned char *d = dest;
	while (n-- && (*d++ = *s++));
	return dest;
}

size_t strlen ( const char * str )
{
    int count;
    for(count=0 ; str[count]!='\0';count++);
    return count;
}

int strcmp ( const char * str1, const char * str2 )
{
    int i=-1;
    do
    {
        i++;
        if(str1[i]!=str2[i])
        {
            return (str1[i]>str2[i])?1:-1;
        }

    }while(str1[i]!='\0' && str2[i]!='\0');

    return 0;
}

char * strcat ( char * destination, const char * source )//suppose the destination has enough space for concatenating the source
{
    int dLength = strlen(destination);
    int sLength = strlen(source);
    int i;
    for (i = 0; i<sLength; i++)
    {
        destination[i+dLength]=source[i];
    }
    destination[dLength+sLength]='\0';
    return destination;
}

char* itoa(int value, char* str)//only support base=10
{
    int base = 10;
    int divideNum = base;
    int i=0;
    while(value/divideNum > 0)
    {
        divideNum*=base;
    }
    if(value < 0)
    {
        str[0] = '-';
        i++;
    }
    while(divideNum/base > 0)
    {
        divideNum/=base;
        str[i++]=value/divideNum+48;
        value%=divideNum;
    }
    str[i]='\0';
    return str;

}
