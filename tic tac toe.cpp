#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <cstdlib> 
using namespace std;

#ifdef _WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

class Board {
public:
    char board[3][3] = { {' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '} }; // tạo bàn cờ trống

    bool isFull() const { // check bàn cờ  đầy chưa 
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    return false; // Nếu cón ô trống thì chưa
                }
            }
        }
        return true; // Nếu không có ô trống thì đầy
    }

    bool checkWin(char symbol) const { // check người chơi có thắng không
        for (int i = 0; i < 3; i++) {
            if (board[i][0] == symbol && board[i][1] == symbol && board[i][2] == symbol) return true; // Kiểm tra hàng ngang
            if (board[0][i] == symbol && board[1][i] == symbol && board[2][i] == symbol) return true; // Kiểm tra hàng dọc
        }
        if (board[0][0] == symbol && board[1][1] == symbol && board[2][2] == symbol) return true; // Kiểm tra đường chéo chính
        if (board[0][2] == symbol && board[1][1] == symbol && board[2][0] == symbol) return true; // Kiểm tra đường chéo phụ
        return false;
    }

    void printBoard(const  string& difficulty) const { // In bàn cờ ra màn hình
        system(CLEAR); // Xóa màn hình (xóa các lựa chọn trước )
        cout << "Do kho: " << difficulty <<  endl; // Hiển thị độ khó (sau khi chọn chế độ thì hiển thị độ khó khi đấu với bot)
         cout << "   0 1 2\n";
        for (int i = 0; i < 3; i++) {
             cout << i << " |";
            for (int j = 0; j < 3; j++) {
                 cout << board[i][j] << "|";
            }
             cout <<  endl;
        }
    }

    void reset() { // Đặt lại bàn cờ
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                board[i][j] = ' ';
            }
        }
    }
};

class Player {
public:
    char symbol; // Biểu tượng của người chơi (X hoặc O)

    Player(char s) : symbol(s) {}

    virtual int makeMove(const Board& board) = 0; // Hàm ảo để thực hiện nước đi, sẽ được định nghĩa trong lớp con
};

class HumanPlayer : public Player {
public:
    HumanPlayer(char s) : Player(s) {}

    int makeMove(const Board& board) override { // Hàm thực hiện nước đi của người chơi
        int row, col;
        while (true) {
             cout << "Nhap nuoc di (hang cot): ";
             cin >> row >> col;

            // Kiểm tra nếu nhập ký tự không phải số
            if ( cin.fail()) {
                 cin.clear(); // Xóa cờ lỗi
                 cin.ignore( numeric_limits< streamsize>::max(), '\n'); // Bỏ qua phần nhập còn lại
                 cout << "Vui long nhap so nguyen.\n";
                continue;
            }

            if (row < 0 || row > 2 || col < 0 || col > 2) { // Kiểm tra vị trí hợp lệ
                 cout << "Vi tri khong hop le. Vui long nhap lai.\n";
            }
            else if (board.board[row][col] != ' ') { // Kiểm tra vị trí đã được chọn chưa
                 cout << "Vi tri da duoc chon. Vui long nhap lai.\n";
            }
            else {
                break; // Nếu vị trí hợp lệ thì thoát khỏi vòng lặp
            }
        }
        return row * 3 + col; // Trả về vị trí trên bàn cờ
    }
};

class Robot : public Player {
public:
    enum Difficulty { EASY, MEDIUM, HARD }; // Định nghĩa các mức độ khó
    Difficulty difficulty; // Mức độ khó của robot
     mt19937 rng; // Bộ sinh số ngẫu nhiên

    Robot(char s, Difficulty d) : Player(s), difficulty(d), rng( random_device{}()) {}

    int makeMove(const Board& board) override { // Hàm thực hiện nước đi của robot
        if (difficulty == EASY) {
             uniform_int_distribution<int> dist(0, 8); // Phân phối ngẫu nhiên trong khoảng từ 0 đến 8
            int move;
            do {
                move = dist(rng);
            } while (board.board[move / 3][move % 3] != ' '); // Chọn vị trí ngẫu nhiên cho đến khi tìm được ô trống
            return move;
        }
        else if (difficulty == MEDIUM) {
            return mediumMove(board); // Nước đi mức trung bình
        }
        else { // HARD
            int bestMove = -1;
            int bestValue = - numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    Board newBoard = board;
                    newBoard.board[i / 3][i % 3] = symbol;
                    int moveValue = minimax(newBoard, 0, false, - numeric_limits<int>::max(),  numeric_limits<int>::max());
                    if (moveValue > bestValue) {
                        bestMove = i;
                        bestValue = moveValue;
                    }
                }
            }
            return bestMove;
        }
    }

