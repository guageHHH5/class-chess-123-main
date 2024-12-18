#include "Chess.h"
#include <string>
#include "Evaluate.h"

const int AI_PLAYER = 1;
const int HUMAN_PLAYER = -1;

Chess::Chess()
{
}

Chess::~Chess()
{
}

//
// make a chess piece for the player
//
Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    // depending on playerNumber load the "x.png" or the "o.png" graphic
    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("chess/") + (playerNumber == 0 ? "w_" : "b_") + pieceName;

    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    //std::cerr << "Created piece: " << pieceName << " for player: " << playerNumber << "\n";
    return bit;
}

void Chess::Place(const ChessPiece c, const int x, const int y, const int player){
    Bit* b = PieceForPlayer(player, c);
    if (!b) {
        std::cerr << "Failed to create piece for player " << player << " at (" << x << ", " << y << ")\n";
    }      
    b->setPosition(_grid[y][x].getPosition());
    b->setParent(&_grid[y][x]);
    b->setGameTag(c);
    _grid[y][x].setBit(b);
    if (!_grid[y][x].bit()) {
        std::cerr << "Piece not placed on grid at (" << x << ", " << y << ")\n";
    }
    std::cerr << "Placing piece type: " << c << " for player: " << player << " at (" << x << ", " << y << ")\n";
}

void Chess::FENtoBoard(const std::string &st){
    int x = 0;
    int y = 7;
    int player = 0;
    char c;
    Bit* bit = PieceForPlayer(player, Rook);
    int i;
    for(i = 0; i < st.length(); i++){
        c = st.at(i);
        if(y >= 0){
            switch(c){
                case 'r':
                    Place(Rook, x, y, 1);
                    break;
                case 'n':
                    Place(Knight, x, y, 1);
                    break;
                case 'b':
                    Place(Bishop, x, y, 1);
                    break;
                case 'q':
                    Place(Queen, x, y, 1);
                    break;
                case 'k':
                    Place(King, x, y, 1);
                    break;
                case 'p':
                    Place(Pawn, x, y, 1);
                    break;
                case 'R':
                    Place(Rook, x, y, 0);
                    break;
                case 'N':
                    Place(Knight, x, y, 0);
                    break;
                case 'B':
                    Place(Bishop, x, y, 0);
                    break;
                case 'Q':
                    Place(Queen, x, y, 0);
                    break;
                case 'K':
                    Place(King, x, y, 0);
                    break;
                case 'P':
                    Place(Pawn, x, y, 0);
                    break;
                case '1':
                    x = x + 0;
                    break;
                case '2':
                    x = x + 1;
                    break;
                case '3':
                    x = x + 2;
                    break;
                case '4':
                    x = x + 3;
                    break;
                case '5':
                    x = x + 4;
                    break;
                case '6':
                    x = x + 5;
                    break;
                case '7':
                    x = x + 6;
                    break;
                case '8':
                    x = x + 7;
                    break;
                case '/':
                    x = -1;
                    y--;
                    break;
                case ' ':
                    y = -1;
                    break;
            }
            x++;
        }
        else{
            break;
        }
    }
    int count = 0;
    std::string s = "";
    while(i < st.length()){
        if(count == 0){
            if(st.at(i) == 'b'){
                endTurn();
            }
            count++;
            i++;
        } else if(count == 1){
            if(st.at(i) == '-'){
                count++;
                i++;
            } else if(st.at(i) == 'k'){
                bKingCastle = true;
            }else if(st.at(i) == 'q'){
                bQueenCastle = true;
            }else if(st.at(i) == 'K'){
                wKingCastle = true;
            }else if(st.at(i) == 'Q'){
                wQueenCastle = true;
            }else if(st.at(i) == ' '){
                count++;
            }else{
                std::cout << "Castling Invalid";
            }
        } else if(count == 2){
            if (st.at(i) == '-'){
                count++;
                i++;
            }else if(st.at(i) >= 'a' && st.at(i) <= 'g'){
                enPassantT = &_grid[st.at(i) - 'a'][st.at(i+1)];
                count++;
                i = i + 2;
            }else{
                std::cout << "En Passant Invalid";
            }
        } 
        else if(count == 3){
            if(st.at(i) == ' '){
                count++;
                countHalfMove = stoi(s);
                s = "";
            }
            else{
                s += st.at(i);
            }
        }   
        else if(count == 4){
            s += st.at(i);
            if(i == st.length()-1){
                count++;
                countFullMove = stoi(s);
                s = "";
            }
        }
        i++;
    }
}


