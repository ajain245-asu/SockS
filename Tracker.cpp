// Implements the server side of an echo client-server application program.
// The client reads ITERATIONS strings from stdin, passes the string to the
// this server, which simply sends the string back to the client.
//
// Compile on general.asu.edu as:
//   g++ -o server UDPEchoServer.c
//
// Only on general3 and general4 have the ports >= 1024 been opened for
// application programs.
#include <stdio.h>      // for printf() and fprintf()
#include <sys/socket.h> // for socket() and bind()
#include <arpa/inet.h>  // for sockaddr_in and inet_ntoa()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <iostream>
#include "game.cpp"
#include <set>
#include <future>

using namespace std;
#define ECHOMAX 255     // Longest string to echo

vector<Game> games;
vector<Player> players;

void DieWithError( const char *errorMessage ) // External error handling function
{
    perror( errorMessage );
    exit( 1 );
}

void TheGameThePlayTheEverything(int t_port) {

    struct sockaddr_in trackerAddr; // Local address of server
    struct sockaddr_in playerAddr; // Client address
    unsigned int cliAddrLen;         // Length of incoming message
    char echoBuffer[ ECHOMAX ];      // Buffer for echo string
    unsigned short trackerPort = t_port;     // Server port
    int recvMsgSize;                 // Size of received message

    // Create socket for sending/receiving datagrams
    int sock;
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "server: socket() failed" );

    // Construct local address structure */
    memset( &trackerAddr, 0, sizeof( trackerAddr ) ); // Zero out structure
    trackerAddr.sin_family = AF_INET;                  // Internet address family
    trackerAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // Any incoming interface
    trackerAddr.sin_port = htons( trackerPort );      // Local port

    // Bind to the local address
    if( ::bind( sock, (struct sockaddr *) &trackerAddr, sizeof(trackerAddr)) < 0 )
        DieWithError( "server: bind() failed" );

    char start_msg[100];
    snprintf(start_msg, sizeof(start_msg), "server: Game is starting at port: %d", trackerPort);

    int sendtoSock = sendto( sock, start_msg, strlen( start_msg ), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) );
    if( sendtoSock == strlen( start_msg ) ) {
        cout << "val of send to sock just before the for loop is: " << sendtoSock;
        printf( "server: Game is starting at port: %d\n", trackerPort );
    }

    // cout << endl <<  playerAddr.sin_addr.s_addr << endl;
    // cout << playerAddr.sin_port << endl;
    // cout << trackerAddr.sin_addr.s_addr << endl;
    // cout << trackerAddr.sin_port << endl;

    for(;;) // Run forever
    {
        cliAddrLen = sizeof( playerAddr );
        Game_Pack comm_to_recv = Game_Pack();

        //debug
        cout << "Reached the for loop" << endl;

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &comm_to_recv, sizeof(comm_to_recv), 0, (struct sockaddr *) &playerAddr, &cliAddrLen )) < 0 )
            DieWithError( "server: recvfrom() failed" );

        echoBuffer[ recvMsgSize ] = '\0';
        cout << comm_to_recv.comm;

        printf( "server: received a request from client on IP address %s\n", inet_ntoa( playerAddr.sin_addr ) );

        if (comm_to_recv.comm == DRAW_FROM_STOCK) {
            cout << "Draw from stock selected" << endl;

        } else if (comm_to_recv.comm == DRAW_FROM_DISCARD) {
            string output = "\n\n----------------------------------------------\n";
            output += "\n\tDRAW_FROM_STOCK: ";
            output += "\n\tDRAW_FROM_DISCARD: ";
            output += "\n\tSTEAL_FROM_PLAYER: ";
            output += "\n";
            output += "\n----------------------------------------------\n";

            cout << output;

            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) == strlen( echoBuffer ) )
                cout << "Sent data!" << endl;
            

        } else if (comm_to_recv.comm == STEAL_FROM_PLAYER) {
            cout << "steal from player chosen" << endl;
            
        }

        // Send received datagram back to the client
        if( sendto( sock, echoBuffer, strlen( echoBuffer ), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) != strlen( echoBuffer ) )
            DieWithError( "server: sendto() sent a different number of bytes than expected" );
    }
    // NOT REACHED */

}

