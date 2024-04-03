#include "../include/client.h"

int main(int argc, char *argv[])
{
    int                sockfd;
    struct sockaddr_in servaddr;
    const char        *server_ip = argv[1];
    int                ch;
    char               buffer[MAXLINE];
    int                server_port = atoi(argv[2]);
    if(argc != 3)
    {
        fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Create a socket
    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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

        // Send the coordinate changes to the server
        if(sendto(sockfd, buffer, strlen(buffer), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        {
            perror("sendto failed");
            break;
        }
    }

    // End ncurses mode
    endwin();

    close(sockfd);
    return 0;
}
