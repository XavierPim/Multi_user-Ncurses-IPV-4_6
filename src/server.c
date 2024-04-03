#include "../include/server.h"

int main(int argc, const char *argv[])
{
    const char        *ip = argv[1];
    char              *endptr;
    long               port = strtol(argv[2], &endptr, TENNER);
    int                sockfd;
    struct sockaddr_in servaddr;
    int                quit = 0;
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <IP> <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(*endptr != '\0')
    {
        fprintf(stderr, "Invalid port number.\n");
        exit(EXIT_FAILURE);
    }
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Create a socket
    if(sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Fill server information
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons((uint16_t)port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    // Bind the socket with the server address
    if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while(!quit)
    {
        int                ch = getch();
        struct sockaddr_in cliaddr;
        char               buffer[MAXLINE];
        socklen_t          len = sizeof(cliaddr);
        ssize_t            n;
        int                dx;
        int                dy;
        char              *endPtr;
        int                player_id = assign_position();
        if(ch == TWENNYSEVEN)
        {    // ESC key
             // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
            quit = 1;
            break;
        }

        n         = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        if(player_id == -1)
        {
            // No available positions
            continue;
        }

        // Parse the received buffer to extract x and y coordinates
        dx = (int)strtol(buffer, &endPtr, TENNER);
        dy = (int)strtol(endPtr, NULL, TENNER);

        // Update the position of the player
        update_position(&players[player_id], dx, dy);

        // Clear the screen and display all players
        clear();
        for(int i = 0; i < MAX_PLAYERS; i++)
        {
            if(players[i].occupied)
            {
                mvprintw(players[i].y, players[i].x, "X");    // Display the character 'X' at each player's position
            }
        }

        refresh();
    }
    if(quit)
    {
        // This is a placeholder to use 'quit'
        printf("Quitting...\n");
    }
    // End ncurses mode
    endwin();
    close(sockfd);
    return 0;
}

// Function to update the position of a player
void update_position(Player *player, int dx, int dy)
{
    player->x += dx;
    player->y += dy;
}

// Assign an unoccupied position to a new player
int assign_position(void)
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        if(!players[i].occupied)
        {
            players[i].occupied = 1;
            return i;
        }
    }
    return -1;    // No available positions
}
