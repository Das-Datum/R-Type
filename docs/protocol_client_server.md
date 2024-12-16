# Protocol

This table outlines the commands, corresponding binary values, and their descriptions for the protocol used in the system.

| Command      | Binary                                | Description                                  |
|--------------|---------------------------------------|----------------------------------------------|
| **SHT**      | 01010011 01001000 01010100 id;       | Shoot command                               |
| **PSH**      | 01010000 01010011 01001000 id;       | Power shot                                  |
| **SHD**      | 01010011 01001000 01000100 id;       | Player uses its ability                     |
| **KLL**      | 01001011 01001100 01001100 id;       | An enemy is killed                          |
| **DTH**      | 01000100 01010100 01001000 id;       | Player dies                                 |
| **BDT MSG**  | 01000010 01000100 01010100 id,msg;   | Player sends a message to all other players |
| **POS X Y**  | 01010000 01001111 01010011 id,x,y;   | Position of the player                      |
| **NME**      | 01001110 01001101 01000101 id;       | Name of the player                          |
| **SCR**      | 01010011 01000011 01010010 id;       | Score of the player                         |
| **SCM**      | 01010011 01000011 01001101 id;       | Max score or accumulated score of team      |
| **STM**      | 01010011 01000011 01001101 id,val;   | Change time elapsed                         |
| **GTM**      | 01000111 01010100 01001101 id;       | Retrieve time from server                   |
| **ENG**      | 01000101 01001110 01000111 id;       | All players are dead                        |
| **SVR MSG**  | 01010011 01010110 01010010 id,msg;   | Server sends a message                      |
| **NNM NAME** | 01001110 01001110 01001101 id,name;  | New player with their name                  |
| **QIT**      | 01010001 01001001 01010100 id;       | Player quits the server                     |
| **ERN NAME** | 01000101 01010010 01001110 id,name;  | Error with player name                      |
| **MUP**      | 01001101 01010101 01010000 id;       | Player moves up                             |
| **MDW**      | 01001101 01000100 01010111 id;       | Player moves down                           |
| **MLF**      | 01001101 01001100 01000110 id;       | Player moves left                           |
| **MRT**      | 01001101 01010010 01010100 id;       | Player moves right                          |
| **BGN**      | 01000010 01000111 01001110 id;       | Server sends Begin                          |