void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
    //
    // we want white to be at the bottom of the screen so we need to reverse the board
    //
    char piece[2];
    piece[1] = 0;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            ImVec2 position((float)(pieceSize * x + pieceSize), (float)(pieceSize * (_gameOptions.rowY - y) + pieceSize));
            _grid[y][x].initHolder(position, "boardsquare.png", x, y);
            _grid[y][x].setGameTag(0);
            piece[0] = bitToPieceNotation(y,x);
            _grid[y][x].setNotation(piece);
        }
    }
    
    
    bKingCastle = false;
    bQueenCastle = false;
    wKingCastle = false;
    wQueenCastle = false; 
    enPassantT = NULL;
    countHalfMove = 0;
    countFullMove = 1;

    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

}



//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool Chess::actionForEmptyHolder(BitHolder &holder, ChessPiece p)
{
    if(holder.bit()){
        return false;
    }

    Bit *bit = PieceForPlayer(getCurrentPlayer()->playerNumber(), p);
    bit->setPosition(holder.getPosition());
    bit->setParent(&holder);
    bit->setGameTag(p);
    holder.setBit(bit);

    return true;
}

position Chess::getPosition(BitHolder &BH){
    for(int y = 0; y < _gameOptions.rowY; y++){
        for(int x = 0; x < _gameOptions.rowX; x++){
            if(_grid[y][x].getPosition().x == BH.getPosition().x && _grid[y][x].getPosition().y == BH.getPosition().y){
                position p;
                p.x = x;
                p.y = y;
                return p;
            }
        }
    }
    position p;
    p.x = -1;
    p.y = -1;
    return p;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    return bit.getOwner()->playerNumber() == getCurrentPlayer()->playerNumber();
}



bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    //std::cout << "Validating move for piece type: " << bit.gameTag() << "\n";
    //std::cout << "Source: (" << src.getColumn() << ", " << src.getRow() << ") Destination: (" << dst.getColumn() << ", " << dst.getRow() << ")\n";

    //return true; // For debugging, temporarily allow all moves
    // chess movement

    
    if(bit.gameTag() == Pawn){
        int player = bit.getOwner()->playerNumber();
        if(player == 0){
            std::cout << src.getColumn() << src.getRow() << dst.getColumn() << dst.getRow() << std::endl;
            std::cout << (src.getRow() == 1) << (src.getColumn() == dst.getColumn()) << (src.getRow() == dst.getRow() - 2) << dst.empty() << _grid[src.getRow()+1][src.getColumn()].empty() << std::endl;

            // double move implementation
            if(src.getRow() == 1 && src.getColumn() == dst.getColumn() && src.getRow() == dst.getRow() - 2 && dst.empty() && _grid[src.getRow()+1][src.getColumn()].empty()){
                return true;
            }

            // capture implementation
            if((src.getColumn() == dst.getColumn() - 1 || src.getColumn() == dst.getColumn() + 1) && src.getRow() == dst.getRow() - 1 && dst.bit()){
                return true;
            }

            // en passant
            if(enPassantT != NULL && dst.getColumn() == enPassantT->getColumn() && dst.getRow() == enPassantT->getRow() + 1){
                if(src.getRow() == enPassantT->getRow() && (src.getColumn() == dst.getColumn() - 1 || src.getColumn() == dst.getColumn() + 1)){
                    return true;
                }
            }
            // pawn push
            return dst.empty() && src.getColumn() == dst.getColumn() && src.getRow() == dst.getRow() - 1;
        }
        if(player == 1){
            // double move implementation
            if(src.getRow() == 6 && src.getColumn() == dst.getColumn() && src.getRow() == dst.getRow() + 2 && dst.empty() && _grid[src.getRow() - 1][src.getColumn()].empty()){
                return true;
            }

            // capture
            if((src.getColumn() == dst.getColumn() - 1 || src.getColumn() == dst.getColumn() + 1) && src.getRow() == dst.getRow() + 1 && dst.bit()){
                return true;
            }

            // en passant
            if(enPassantT != NULL && dst.getColumn() == enPassantT->getColumn() && dst.getRow() == enPassantT->getRow() - 1){
                if(src.getRow() == enPassantT->getRow() && (src.getColumn() == dst.getColumn() - 1 || src.getColumn() == dst.getColumn() + 1)){
                    return true;
                }
            }

            // pawn push
            return dst.empty() && src.getColumn() == dst.getColumn() && src.getRow() == dst.getRow() + 1;
        }
        return false;
    }
    

    if(bit.gameTag() == Knight){
        int NMove[8][2] = {
            {2, 1},
            {2, -1},
            {-2, -1},
            {-2, -2},
            {1, 2},
            {1, -2},
            {-1, 2},
            {-1, -2}
        };
        for(int i = 0; i < 8; i++){
            if(src.getColumn() + NMove[i][0] == dst.getColumn() && src.getRow() + NMove[i][1] == dst.getRow()){
                return true;
            }
        }
        return false;
    }
    if(bit.gameTag() == Bishop){
        int x, y;
        if(src.getColumn() < dst.getColumn()){
            x = 1;
        } else {
            x = -1;
        }
        if(src.getRow() < dst.getRow()){
            y = 1;
        } else {
            y = -1;
        }
        int srcColumn = src.getColumn();
        int srcRow = src.getRow();
        while(srcRow >= 0 && srcRow < 8){
            srcColumn = srcColumn + x;
            srcRow = srcRow + y;
            if(srcColumn == dst.getColumn() && srcRow == dst.getRow()){
                return true;
            }
            if(_grid[srcRow][srcColumn].bit() != nullptr){
                return false;
            }
        }
        return false;
    }
    if(bit.gameTag() == Rook){
        if((src.getColumn() == dst.getColumn()) != (src.getRow() == dst.getRow())){
            
            //same file
            if(src.getColumn() == dst.getColumn()){
                int y;
                if(src.getRow() < dst.getRow()){
                    y = 1;
                } else {
                    y = -1;
                }
                int srcRow = src.getRow();
                while(srcRow >= 0 && srcRow < 8){
                    srcRow = srcRow + y;
                    if(srcRow == dst.getRow()){
                        return true;
                    }
                    if(_grid[srcRow][src.getColumn()].bit() != nullptr){
                        return false;
                    }
                }
                return false;
            // same rank    
            } else {
                int x;
                if(src.getColumn() < dst.getColumn()){
                    x = 1;
                } else {
                    x = -1;
                }
                int srcColumn = src.getColumn();
                while(srcColumn >= 0 && srcColumn < 8){
                    srcColumn = srcColumn + x;
                    if(srcColumn == dst.getColumn()){
                        return true;
                    }
                    if(_grid[src.getRow()][srcColumn].bit() != nullptr){
                        return false;
                    }
                }
                return false;
            }
        }
        return false;
    }
    if(bit.gameTag() == Queen){
        bool isQueen = false;
        bit.setGameTag(Bishop);
        if(canBitMoveFromTo(bit, src, dst)){
            isQueen = true;
        }
        bit.setGameTag(Rook);
        if(canBitMoveFromTo(bit, src, dst)){
            isQueen = true;
        }
        bit.setGameTag(Queen);
        return isQueen;
    }
    

    
    if(bit.gameTag() == King){
        int KMove[8][2] = {
            {1, 1},
            {1, 0},
            {1, -1},
            {0, 1},
            {0, -1},
            {-1, 1},
            {-1, 0},
            {-1, -1}
        };
        for(int i = 0; i < 8; i++){
            if(src.getColumn() + KMove[i][0] == dst.getColumn() && src.getRow() + KMove[i][1] == dst.getRow()){
                return true;
            }
        }
        int getPlayerID = getCurrentPlayer()->playerNumber();
        if(getPlayerID == 0){
            // check if king home
            if(src.getColumn() == 4 && src.getRow() == 0){
                // check if correct tile was chosen
                if(dst.getColumn() == 6 && dst.getRow() == 0 && wKingCastle){
                    // check if rook is present
                    if(_grid[0][7].bit()->gameTag() == Rook){
                        return true;
                    }
                }
                // another tile
                if(dst.getColumn() == 2 && dst.getRow() == 0 && wQueenCastle){
                    // check if rook is present
                    if(_grid[0][0].bit()->gameTag() == Rook){
                        return true;
                    }
                }
            }
        }
        if(getPlayerID == 1){
            // check if king home
            if(src.getColumn() == 4 && src.getRow() == 7){
                // check if correct tile was chosen
                if(dst.getColumn() == 6 && dst.getRow() == 7 && bKingCastle){
                    // check if rook is present
                    if(_grid[7][7].bit()->gameTag() == Rook){
                        return true;
                    }
                }
                // another tile
                if(dst.getColumn() == 2 && dst.getRow() == 7 && bQueenCastle){
                    // check if rook is present
                    if(_grid[7][0].bit()->gameTag() == Rook){
                        return true;
                    }
                }
            }
        }

        return false;
    }
    return false;
 
}



