#define NUM_ROWS 6
#define NUM_COLS 7

// NOTE(Zach): Create a new type called Token
typedef enum {EMPTY, RED, BLUE} Token;

// NOTE(Zach): Return the value of the board at (row, col)
Token checkCell(int row, int col);

// NOTE(Zach): Insert a token into the board, only allow dropping tokens in
// Return 0 for success and -1 for failure
int insertToken(Token token, int col);

// NOTE(Zach): Return row where a token will land when dropped into the board.
// Return -1 for failure.
int insertPosition(int col);

// NOTE(Zach): Empty the board
void emptyBoard(void);
