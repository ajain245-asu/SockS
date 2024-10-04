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

using namespace std;
#define ECHOMAX 255     // Longest string to echo

vector<Game> games;
vector<Player> players;

void DieWithError( const char *errorMessage ) // External error handling function
{
    perror( errorMessage );
    exit( 1 );
}

int main( int argc, char *argv[] )
{
    int sock;                        // Socket
    struct sockaddr_in echoServAddr; // Local address of server
    struct sockaddr_in echoClntAddr; // Client address
    unsigned int cliAddrLen;         // Length of incoming message
    char echoBuffer[ ECHOMAX ];      // Buffer for echo string
    unsigned short echoServPort;     // Server port
    int recvMsgSize;                 // Size of received message

    if( argc != 2 )         // Test for correct number of parameters
    {
        fprintf( stderr, "Usage:  %s <UDP SERVER PORT>\n", argv[ 0 ] );
        exit( 1 );
    }

    echoServPort = atoi(argv[1]);  // First arg: local port

    // Create socket for sending/receiving datagrams
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "server: socket() failed" );

    // Construct local address structure */
    memset( &echoServAddr, 0, sizeof( echoServAddr ) ); // Zero out structure
    echoServAddr.sin_family = AF_INET;                  // Internet address family
    echoServAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // Any incoming interface
    echoServAddr.sin_port = htons( echoServPort );      // Local port

    // Bind to the local address
    if( bind( sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0 )
        DieWithError( "server: bind() failed" );

	printf( "server: Port server is listening to is: %d\n", echoServPort );

    for(;;) // Run forever
    {
        cliAddrLen = sizeof( echoClntAddr );
        Serv_Comms comm_to_recv = Serv_Comms();

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &comm_to_recv, sizeof(comm_to_recv), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen )) < 0 )
            DieWithError( "server: recvfrom() failed" );

        echoBuffer[ recvMsgSize ] = '\0';

        printf( "server: received a request from client on IP address %s\n", inet_ntoa( echoClntAddr.sin_addr ) );

        if (comm_to_recv.comm == END) {

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

            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) == strlen( echoBuffer ) )
                cout << "Sent data!" << endl;
            

        } else if (comm_to_recv.comm == START) {
            for (int i = 0 ; i < players.size() ; i++) {

            }
            
        } else if (comm_to_recv.comm == QUERY_GAME) {
            string output = "\n\nNo Ongoing games\n\n";
            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) == strlen( echoBuffer ) )
                cout << "Sent data!" << endl;
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
        if( sendto( sock, echoBuffer, strlen( echoBuffer ), 0, (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) != strlen( echoBuffer ) )
            DieWithError( "server: sendto() sent a different number of bytes than expected" );
    }
    // NOT REACHED */
}

void TheGameThePlayTheEverything(int t_port) {

    struct sockaddr_in echoServAddr; // Local address of server
    struct sockaddr_in echoClntAddr; // Client address
    unsigned int cliAddrLen;         // Length of incoming message
    char echoBuffer[ ECHOMAX ];      // Buffer for echo string
    unsigned short echoServPort = t_port;     // Server port
    int recvMsgSize;                 // Size of received message

    // Create socket for sending/receiving datagrams
    int sock;
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "server: socket() failed" );

    // Construct local address structure */
    memset( &echoServAddr, 0, sizeof( echoServAddr ) ); // Zero out structure
    echoServAddr.sin_family = AF_INET;                  // Internet address family
    echoServAddr.sin_addr.s_addr = htonl( INADDR_ANY ); // Any incoming interface
    echoServAddr.sin_port = htons( echoServPort );      // Local port

    // Bind to the local address
    if( bind( sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0 )
        DieWithError( "server: bind() failed" );

	printf( "server: Game is starting at port: %d\n", echoServPort );

    for(;;) // Run forever
    {
        cliAddrLen = sizeof( echoClntAddr );
        Serv_Comms comm_to_recv = Serv_Comms();

        // Block until receive message from a client
        if( ( recvMsgSize = recvfrom( sock, &comm_to_recv, sizeof(comm_to_recv), 0, (struct sockaddr *) &echoClntAddr, &cliAddrLen )) < 0 )
            DieWithError( "server: recvfrom() failed" );

        echoBuffer[ recvMsgSize ] = '\0';

        printf( "server: received a request from client on IP address %s\n", inet_ntoa( echoClntAddr.sin_addr ) );

        if (comm_to_recv.comm == END) {

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

            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) == strlen( echoBuffer ) )
                cout << "Sent data!" << endl;
            

        } else if (comm_to_recv.comm == START) {
            for (int i = 0 ; i < players.size() ; i++) {

            }
            
        } else if (comm_to_recv.comm == QUERY_GAME) {
            string output = "\n\nNo Ongoing games\n\n";
            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) == strlen( echoBuffer ) )
                cout << "Sent data!" << endl;
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
        if( sendto( sock, echoBuffer, strlen( echoBuffer ), 0, (struct sockaddr *) &echoClntAddr, sizeof( echoClntAddr ) ) != strlen( echoBuffer ) )
            DieWithError( "server: sendto() sent a different number of bytes than expected" );
    }
    // NOT REACHED */

}