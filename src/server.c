#include "../include/server.h"

int main(int argc, const char *argv[])
{
    int                sockfd;
    struct sockaddr_in servaddr;
    const char        *ip = argv[1];
    char              *endptr;
    long               port = strtol(argv[2], &endptr, TENNER);
    Player             players[MAX_PLAYERS];

    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        players[i].occupied = 0;    // Initialize to 0 (not occupied)
        players[i].x        = 0;    // Initialize x position
        players[i].y        = 0;    // Initialize y position
    }

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
    if(sockfd < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons((uint16_t)port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    if(bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening for coordinate changes...\n");

    while(1)
    {
        struct sockaddr_in cliaddr;
        char               buffer[MAXLINE];
        socklen_t          len = sizeof(cliaddr);
        int                player_id;
        ssize_t            n;
        char              *endPtr;
        int                dx;
        int                dy;

        n         = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&cliaddr, &len);
        buffer[n] = '\0';

        if(strcmp(buffer, "quit") == 0)
        {
            break;    // Exit the loop if the "quit" message is received
        }

        player_id = find_or_assign_client(&cliaddr, players);
        if(player_id == -1)
        {
            continue;    // No available positions
        }

        dx = (int)strtol(buffer, &endPtr, TENNER);
        dy = (int)strtol(endPtr, NULL, TENNER);

        update_position(players, player_id, dx, dy);
        printf("Updated player %d position to (%d, %d)\n", player_id, players[player_id].x, players[player_id].y);
        broadcast_positions(sockfd, players);
    }

    close(sockfd);
    return 0;
}

// Function to find or assign a client to a player position
int find_or_assign_client(struct sockaddr_in *cliaddr, Player *players)
{
    int i;
    // Check if the client is already assigned to a player
    for(i = 0; i < num_clients; i++)
    {
        if(memcmp(&clients[i].addr, cliaddr, sizeof(struct sockaddr_in)) == 0)
        {
            return clients[i].player_id;
        }
    }

    // Assign a new player position to the client
    if(num_clients < MAX_PLAYERS)
    {
        int player_id = assign_position(players);
        if(player_id != -1)
        {
            clients[num_clients].addr      = *cliaddr;
            clients[num_clients].player_id = player_id;
            num_clients++;
            return player_id;
        }
    }
    return -1;    // No available positions
}

// Function to update the position of a player
void update_position(Player *players, int player_id, int dx, int dy)
{
    players[player_id].x += dx;
    players[player_id].y += dy;
}

int assign_position(Player *players)
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        if(!players[i].occupied)
        {
            players[i].occupied = 1;
            players[i].x        = 0;    // Initialize position to (0, 0)
            players[i].y        = 0;
            return i;
        }
    }
    return -1;    // No available positions
}

// Function to broadcast player positions to all clients
void broadcast_positions(int sockfd, Player *players)
{
    char message[MAXLINE];
    int  i;
    int  offset = 0;    // Keep track of the current position in the message buffer

    // Construct the message with player positions
    for(i = 0; i < num_clients; i++)    // Use num_clients instead of MAX_PLAYERS
    {
        offset += snprintf(message + offset, (unsigned long)(MAXLINE - offset), "%d %d ", players[i].x, players[i].y);
    }

    // Print the message before sending
    printf("Broadcasting: %s\n", message);

    // Send the message to all connected clients
    for(i = 0; i < num_clients; i++)
    {
        sendto(sockfd, message, strlen(message), 0, (struct sockaddr *)&clients[i].addr, sizeof(clients[i].addr));
    }
}
