#ifndef COMP4981_ASS3_SERVER_H
#define COMP4981_ASS3_SERVER_H

#include <arpa/inet.h>
#include <ncurses.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXLINE 1024
#define MAX_PLAYERS 4
#define TENNER 10
#define TWENNER 20
#define TWENNYSEVEN 27

// Player structure
typedef struct
{
    int x, y;
    int occupied;
} Player;

// Initialize player positions
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static Player players[MAX_PLAYERS] = {
  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
    {TENNER,  0,       0}, // North
    {TENNER,  TWENNER, 0}, // South
    {0,       TENNER,  0}, // West
    {TWENNER, TENNER,  0}  // East
};
// Function to update the position of a player
void update_position(Player *player, int dx, int dy);

int assign_position(void);
#endif    // COMP4981_ASS3_SERVER_H
