#ifndef COMMAND_H
#define COMMAND_H

typedef void (*cmd_func)(char splitInput[][20], int);
typedef struct
{
    char *name;
    cmd_func handler;
}cmd_type;

void ps(char splitInput[][20], int splitNum);
void echo(char splitInput[][20], int splitNum);
void cat(char splitInput[][20], int splitNum);
void hello(char splitInput[][20], int splitNum);
void ls(char splitInput[][20], int splitNum);
void mmtest(char splitInput[][20], int splitNum);

static cmd_type cmd[]={
    {
        .name="ps",
        .handler=ps
    },
    {
        .name="echo",
        .handler=echo
    },
    {
        .name="cat",
        .handler=cat
    },
    {
        .name="hello",
        .handler=hello
    },
    {
        .name="ls",
        .handler=ls
    },
    {
        .name="mmtest",
        .handler=mmtest
    }
};

#endif