void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst){
    int getPlayerID = getCurrentPlayer()->playerNumber();
    if(bit.gameTag() == King){
        if(getPlayerID == 0){
            if(src.getColumn() == 4 && src.getRow() == 0){
                // check if correct tile was chosen
                if(dst.getColumn() == 6 && dst.getRow() == 0){
                    // check if rook is present
                    if(_grid[0][7].bit()->gameTag() == Rook){
                        actionForEmptyHolder(_grid[0][5], Rook);
                        _grid[0][7].destroyBit();
                    }
                }
                // another tile
                if(dst.getColumn() == 2 && dst.getRow() == 0){
                    // check if rook is present
                    if(_grid[0][0].bit()->gameTag() == Rook){
                        actionForEmptyHolder(_grid[0][3], Rook);
                        _grid[0][0].destroyBit();
                    }
                }
            }
        }
        if(getPlayerID == 1){
            if(src.getColumn() == 4 && src.getRow() == 7){
                // check if correct tile was chosen
                if(dst.getColumn() == 6 && dst.getRow() == 7){
                    // check if rook is present
                    if(_grid[7][7].bit()->gameTag() == Rook){
                        actionForEmptyHolder(_grid[7][5], Rook);
                        _grid[7][7].destroyBit();
                    }
                }
                // another tile
                if(dst.getColumn() == 2 && dst.getRow() == 7){
                    // check if rook is present
                    if(_grid[7][0].bit()->gameTag() == Rook){
                        actionForEmptyHolder(_grid[7][3], Rook);
                        _grid[7][0].destroyBit();
                    }
                }
            }
        }
    }
    // promote to queen
    else if(bit.gameTag() == Pawn){
        if(getPlayerID == 0){
            if(src.getColumn() == dst.getColumn() && src.getRow() == dst.getRow() - 2){
                enPassantT = &dst;
            } else if(enPassantT != NULL && dst.getColumn() == enPassantT->getColumn() && dst.getRow() == enPassantT->getRow() + 1){
                if(src.getRow() == enPassantT->getRow() && (src.getColumn() == dst.getColumn() - 1 || src.getColumn() == dst.getColumn() + 1)){
                    _grid[enPassantT->getRow()][enPassantT->getColumn()].destroyBit();
                    enPassantT = NULL;
                }
            } else {
                enPassantT = NULL;
            }
            if(dst.getRow() == 7){
                _grid[dst.getRow()][dst.getColumn()].destroyBit();
                Place(Queen, dst.getColumn(), dst.getRow(), 0);
            }
        }
        if(getPlayerID == 1){
            if(src.getColumn() == dst.getColumn() && src.getRow() == dst.getRow() + 2){
                enPassantT = &dst;
            } else if(enPassantT != NULL && dst.getColumn() == enPassantT->getColumn() && dst.getRow() == enPassantT->getRow() - 1){
                if(src.getRow() == enPassantT->getRow() && (src.getColumn() == dst.getColumn() - 1 || src.getColumn() == dst.getColumn() + 1)){
                    _grid[enPassantT->getRow()][enPassantT->getColumn()].destroyBit();
                    enPassantT = NULL;
                }
            } else {
                enPassantT = NULL;
            }
            if(dst.getRow() == 0){
                _grid[dst.getRow()][dst.getColumn()].destroyBit();
                Place(Queen, dst.getColumn(), dst.getRow(), 1);
            }
        }
    }
    endTurn();
}

