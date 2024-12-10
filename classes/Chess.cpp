#include "Chess.h"
#include <string>
#include <iostream>

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

    return bit;
}

void Chess::Place(const ChessPiece c, const int x, const int y, const int player){
    Bit* b = PieceForPlayer(player, c);
    b->setPosition(_grid[y][x].getPosition());
    b->setParent(&_grid[y][x]);
    b->setGameTag(player);
    _grid[y][x].setBit(b);

}

void Chess::FENtoBoard(const std::string &st){
    int x = 0;
    int y = 7;
    int player = 0;
    char ch;
    int piecePlayer = isupper(ch) ? 0 : 1;
    Bit* b = PieceForPlayer(player, Rook);
    int i;
    for(i = 0; (unsigned)i < st.length(); i++){
        ch = st.at(i);
        if(y >= 0){
            if(tolower(ch) == 'r'){
                Place(Rook, x, y, piecePlayer);
            } else if(tolower(ch) == 'n'){
                Place(Knight, x, y, piecePlayer);
            } else if(tolower(ch) == 'b'){
                Place(Bishop, x, y, piecePlayer);
            }else if(tolower(ch) == 'q'){
                Place(Queen, x, y, piecePlayer);
            }else if(tolower(ch) == 'k'){
                Place(King, x, y, piecePlayer);
            }else if(tolower(ch) == 'p'){
                Place(Pawn, x, y, piecePlayer);
            }else if(ch == '1'){
                x = x + 0;
            }else if(ch == '2'){
                x = x + 1;
            }else if(ch == '3'){
                x = x + 2;
            }else if(ch == '4'){ 
                x = x + 3;
            }else if(ch == '5'){
                x = x + 4;
            }else if(ch == '6'){
                x = x + 5;
            }else if(ch == '7'){
                x = x + 6;
            }else if(ch == '8'){
                x = x + 7;
            }else if(ch == '/'){
                x = -1;
                y--;
            }else if(ch == ' '){
                y = -1;
            }
            x++;
        }else{
            break;
        }
    }
    int count = 0;
    std::string s = "";
    while((unsigned)i < st.length()){
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
        } else if(count ==2){
            if(st.at(i) == ' '){
                count++;
                i++;
            }else if (st.at(i) == '-'){
                count++;
                i++;
            }else if(st.at(i) >= 'a' && st.at(i) <= 'h'){
                int fileIndex = st.at(i) - 'a';
                int rankIndex = st.at(i+1) - '1';
                enPassantT = &_grid[fileIndex][rankIndex];
                //enPassantT = &_grid[st.at(i) - 'a'][st.at(i+1)];
                count++;
                i = i + 2;
            }else{
                std::cout << "En Passant Invalid";
                count++;
                i++;
            }
        } 
        else if(count == 3){
            if(st.at(i) == ' '){
                count++;
                if(!s.empty()){
                    bool isNumeric = true;
                    for (char c : s){
                        if(!isdigit(static_cast<unsigned char>(c))){
                            isNumeric = false;
                            break;
                        }
                    }
                    if(isNumeric){
                        try{
                            countHalfMove = stoi(s);
                        }
                        catch(...){
                            countHalfMove = 0;
                        }
                    } else {
                        countHalfMove = 0;
                    }
                } else {
                    countHalfMove = 0;
                }
                s.clear();
            } else {
                s += st.at(i);
            }
        } else if(count == 4){
            s += st.at(i);
            if((unsigned)i == st.length()-1){
                count++;
                if(!s.empty()){
                    bool isNumeric = true;
                    for (char c : s){
                        if(!isdigit(static_cast<unsigned char>(c))){
                            isNumeric = false;
                            break;
                        }
                    }
                    if(isNumeric){
                        try{
                            countFullMove = stoi(s);
                        } catch (...){
                            countFullMove = 1;
                        }
                    }else{
                        countFullMove = 1;
                    }
                    
                }
            } else {
                countFullMove = 1;
            }
            s.clear();
            break;
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
            int x, y;
            //same file
            if(src.getColumn() == dst.getColumn()){
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
        int i;
        for(i = 0; i < 8; i++){
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
                if(dst.getColumn() == 6 && dst.getRow() == 7 && wKingCastle){
                    // check if rook is present
                    if(_grid[7][7].bit()->gameTag() == Rook){
                        return true;
                    }
                }
                // another tile
                if(dst.getColumn() == 2 && dst.getRow() == 7 && wQueenCastle){
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
}

