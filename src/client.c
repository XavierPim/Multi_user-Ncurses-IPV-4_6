#include "../include/client.h"

int main(int argc, const char *argv[])
{
    int                sockfd;
    struct sockaddr_in servaddr;
    const char        *server_ip = argv[1];
    char               buffer[MAXLINE];
    long               server_port;
    char              *endptr;
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    server_port = strtol(argv[2], &endptr, TENNER);
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
    servaddr.sin_port        = htons((uint16_t)server_port);
    servaddr.sin_addr.s_addr = inet_addr(server_ip);

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while(1)
    {
        int     ch;
        ssize_t n;
        ch = getch();    // Get the arrow key press

        if(ch == TWENNYSEVEN)
        {             // ESC key
            break;    // Exit the loop
        }

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
            default:
                continue;    // Ignore other keys
        }
        printf("Sending: %s\n", buffer);
        // Send the coordinate changes to the server
        if(sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("sendto failed");
            break;
        }

        // Receive the broadcast message from the server
        n         = recvfrom(sockfd, buffer, MAXLINE, 0, NULL, NULL);
        buffer[n] = '\0';

        update_positions(buffer);
        draw_players();
    }

    // End ncurses mode
    endwin();

    close(sockfd);
    return 0;
}

void update_positions(const char *message)
{
    int         i;
    const char *p = message + strlen("Positions:");
    char       *endPtr;

    for(i = 0; i < MAX_PLAYERS; i++)
    {
        int newX;
        int newY;
        if(!p)
        {
            break;    // Check if pointer is NULL
        }

        errno = 0;
        newX  = (int)strtol(p, &endPtr, TENNER);
        if(p == endPtr || errno == ERANGE)
        {
            break;    // Check for valid conversion and range errors
        }
        p = endPtr;

        newY = (int)strtol(p, &endPtr, TENNER);
        if(p == endPtr)
        {
            break;    // Check for valid conversion
        }
        p = strchr(p, ',');
        if(p)
        {
            p += 2;    // Skip to the next player's coordinates
        }

        // Check if new positions are within bounds
        if(newX >= 0 && newX < MAX_WIDTH && newY >= 0 && newY < MAX_HEIGHT)
        {
            players[i].x = newX;
            players[i].y = newY;
        }
    }
}

void draw_players(void)
{
    int i;
    clear();
    for(i = 0; i < MAX_PLAYERS; i++)
    {
        mvprintw(players[i].y, players[i].x, "X");
    }
    refresh();
}
