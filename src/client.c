#include "../include/client.h"
#include <ncurses.h>
#include <sys/select.h>

void update_positions(const char *message, Player *players, int *num_players)
{
    int         i = 0;
    const char *p = message;
    char       *endPtr;

    while(*p != '\0' && i < MAX_PLAYERS)
    {
        int newX;
        int newY;
        newX = (int)strtol(p, &endPtr, TENNER);
        if(p == endPtr)
        {
            break;
        }    // No more numbers to read
        p = endPtr;

        newY = (int)strtol(p, &endPtr, TENNER);
        if(p == endPtr)
        {
            break;    // No more numbers to read
        }
        p = endPtr;

        // Skip any spaces or commas between numbers
        while(*p == ' ' || *p == ',')
        {
            p++;
        }

        // Update the player's position
        players[i].x = newX;
        players[i].y = newY;
        i++;
    }

    *num_players = i;    // Update the number of players
}

void draw_players(Player *players, int num_players)
{
    clear();
    for(int i = 0; i < num_players; i++)
    {
        mvprintw(players[i].y, players[i].x, "X");
    }
    refresh();
}

int main(int argc, const char *argv[])
{
    int                sockfd;
    struct sockaddr_in servaddr;
    char               buffer[MAXLINE];
    ssize_t            n;
    Player             players[MAX_PLAYERS];
    int                num_players = MAX_PLAYERS;
    long               server_port;

    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_port = strtol(argv[2], NULL, TENNER);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons((uint16_t)server_port);
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while(1)
    {
        int ch = getch();    // Get the arrow key press

        switch(ch)
        {
            case KEY_UP:
                snprintf(buffer, MAXLINE, "0 -1");
                break;
            case KEY_DOWN:
                snprintf(buffer, MAXLINE, "0 1");
                break;
            case KEY_LEFT:
                snprintf(buffer, MAXLINE, "-1 0");
                break;
            case KEY_RIGHT:
                snprintf(buffer, MAXLINE, "1 0");
                break;
            case 'q':
                goto end;    // Quit
            default:
                continue;    // Ignore other keys
        }

        printf("Sending: %s\n", buffer);
        if(sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("sendto failed");
            break;
        }

        n = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
        if(n > 0)
        {
            buffer[n] = '\0';
            update_positions(buffer, players, &num_players);
            draw_players(players, num_players);
        }
    }

end:
    endwin();    // End ncurses mode
    close(sockfd);
    return 0;
}