int Chess::FLIP(int index){
    return 63 - index;
}

void Chess::makeMove(Move& move){
    //src and dst coords
    BitHolder& src = _grid[move.srcY][move.srcX];
    BitHolder& dst = _grid[move.dstY][move.dstX];

    move.capturedPiece = dst.bit();

    dst.setBit(src.bit());
    dst.bit()->setPosition(dst.getPosition());
    src.setBit(nullptr);

    if(move.promotionPiece != NoPiece){
        dst.bit()->setGameTag(move.promotionPiece);
    }
    if(move.enPassant){
        int capturedPawnY = move.dstY + (dst.bit()->getOwner()->playerNumber() == 0 ? -1 : 1);
        _grid[capturedPawnY][move.dstX].destroyBit();
    }
}

void Chess::undoMove(Move& move){
    BitHolder& src = _grid[move.srcY][move.srcX];
    BitHolder& dst = _grid[move.dstY][move.dstX];

    src.setBit(dst.bit());
    src.bit()->setPosition(src.getPosition());
    dst.setBit(move.capturedPiece);

    if(move.promotionPiece != NoPiece){
        src.bit()->setGameTag(Pawn);
    }
    if(move.enPassant){
        int capturedPawnY = move.dstY + (dst.bit() ? -1 : 1);
        _grid[capturedPawnY][move.dstY].setBit(move.capturedPiece);
    }
}

std::vector<Move> Chess::generateMoves(){
    std::vector<Move> moves;

    for (int y = 0; y < 8; ++y){
        for(int x = 0; x < 8; ++x){
            BitHolder& src = _grid[y][x];
            Bit* bit = src.bit();

            if(bit && canBitMoveFrom (*bit, src)){
                for(int dy = 0; dy < 8; ++dy){
                    for(int dx = 0; dx < 8; ++dx){
                        BitHolder& dst = _grid[dy][dx];

                        if(canBitMoveFromTo(*bit, src, dst)){
                            Move move;
                            move.srcX = x;
                            move.srcY = y;
                            move.dstX = dx;
                            move.dstY = dy;
                            move.capturedPiece = dst.bit();
                            move.promotionPiece = NoPiece;
                            move.enPassant = (bit->gameTag() == Pawn && enPassantT != nullptr && enPassantT->getPosition().x == dst.getPosition().x && enPassantT->getPosition().y == dst.getPosition().y);

                            if(bit->gameTag() == Pawn && (dy == 0 || dy == 7)){
                                for(int promoPiece = Queen; promoPiece <= Knight; ++promoPiece){
                                    move.promotionPiece = static_cast<ChessPiece>(promoPiece);
                                    moves.push_back(move);
                                }
                            } else {
                                moves.push_back(move);
                            }
                        }
                    }
                }
            }
        }
    }

    return moves;
}