int main( int argc, char *argv[] )
{
    int sock;                        // Socket
    struct sockaddr_in trackerAddr; // Local address of server
    struct sockaddr_in playerAddr; // Client address
    unsigned int cliAddrLen;         // Length of incoming message
    char echoBuffer[ ECHOMAX ];      // Buffer for echo string
    unsigned short trackerPort;     // Server port
    int recvMsgSize;                 // Size of received message

    if( argc != 2 )         // Test for correct number of parameters
    {
        fprintf( stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[ 0 ] );
        exit( 1 );
    }

    trackerPort = atoi(argv[1]);  // First arg: local port

    // Create socket for sending/receiving datagrams
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "server: socket() failed" );

    // Construct local address structure */
    memset( &trackerAddr, 0, sizeof( trackerAddr ) ); // Zero out structure
    trackerAddr.sin_family = AF_INET;                  // Internet address family
    trackerAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // Any incoming interface
    trackerAddr.sin_port = htons( trackerPort );      // Local port

    // Bind to the local address
    if( ::bind( sock, (struct sockaddr *) &trackerAddr, sizeof(trackerAddr)) < 0 )
        DieWithError( "server: bind() failed" );

	printf( "server: Port server is listening to is: %d\n", trackerPort );

    for(;;) // Run forever
    {
        cliAddrLen = sizeof( playerAddr );
        Serv_Pack comm_to_recv = Serv_Pack();

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &comm_to_recv, sizeof(comm_to_recv), 0, (struct sockaddr *) &playerAddr, &cliAddrLen )) < 0 )
            DieWithError( "server: recvfrom() failed" );

        // echoBuffer[ recvMsgSize ] = '\0';

        printf( "server: received a request from client on IP address %s\n", inet_ntoa( playerAddr.sin_addr ) );

        if (comm_to_recv.comm == END) {
            // Can do nothing here

        } else if (comm_to_recv.comm == REGISTER) {
            Player playa = Player();
            playa = comm_to_recv.comm_args.reg;
            players.push_back(playa);

        } else if (comm_to_recv.comm == QUERY_PLAYER) {
            string output = "\n\n----------------------------------------------\n";
            for (int i = 0 ; i < players.size() ; i++) {
                output += "\n\tPlayer: ";
                output += players[i].name;
                output += "\n\tIPv4: ";
                output += players[i].ipv4;
                output += "\n\tTracker Port: ";
                output += to_string(players[i].t_port);
                output += "\n\tPlayer Port: ";
                output += to_string(players[i].p_port);
                output += "\n";
            }
            output += "\n----------------------------------------------\n";

            // cout << output;

            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) == strlen( echoBuffer ) )
                cout << "Sent data!" << endl;
            

        } else if (comm_to_recv.comm == START) {
            string output = "Starting Game at port: 8001";
            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) == output.size() ) {
                cout << "Sent data to hopefully start the game" << endl;

                // launch::async(TheGameThePlayTheEverything , 8001);
                std::async(std::launch::async, TheGameThePlayTheEverything, 8001);
            }

        } else if (comm_to_recv.comm == QUERY_GAME) {
            string output = "\n\nNo Ongoing games\n\n";
            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) == output.size() )
                cout << "Sent Queried Games Data!" << endl;
        } else if (comm_to_recv.comm == DEREGISTER) {
            vector<Player> tmp;
            for (Player p : players) {
                if (p.name == comm_to_recv.comm_args.drg.player) {
                    continue;
                }
                tmp.push_back(p);
            }
            players = tmp;
        }

        // Send received datagram back to the client
        if( sendto( sock, echoBuffer, strlen( echoBuffer ), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) != strlen( echoBuffer ) )
            DieWithError( "server: sendto() sent a different number of bytes than expected" );
    }
    // NOT REACHED */
}
