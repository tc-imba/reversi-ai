#include <utility>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

#define BOARD_SIZE 8
#define EMPTY 0
#define ME 1
#define OTHER 2

#define START "START"
#define PLACE "PLACE"
#define DONE  "DONE"
#define TURN  "TURN"
#define BEGIN "BEGIN"
#define END   "END"
#define PASS  "PASS"

struct Position {
    int x;
    int y;
};

ostream &operator<<(ostream &out, const Position &pos) {
    return out << (char) ('a' + pos.y) << " " << (1 + pos.x);
}

class AI {
private:
    /*
     * You can define your own struct and variable here
     * 你可以在这里定义你自己的结构体和变量
     */
    size_t boardSize;
    vector<vector<int> > &board;
    const vector<pair<int, int> > directions = {
            {1,  -1},
            {1,  0},
            {1,  1},
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {0,  -1},
            {0,  1}
    };
public:
    explicit AI(vector<vector<int> > &board);

    void init();

    Position begin();

    Position turn(const Position &other);

    bool canrev(int i, int j, int who);

    int reverseDirection(int i, int j, int who, pair<int, int> dir, bool reverse = false);

    bool putChess(int i, int j, int who);
};

AI::AI(vector<vector<int> > &board) : board(board) {
    this->boardSize = board.size();
}

class REPL {
private:
    vector<vector<int> > board;
    AI ai;

    const char symbol[3] = {' ', 'X', 'O'};
public:
    REPL();

    ~REPL() = default;

    void printBoard();

    void loop();
};

REPL::REPL() : ai(board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        board.emplace_back(move(vector<int>(BOARD_SIZE, 0)));
    }
}

void REPL::printBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(stderr, "   %c", 'a' + i);
    }
    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(stderr, "\n ");
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(stderr, "+---");
        }
        fprintf(stderr, "+\n%c", '1' + i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            fprintf(stderr, "| %c ", symbol[board[i][j]]);
        }
        fprintf(stderr, "|");
    }
    fprintf(stderr, "\n ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        fprintf(stderr, "+---");
    }
    fprintf(stderr, "+\n");
}

void REPL::loop() {
    while (true) {
        string buffer;
        getline(cin, buffer);
        if (buffer.empty()) continue;
        istringstream ss(buffer);

        string command;
        ss >> command;
        if (command == START) {
            ai.init();
        } else if (command == PLACE) {
            int x, y, z;
            char temp;
            ss >> temp >> y >> z;
            x = temp - 'a';
            --y;
            board[y][x] = z;
        } else if (command == DONE) {
            cout << "OK" << endl;
        } else if (command == BEGIN) {
            Position pos = ai.begin();
            ai.putChess(pos.x, pos.y, ME);
            cout << pos << endl;
        } else if (command == TURN) {
            int x, y;
            char temp;
            ss >> temp >> y;
            x = temp - 'a';
            --y;
            if (ai.putChess(y, x, OTHER)) {
                Position pos = ai.turn({x, y});
                if (pos.x >= 0) {
                    ai.putChess(pos.x, pos.y, ME);
                    cout << pos << endl;
                } else {
                    cout << PASS << endl;
                }
            } else {
                cout << "input error" << endl;
            }
        } else if (command == PASS) {
            Position pos = ai.turn({0, 0});
            if (pos.x >= 0) {
                ai.putChess(pos.x, pos.y, ME);
                cout << pos << endl;
            } else {
                cout << PASS << endl;
            }
        } else if (command == END) {
            break;
        }
        //printBoard();
    }
}

/*
 * YOUR CODE BEGIN
 * 你的代码开始
 */

bool AI::putChess(int i, int j, int who) {
    if (i < 0 || i >= boardSize || j < 0 || j >= boardSize) return false;
    if (board[i][j] != EMPTY) return false;
    bool flag = false;
    for (auto &dir: directions) {
        int num = reverseDirection(i, j, who, dir);
        if (num) {
            flag = true;
            reverseDirection(i, j, who, dir, true);
        }
    }
    if (flag) {
        board[i][j] = who;
    }
    return flag;
}

int AI::reverseDirection(int i, int j, int who, pair<int, int> dir, bool reverse) {
    int x = i + dir.first;
    int y = j + dir.second;
    int num = 0;
    bool success = false;
    int other = 3 - who;
    while (x >= 0 && x < boardSize && y >= 0 && y < boardSize) {
        if (board[x][y] == other) {
            num++;
            if (reverse) board[x][y] = who;
        } else if (board[x][y] == who && num > 0) {
            success = true;
            break;
        } else {
            break;
        }
        x += dir.first;
        y += dir.second;
    }
    if (!success) return 0;
    return num;
}

bool AI::canrev(int i, int j, int who) {
    if (i < 0 || i >= boardSize || j < 0 || j >= boardSize) return false;
    if (board[i][j] != EMPTY) return false;
    for (auto &dir: directions) {
        int num = reverseDirection(i, j, who, dir);
        if (num) return true;
    }
    return false;
}

/*
 * You should init your AI here
 * 在这里初始化你的AI
 */
void AI::init() {
    this->boardSize = board.size();
}


/*
 * Game Start, you will put the first chess.
 * Warning: This method will only be called when after the initialize of the map,
 * it is your turn to put the chess, or this method will not be called.
 * You should return a valid Position variable.
 * 棋局开始，首先由你来落子
 * 请注意：只有在当棋局初始化后，轮到你落子时才会触发这个函数。如果在棋局初始化完毕后，轮到对手落子，则这个函数不会被触发。详见项目要求。
 * 你需要返回一个结构体Position，在x属性和y属性填上你想要落子的位置。
 */
Position AI::begin() {
    return turn({0, 0});
}

Position AI::turn(const Position &other) {
    /*
     * TODO: Write your own ai here!
     * Here is a simple AI which just put chess at empty position!
     * 代做：在这里写下你的AI。
     * 这里有一个示例AI，它只会寻找第一个可下的位置进行落子。
     */
    int i, j;
    Position preferedPos = {-1, -1};

    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
            if (EMPTY == board[i][j] && canrev(i, j, ME)) {
                preferedPos.x = i;
                preferedPos.y = j;
                return preferedPos;
            }
        }
    }

    return preferedPos;
}

/*
 * YOUR CODE END
 * 你的代码结束
 */

int main(int argc, char *argv[]) {
    REPL repl;
    repl.loop();
    return 0;
}