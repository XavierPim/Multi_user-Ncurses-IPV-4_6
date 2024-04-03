#ifndef COMP4981_ASS3_CLIENT_H
#define COMP4981_ASS3_CLIENT_H

#include <arpa/inet.h>
#include <errno.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAXLINE 1024
#define TENNER 10
#define TWENNER 20
#define TWENNYSEVEN 27
#define MAX_PLAYERS 27
#define MAX_WIDTH 80
#define MAX_HEIGHT 24

typedef struct
{
    int x, y;
} Player;

void          update_positions(const char *message);
void          draw_players(void);
static Player players[MAX_PLAYERS];    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

#endif    // COMP4981_ASS3_CLIENT_H