private:
    int mediumMove(const Board& board) {
        char opponentSymbol = (symbol == 'X') ? 'O' : 'X';

        // 1. Thắng nếu có thể
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = symbol;
                if (newBoard.checkWin(symbol)) {
                    return i;
                }
            }
        }

        // 2. Chặn đối thủ nếu đối thủ có thể thắng
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = opponentSymbol;
                if (newBoard.checkWin(opponentSymbol)) {
                    return i;
                }
            }
        }

        // 3. Tạo 2 trong 1 hàng
        for (int i = 0; i < 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = symbol;
                if (hasTwoInARow(newBoard, symbol)) {
                    return i;
                }
            }
        }

        // 4. Chặn đối thủ tạo 2 trong 1 hàng
        for (int i = 0; 9; i++) {
            if (board.board[i / 3][i % 3] == ' ') {
                Board newBoard = board;
                newBoard.board[i / 3][i % 3] = opponentSymbol;
                if (hasTwoInARow(newBoard, opponentSymbol)) {
                    return i;
                }
            }
        }

        // 5. Nếu không có nước đi thắng hoặc chặn, chọn ngẫu nhiên
         uniform_int_distribution<int> dist(0, 8);
        int move;
        do {
            move = dist(rng);
        } while (board.board[move / 3][move % 3] != ' ');
        return move;
    }

    bool hasTwoInARow(const Board& board, char symbol) const {
        // Kiểm tra các hàng
        for (int i = 0; i < 3; i++) {
            int count = 0;
            for (int j = 0; j < 3; j++) {
                if (board.board[i][j] == symbol) {
                    count++;
                }
            }
            if (count == 2) return true;
        }

        // Kiểm tra các cột
        for (int j = 0; j < 3; j++) {
            int count = 0;
            for (int i = 0; i < 3; i++) {
                if (board.board[i][j] == symbol) {
                    count++;
                }
            }
            if (count == 2) return true;
        }

        // Kiểm tra đường chéo chính
        int count = 0;
        for (int i = 0; i < 3; i++) {
            if (board.board[i][i] == symbol) {
                count++;
            }
        }
        if (count == 2) return true;

        // Kiểm tra đường chéo phụ
        count = 0;
        for (int i = 0; i < 3; i++) {
            if (board.board[i][2 - i] == symbol) {
                count++;
            }
        }
        if (count == 2) return true;

        return false;
    }

    int minimax(Board board, int depth, bool maximizingPlayer, int alpha, int beta) {
        char opponentSymbol = (symbol == 'X') ? 'O' : 'X';

        if (board.checkWin(symbol)) return 10 - depth; // Nếu người chơi thắng, trả về điểm
        if (board.checkWin(opponentSymbol)) return depth - 10; // Nếu đối thủ thắng, trả về điểm âm
        if (board.isFull()) return 0; // Nếu bàn cờ đầy, trả về hòa

        if (maximizingPlayer) {
            int maxEval = - numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    board.board[i / 3][i % 3] = symbol;
                    int eval = minimax(board, depth + 1, false, alpha, beta);
                    board.board[i / 3][i % 3] = ' ';
                    maxEval =  max(maxEval, eval);
                    alpha =  max(alpha, maxEval);
                    if (beta <= alpha) {
                        break; // Cắt tỉa beta
                    }
                }
            }
            return maxEval;
        }
        else {
            int minEval =  numeric_limits<int>::max();
            for (int i = 0; i < 9; i++) {
                if (board.board[i / 3][i % 3] == ' ') {
                    board.board[i / 3][i % 3] = opponentSymbol;
                    int eval = minimax(board, depth + 1, true, alpha, beta);
                    board.board[i / 3][i % 3] = ' ';
                    minEval =  min(minEval, eval);
                    beta =  min(beta, minEval);
                    if (beta <= alpha) {
                        break; // Cắt tỉa alpha
                    }
                }
            }
            return minEval;
        }
    }
};

class Game {
public:
    Board board; // Bàn cờ
    Player* player1; // Người chơi 1
    Player* player2; // Người chơi 2
     string difficulty; // Biến lưu độ khó

    Game() {
        int choice;
         cout << "Chon che do choi:\n";
         cout << "1. Nguoi vs Nguoi\n";
         cout << "2. Nguoi vs May\n";
         cin >> choice;

        board.reset(); // Đặt lại bàn cờ khi bắt đầu trò chơi

        if (choice == 1) {
            player1 = new HumanPlayer('X');
            player2 = new HumanPlayer('O');
            difficulty = "Nguoi vs Nguoi";
        }
        else {
            player1 = new HumanPlayer('X');
             cout << "Chon do kho cho may:\n";
             cout << "1. De\n";
             cout << "2. Trung binh\n";
             cout << "3. Kho\n";
             cin >> choice;
            Robot::Difficulty diff = Robot::EASY;
            if (choice == 2) diff = Robot::MEDIUM;
            else if (choice == 3) diff = Robot::HARD;
            player2 = new Robot('O', diff);
            if (diff == Robot::EASY) difficulty = "De";
            else if (diff == Robot::MEDIUM) difficulty = "Trung binh";
            else difficulty = "Kho";
        }
    }

    ~Game() {
        delete player1;
        delete player2;
    }

    void play() {
        board.reset(); // Đặt lại bàn cờ khi bắt đầu trò chơi
        int currentPlayer = 0;
        while (!board.isFull() && !board.checkWin('X') && !board.checkWin('O')) {
            board.printBoard(difficulty); // In bàn cờ ra màn hình
            int move = (currentPlayer == 0 ? player1 : player2)->makeMove(board);
            board.board[move / 3][move % 3] = (currentPlayer == 0 ? 'X' : 'O');
            currentPlayer = 1 - currentPlayer; // Chuyển lượt chơi
        }

        board.printBoard(difficulty); // In bàn cờ ra màn hình
        if (board.checkWin('X')) {
             cout << "Nguoi choi X thang!\n";
        }
        else if (board.checkWin('O')) {
             cout << "Nguoi choi O thang!\n";
        }
        else {
             cout << "Hoa!\n";
        }
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}
