#include <iostream>

using namespace std;

struct Player {
    char name[256];
    char ipv4[16];
    int t_port;
    int p_port;
    bool playing; // For tracker to know if this player is availible for a game
};

struct Game {
    int deck[52];
    Player dealer;
    vector<Player> players;
};

enum Tracker_Comms {
    END,
    REGISTER,
    QUERY_PLAYER,
    START,
    QUERY_GAME,
    DEREGISTER,
};

struct comm_stg_args {
    char player[256];
    int n;
    int holes;
};

struct comm_drg_args {
    char player[256];
};

struct comm_end_args {
    int game_id;
};

struct Serv_Pack {
    Tracker_Comms comm;
    union {
        struct Player reg;
        struct comm_stg_args stg;
        struct comm_drg_args drg;
        struct comm_end_args end;
    } comm_args;
    
};

enum Game_Comms {
    DRAW_FROM_STOCK,
    DRAW_FROM_DISCARD,
    STEAL_FROM_PLAYER,
};

struct Game_Pack {
    Game_Comms comm;
};
