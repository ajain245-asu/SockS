#include <iostream>

using namespace std;

struct Player {
    char name[256];
    string ipv4;
    int t_port;
    int p_port;
};

struct Game {
    int deck[52];
    Player dealer;
    vector<Player> players;
};

enum Game_Comms {
    END,
    REGISTER,
    QUERY_PLAYER,
    START,
    QUERY_GAME,
    DEREGISTER,
};

struct comm_reg_args {
    char player[256];
    char ipv4[16];
    int t_port;
    int p_port;
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

struct Serv_Comms {
    Game_Comms comm;
    union {
        struct comm_reg_args reg;
        struct comm_stg_args stg;
        struct comm_drg_args drg;
        struct comm_end_args end;
    } comm_args;
    
};
