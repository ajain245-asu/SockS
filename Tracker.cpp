//Name: Dhyey Sanghvi and Aarav Jain
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
#include <random>
#include <thread>

using namespace std;
#define ECHOMAX 255     // Longest string to echo

vector<Game> games;
vector<Player> players;

void DieWithError( const char *errorMessage ) // External error handling function
{
    perror( errorMessage );
    exit( 1 );
}

// Function to generate a deck of 52 playing cards as a vector of integers.
vector<int> getCardDeck() {

    // Init the deck
    vector<int> deck;

    // Populating the deck of cards
    for (int i = 0 ; i < 52 ; i++) {
        deck.push_back(i + 1);
    }

    // Shuffling once using Fisher-Yates shuffle algorithm
    for (int i = 0; i < deck.size() - 1; i++) {
        int j = i + rand() % (deck.size() - i);
        swap(deck[i], deck[j]);
    }
    // Shuffling once again using Fisher-Yates shuffle algorithm just to be sure
    for (int i = 0; i < deck.size() - 1; i++) {
        int j = i + rand() % (deck.size() - i);
        swap(deck[i], deck[j]);
    }

    //// Apparantly we could also do this
    // Initialize random number generator
    random_device rd;
    mt19937 g(rd());

    // Shuffle the vector
    shuffle(deck.begin(), deck.end(), g);

    // Shuffling yet again using Fisher-Yates shuffle algorithm cuz 
    // third time's the charm plus i dont trust the shuffle func
    for (int i = 0; i < deck.size() - 1; i++) {
        int j = i + rand() % (deck.size() - i);
        swap(deck[i], deck[j]);
    }


    return deck;
}

void TheGameThePlayTheEverything(int dealer, int t_port) {
    printf("TheGameThePlayTheEverything:- %ld :: %ld", (long)getpid(), (long)getppid());

    vector<int> deck = getCardDeck();
    // cout << "Shuffled vector: ";
    // for (int num : deck) {
    //     cout << num << " ";
    // }
    // cout << endl;


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
        // if( sendto( sock, echoBuffer, strlen( echoBuffer ), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) != strlen( echoBuffer ) )
        //     DieWithError( "server: sendto() sent a different number of bytes than expected" );
    }
    // NOT REACHED */

}

int main( int argc, char *argv[] )
{
    printf("main:- %ld :: %ld", (long)getpid(), (long)getppid());
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
        Serv_Pack comm_to_recv{};

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

            string output = "Registered player, " + string(playa.name);
            cout << output << endl;

            if( sendto( sock, output.c_str(), output.size(), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) == output.size() ) {
                cout << "Sent data to hopefully start the game" << endl;
            }

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

            bool err = true;
            vector<int> availibes;
            int dealer;

            for (int i = 0 ; i < players.size() ; i++) {
                cout << "sent ppplayer name is " << comm_to_recv.comm_args.stg.player << endl;
                cout << "player i name is : " << players[i].name << endl;
                cout << "player i tport is : " << players[i].t_port << endl;
                if (strcmp(comm_to_recv.comm_args.stg.player , players[i].name) == 0) {
                    cout << "exectuted here" << endl;
                    err = false;
                    dealer = i;
                }
                if (players[i].playing == false) {
                    availibes.push_back(i);
                }
                if (comm_to_recv.comm_args.stg.n > 3 || comm_to_recv.comm_args.stg.n < 1) {
                    err = true;
                    cout << "err in n " << endl;
                }
            }

            if (availibes.size() < comm_to_recv.comm_args.stg.n) {
                err = true;
                cout << "err in avaible size" << "availible size is " << availibes.size() << endl << "n is " << comm_to_recv.comm_args.stg.n << endl;
            }

            if (comm_to_recv.comm_args.stg.holes > 9 || comm_to_recv.comm_args.stg.n < 1) {
                err = true;
                cout << "Wrong holes" << endl;
            }

            if (err) {
                cout << endl << "Error in starting game" << endl;
            } else {

                int newPort = games.size() + 400;
                newPort += trackerPort;
                string output = "SUCCESS!\nGame commencing on port:";
                output += to_string(newPort);

                cout << "output : " << output << endl << "newport : " << newPort << endl << "trackerPort : " << trackerPort << endl << "games.size: " << games.size() << endl;

                for (int i = 0 ; i < availibes.size() ; i++) {
                    if( sendto( players[availibes[i]].t_port, output.c_str(), output.size(), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) == output.size() ) {
                        cout << "Sent data to hopefully start the game" << endl;

                        struct Game game;
                        game.port = newPort;
                        game.players.push_back(players.back());
                        game.dealer = dealer;
                        players.back().p_port;

                        thread game_thread(TheGameThePlayTheEverything, dealer, newPort);
                        game_thread.detach();
                    }
                }
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
        // if( sendto( sock, echoBuffer, strlen( echoBuffer ), 0, (struct sockaddr *) &playerAddr, sizeof( playerAddr ) ) != strlen( echoBuffer ) )
        //     DieWithError( "server: sendto() sent a different number of bytes than expected" );
    }
    // NOT REACHED */
}
