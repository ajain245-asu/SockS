// Implements the client side of an echo client-server application program.
// The client reads ITERATIONS strings from stdin, passes the string to the
// server, which simply echoes it back to the client.
//
// Compile on general.asu.edu as:
//   g++ -o client UDPEchoClient.c
//
// Only on general3 and general4 have the ports >= 1024 been opened for
// application programs.
#include <stdio.h>      // for printf() and fprintf()
#include <sys/socket.h> // for socket(), connect(), sendto(), and recvfrom()
#include <arpa/inet.h>  // for sockaddr_in and inet_addr()
#include <stdlib.h>     // for atoi() and exit()
#include <string.h>     // for memset()
#include <unistd.h>     // for close()
#include <iostream>
#include "game.cpp"

using namespace std;

#define ECHOMAX 255     // Longest string to echo
#define ITERATIONS	5   // Number of iterations the client executes

void DieWithError( const char *errorMessage ) // External error handling function
{
    perror( errorMessage );
    exit(1);
}

string trim(const std::string &str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        start++;
    }

    auto end = str.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

// Function to check if a string is a number (after trimming)
bool is_number(const string &str) {
    std::string trimmed_str = trim(str);
    return !trimmed_str.empty() && std::all_of(trimmed_str.begin(), trimmed_str.end(), ::isdigit);
}

void PlayGame(int port) {
    size_t nread;
    int sock;                        // Socket descriptor
    struct sockaddr_in echoServAddr; // Echo server address
    struct sockaddr_in fromAddr;     // Source address of echo
    unsigned short gamePort;     // Echo server port
    unsigned int fromSize;           // In-out of address size for recvfrom()
    char *servIP;                    // IP address of server
    char *echoString = NULL;         // String to send to echo server
    string input = "";         // String to send to echo server
    size_t echoStringLen = ECHOMAX;               // Length of string to echo
    int respStringLen;               // Length of received response

    echoString = (char *) malloc( ECHOMAX );

    servIP = "127.0.0.1";
    gamePort = port;  // Second arg: Use given port

    printf( "client: Arguments passed: server IP %s, port %d\n", servIP, gamePort );

    // Create a datagram/UDP socket
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "client: socket() failed" );

    // Construct the server address structure
    memset( &echoServAddr, 0, sizeof( echoServAddr ) ); // Zero out structure
    echoServAddr.sin_family = AF_INET;                  // Use internet addr family
    echoServAddr.sin_addr.s_addr = inet_addr( servIP ); // Set server's IP address
    echoServAddr.sin_port = htons( gamePort );      // Set server's port

	// Pass string back and forth between server ITERATIONS times

	printf( "client: Echoing strings for %d iterations\n", ITERATIONS );



    for( int i = 0; i < ITERATIONS; i++ )
    {
        cout << echoString;
        cout << "\n--------------------------------------------------" << endl;
        cout << "\nType 0 to exit the game or :-\n" << endl
        << "\t 1) DRAW_FROM_STOCK" << endl
        << "\t 2) DRAW_FROM_DISCARD" << endl
        << "\t 3) STEAL_FROM_PLAYER" << endl
        << "Enter your command: \n";
        cin >> input;

        int command = std::stoi(trim(input));
        string player;
        string ipv4;
        int t_port;
        int p_port;

        Game_Pack comm_to_send = Game_Pack();

        if (command == 0) {
            break;

        } else if (command == 1){
            comm_to_send.comm = DRAW_FROM_STOCK;

        } else if (command == 2){
            comm_to_send.comm = DRAW_FROM_DISCARD;

        } else if (command == 3){
            comm_to_send.comm = STEAL_FROM_PLAYER;
            cout << "debugging" << endl;


        }

        printf( "\n Sending data to the server\n" );
        // cout << comm_to_send.comm;

        // Send the string to the server
        if( sendto( sock, &comm_to_send, sizeof(comm_to_send), 0, (struct sockaddr *) &echoServAddr, sizeof( echoServAddr ) ) != sizeof(comm_to_send) )
       		DieWithError( "client: sendto() sent a different number of bytes than expected" );

        // Receive a response
        fromSize = sizeof( fromAddr );

        if( ( respStringLen = recvfrom( sock, echoString, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize ) ) > ECHOMAX )
            DieWithError( "client: recvfrom() failed" );

        echoString[ respStringLen ] = '\0';
        char * newstrin = &echoString[respStringLen-4];
        if (atoi(newstrin) == 8001) {
            cout << "Game started at port 8001." << endl << endl << "Redirecting to the game ..." << endl;
            PlayGame(8001);
        }
 		
    }
    
    close( sock );
    exit( 0 );
}

