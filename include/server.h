#ifndef COMP4981_ASS3_SERVER_H
    #define COMP4981_ASS3_SERVER_H

    #include <arpa/inet.h>
    #include <netinet/in.h>
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <sys/socket.h>
    #include <unistd.h>

    #define MAXLINE 64
    #define MAX_PLAYERS 4
    #define TENNER 10

// Player structure
typedef struct
{
    int x;
    int y;
    int occupied;
} Player;

// Structure to hold client information
typedef struct
{
    struct sockaddr_in addr;
    int                player_id;
} Client;

static Client clients[MAX_PLAYERS];    // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
static int    num_clients = 0;         // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

// Function declarations
void update_position(Player *players, int player_id, int dx, int dy);
int  assign_position(Player *players);
int  find_or_assign_client(struct sockaddr_in *cliaddr, Player *players);
void broadcast_positions(int sockfd, Player *players);

#endif    // COMP4981_ASS3_SERVER_H

// #define TWENNER 20
// #define TWENNYSEVEN 27