int Chess::evaluateBoard(){
    static std::map<char, int> pieceValues = {
        {'P', 100}, {'p', -100},
        {'N', 300}, {'n', -300},
        {'B', 330}, {'b', -330},
        {'R', 500}, {'r', -500},
        {'Q', 900}, {'q', -900},
        {'K', 20000}, {'k', -20000}
    };

    int score = 0;
    for(int y = 0; y < 8; ++y) {
        for(int x = 0; x < 8; ++x){
            char piece = bitToPieceNotation(y, x);
            score += pieceValues[piece];

            int posIndex = (y*8) + x;
            switch(piece){
                case 'P': 
                    score += pawnTable[posIndex];
                    break;
                case 'p':
                    score -= pawnTable[FLIP(posIndex)];
                    break;
                case 'N': 
                    score += knightTable[posIndex];
                    break;
                case 'n':
                    score -= knightTable[FLIP(posIndex)];
                    break;
                case 'B': 
                    score += bishopTable[posIndex];
                    break;
                case 'b':
                    score -= bishopTable[FLIP(posIndex)];
                    break;
                case 'R': 
                    score += rookTable[posIndex];
                    break;
                case 'r':
                    score -= rookTable[FLIP(posIndex)];
                    break;
                case 'Q': 
                    score += queenTable[posIndex];
                    break;
                case 'q':
                    score -= queenTable[FLIP(posIndex)];
                    break;
                case 'K': 
                    score += kingTable[posIndex];
                    break;
                case 'k':
                    score -= kingTable[FLIP(posIndex)];
                    break;
            }
        }
    }
    return score;
}

int Chess::negamax(int depth, int alpha, int beta, int color){
    if(depth == 0){
        return color * evaluateBoard();
    }

    int maxEval = -1000000;
    auto moves = generateMoves();
    for(auto& move : moves){
        makeMove(move);
        int eval = -negamax(depth - 1, -beta, -alpha, -color);
        undoMove(move);

        maxEval = std::max(maxEval, eval);
        alpha = std::max(alpha, eval);

        if(alpha >= beta){
            break;
        }
    }
    return maxEval;
}


//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

Player* Chess::checkForWinner()
{
    // check to see if either player has won
    return nullptr;
}

bool Chess::checkForDraw()
{
    // check to see if the board is full
    return false;
}

//
// add a helper to Square so it returns out FEN chess notation in the form p for white pawn, K for black king, etc.
// this version is used from the top level board to record moves
//
const char Chess::bitToPieceNotation(int row, int column) const {
    if (row < 0 || row >= 8 || column < 0 || column >= 8) {
        return '0';
    }

    const char* wpieces = { "?PNBRQK" };
    const char* bpieces = { "?pnbrqk" };
    unsigned char notation = '0';
    Bit* bit = _grid[row][column].bit();
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag() & 127];
    } else {
        notation = '0';
    }
    return notation;
}

//
// state strings
//
std::string Chess::initialStateString()
{
    return stateString();
}

//
// this still needs to be tied into imguis init and shutdown
// we will read the state string and store it in each turn object
//
std::string Chess::stateString()
{
    std::string s;
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            s += bitToPieceNotation(y, x);
        }
    }
    return s;
}

//
// this still needs to be tied into imguis init and shutdown
// when the program starts it will load the current game from the imgui ini file and set the game state to the last saved state
//
void Chess::setStateString(const std::string &s)
{
    for (int y = 0; y < _gameOptions.rowY; y++) {
        for (int x = 0; x < _gameOptions.rowX; x++) {
            int index = y * _gameOptions.rowX + x;
            int playerNumber = s[index] - '0';
            if (playerNumber) {
                _grid[y][x].setBit(PieceForPlayer(playerNumber - 1, Pawn));
            } else {
                _grid[y][x].setBit(nullptr);
            }
        }
    }
}


//
// this is the function that will be called by the AI
//
void Chess::updateAI() 
{
    int bestVal = -1000000;
    Move bestMove;
    int depth = 3;

    auto moves = generateMoves();
    for (auto& move : moves){
        makeMove(move);
        int moveVal = -negamax(depth - 1, -1000000, 1000000, 1);
        undoMove(move);

        if(moveVal > bestVal){
            bestVal = moveVal;
            bestMove = move;
        }
    }

    makeMove(bestMove);
}