int main( int argc, char *argv[] )
{
    size_t nread;
    int sock;                        // Socket descriptor
    struct sockaddr_in echoServAddr; // Echo server address
    struct sockaddr_in fromAddr;     // Source address of echo
    unsigned short echoServPort;     // Echo server port
    unsigned int fromSize;           // In-out of address size for recvfrom()
    char *servIP;                    // IP address of server
    char *echoString = NULL;         // String to send to echo server
    string input = "";         // String to send to echo server
    size_t echoStringLen = ECHOMAX;               // Length of string to echo
    int respStringLen;               // Length of received response

    echoString = (char *) malloc( ECHOMAX );

    if (argc < 3)    // Test for correct number of arguments
    {
        fprintf( stderr, "Usage: %s <Server IP address> <Echo Port>\n", argv[0] );
        exit( 1 );
    }

    servIP = argv[ 1 ];  // First arg: server IP address (dotted decimal)
    echoServPort = atoi( argv[2] );  // Second arg: Use given port

    printf( "client: Arguments passed: server IP %s, port %d\n", servIP, echoServPort );

    // Create a datagram/UDP socket
    if( ( sock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP ) ) < 0 )
        DieWithError( "client: socket() failed" );

    // Construct the server address structure
    memset( &echoServAddr, 0, sizeof( echoServAddr ) ); // Zero out structure
    echoServAddr.sin_family = AF_INET;                  // Use internet addr family
    echoServAddr.sin_addr.s_addr = inet_addr( servIP ); // Set server's IP address
    echoServAddr.sin_port = htons( echoServPort );      // Set server's port

	// Pass string back and forth between server ITERATIONS times

	printf( "client: Echoing strings for %d iterations\n", ITERATIONS );



    for( int i = 0; i < ITERATIONS; i++ )
    {
        cout << echoString;
        cout << "\n--------------------------------------------------" << endl;
        cout << "\nType 0 to exit the game or :-\n" << endl
        << "\t 1) Register a player" << endl
        << "\t 2) Query Players" << endl
        << "\t 3) Start a Game" << endl
        << "\t 4) Query Games" << endl
        << "\t 5) De-register" << endl
        << "\t 6) Debug start" << endl
        << "Enter your command: \n";
        cin >> input;

        int command = std::stoi(trim(input));
        string player;
        string ipv4;
        int t_port;
        int p_port;

        Serv_Pack comm_to_send = Serv_Pack();

        if (command == 0) {
            break;

        } else if (command == 1){
            cout << "Enter the name of the player: ";
            cin >> player;
            cout << "Enter the ipv4 of the tracker: ";
            cin >> ipv4;
            cout << "Enter tracker's port: ";
            cin >> t_port;
            cout << "Enter player's port: ";
            cin >> p_port;
            strcpy(comm_to_send.comm_args.reg.name , player.c_str());
            strcpy(comm_to_send.comm_args.reg.ipv4 , ipv4.c_str());
            comm_to_send.comm_args.reg.t_port = t_port;
            comm_to_send.comm_args.reg.p_port = p_port;
            comm_to_send.comm_args.reg.playing = 0;
            comm_to_send.comm = REGISTER;

        } else if (command == 2){
            comm_to_send.comm = QUERY_PLAYER;

        } else if (command == 3){
            comm_to_send.comm = START;
            cout << "debugging" << endl;


        } else if (command == 4){
            comm_to_send.comm = QUERY_GAME;

        } else if (command == 5){
            cout << "Enter the name of the player: ";
            cin >> player;
            strcpy(comm_to_send.comm_args.drg.player , player.c_str());
            comm_to_send.comm = DEREGISTER;
        } else if (command == 6) {
            cout << "debugging" << endl;
            Game_Pack comm_to_send = Game_Pack();
            comm_to_send.comm = DRAW_FROM_DISCARD;
            // Send the string to the server
        if( sendto( sock, &comm_to_send, sizeof(comm_to_send), 0, (struct sockaddr *) &echoServAddr, sizeof( echoServAddr ) ) != sizeof(comm_to_send) )
       		DieWithError( "client: sendto() sent a different number of bytes than expected" );
        if( sendto( sock, &comm_to_send, sizeof(comm_to_send), 0, (struct sockaddr *) &echoServAddr, sizeof( echoServAddr ) ) != sizeof(comm_to_send) )
       		DieWithError( "client: sendto() sent a different number of bytes than expected" );

        }

        printf( "\n Sending data to the server\n" );
        // cout << comm_to_send.comm;

        // Send the string to the server
        if( sendto( sock, &comm_to_send, sizeof(comm_to_send), 0, (struct sockaddr *) &echoServAddr, sizeof( echoServAddr ) ) != sizeof(comm_to_send) )
       		DieWithError( "client: sendto() sent a different number of bytes than expected" );

        // Receive a response
        fromSize = sizeof( fromAddr );

        if( ( respStringLen = recvfrom( sock, echoString, ECHOMAX, 0, (struct sockaddr *) &fromAddr, &fromSize ) ) > ECHOMAX )
            DieWithError( "client: recvfrom() failed" );

        echoString[ respStringLen ] = '\0';
        char * newstrin = &echoString[respStringLen-4];
        cout << "respStringLen is: " << respStringLen << endl;
        cout << "newstring is: " << newstrin << endl;
        cout << "echosting is: " << echoString << endl;
        if (atoi(newstrin) == 8001) {
            cout << "Game started at port 8001." << endl << endl << "Redirecting to the game ..." << endl;
            PlayGame(8001);
        }
 		
    }
    
    close( sock );
    exit( 0 );
}

