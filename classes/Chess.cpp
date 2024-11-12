#include "Chess.h"
#include <cmath>

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
            piece[0] = bitToPieceNotation(y,x);
            _grid[y][x].setNotation(piece);
        }
    }
    
    
    wPieces = 0;
    bPieces = 0;
    loadFromFEN("RNBQKBNR/PPPPPPPP/8/8/8/8/pppppppp/rnbqkbnr");

    startGame();

}

void Chess::loadFromFEN(const std::string &fen){
    static const std::unordered_map<char, ChessPiece> pieceMap = {
        {'p', Pawn}, {'r', Rook}, {'n', Knight}, {'b', Bishop}, {'q', Queen}, {'k', King}
    };
    int positionCount = 0;
    

    for(char ch : fen){
        if(ch == ' '){
            break;
        }
        if(ch == ' '){
            continue;
        }
        else if(isdigit(ch)){
            positionCount += ch - '0';
        }
        else{
            int x = positionCount/8;
            int y = positionCount%8;
            bool isWhite = isupper(ch);
            char lowerChar = tolower(ch);
            auto piece = pieceMap.find(lowerChar);

            if (piece != pieceMap.end()){
                Bit* bit = PieceForPlayer(isWhite ? 0: 1, piece->second);
                bit->setPosition(_grid[x][y].getPosition());
                bit->setParent(&_grid[x][y]);
                bit->setGameTag(isWhite ? piece->second : piece->second + 128);
                _grid[x][y].setGameTag(isWhite ? piece->second : piece->second + 128);
                _grid[x][y].setBit(bit);
                (isWhite ? wPieces : bPieces) |=(1ULL << positionCount);
            }
            positionCount++;
        }
    }
}



//
// about the only thing we need to actually fill out for tic-tac-toe
//
bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    return true;
}

bool Chess::canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst)
{
    int pieceTag = (bit.gameTag() > 128) ? bit.gameTag() - 128 : bit.gameTag();
    ChessSquare* srcSqu = dynamic_cast<ChessSquare*>(&src);
    ChessSquare* dstSqu = dynamic_cast<ChessSquare*>(&dst);
    int x = srcSqu->getColumn();
    int y = srcSqu->getRow();
    int total = (8*y)+x;
    int x2 = dstSqu->getColumn();
    int y2 = dstSqu->getRow();
    int total2 = (8*y2)+x2;
    
    
    int p = getCurrentPlayer()->playerNumber();
    bool isWhite = (bit.gameTag() < 128) ? true : false;
    uint64_t checkPieces = p == 0 ? wPieces : bPieces;
    bool playable = (checkPieces & (1ULL << total)) != 0;

    if((isWhite && p == 0) || (!isWhite && p == 1)){
        if (playable){
            uint64_t result = 0;
            switch(pieceTag){
                case Rook:
                    result = ratt(total, wPieces, bPieces);
                    break;
                case Bishop:
                    result = batt(total, wPieces, bPieces);
                    break;
                case Queen:
                    result = ratt(total, wPieces, bPieces) | batt(total, wPieces, bPieces);
                    break;
                case Pawn:
                    result = patt(total, wPieces, bPieces);
                    break;
                case Knight:
                    result = natt(total, wPieces, bPieces);
                    break;
                case King:
                    result = katt(total, wPieces, bPieces);
                    break;
                default:
                    return false;
            }
            return (result & (1ULL << total2)) != 0;
        }
    }
    return false;
}

void Chess::bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) {
    int pieceTag = (bit.gameTag() > 128) ? bit.gameTag() - 128 : bit.gameTag();
    ChessSquare* srcSqu = dynamic_cast<ChessSquare*>(&src);
    ChessSquare* dstSqu = dynamic_cast<ChessSquare*>(&dst);
    int x = srcSqu->getColumn();
    int y = srcSqu->getRow();
    int total = (8*y)+x;
    int x2 = dstSqu->getColumn();
    int y2 = dstSqu->getRow();
    int total2 = (8*y2)+x2;

    updatePieces(wPieces, 0);
    updatePieces(bPieces, 0);

    endTurn();
}

//
// free all the memory used by the game on the heap
//
void Chess::stopGame()
{
}

uint64_t Chess::ratt(int sq, uint64_t wPieces, uint64_t bPieces){
    uint64_t result = 0ULL;
    uint64_t block = wPieces | bPieces;
    int rank = sq / 8, file = sq % 8, r, f;
    int getPiece = _grid[rank][file].gameTag();
    bool isBlack = (getPiece > 128) ? ((getPiece -= 128), true) : false;
    uint64_t blockedPieces = isBlack ? wPieces : bPieces;

    //Up movement
    for(r = rank + 1; r <= 7; r++){
        if(block & (1ULL << (file + r * 8))){
            if(blockedPieces & (1ULL << (file + r * 8))){
                result |= (1ULL << (file + r * 8));
            }
            break;
        }
        result |= (1ULL << (file + r * 8));
    }

    //Down movement
    for(r = rank - 1; r >= 0; r--){
        if(block & (1ULL << (file + r * 8))){
            if(blockedPieces & (1ULL << (file + r * 8))){
                result |= (1ULL << (file + r * 8));
            }
            break;
        }
        result |= (1ULL << (file + r * 8));
    }

    //right movement
    for(f = file + 1; f <= 7; f++){
        if(block & (1ULL << (f + rank * 8))){
            if(blockedPieces & (1ULL << (f + rank * 8))){
                result |= (1ULL << (f + rank * 8));
            }
            break;
        }
        result |= (1ULL << (f + rank * 8));
    }

    //left movement
    for(f = file - 1; f >= 0; f--){
        if(block & (1ULL << (f + rank * 8))){
            if(blockedPieces & (1ULL << (f + rank * 8))){
                result |= (1ULL << (f + rank * 8));
            }
            break;
        }
        result |= (1ULL << (f + rank * 8));
    }

    return result;
}

uint64_t Chess::batt(int sq, uint64_t wPieces, uint64_t bPieces){
    uint64_t result = 0ULL;
    uint64_t block = wPieces | bPieces;
    int rank = sq / 8, file = sq % 8, r, f;
    int getPiece = _grid[rank][file].gameTag();
    bool isBlack = (getPiece > 128) ? ((getPiece -= 128), true) : false;
    uint64_t blockedPieces = isBlack ? wPieces : bPieces;

    //UpRight
    for(r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++){
        if(block & (1ULL << (f + r * 8))){
            if(blockedPieces & (1ULL << (f + r * 8))){
                result |= (1ULL << (f + r * 8));
            }
            break;
        }
        result |= (1ULL << (f + r * 8));
    }

    //DownRight
    for(r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++){
        if(block & (1ULL << (f + r * 8))){
            if(blockedPieces & (1ULL << (f + r * 8))){
                result |= (1ULL << (f + r * 8));
            }
            break;
        }
        result |= (1ULL << (f + r * 8));
    }

    //DownLeft
    for(r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--){
        if(block & (1ULL << (f + r * 8))){
            if(blockedPieces & (1ULL << (f + r * 8))){
                result |= (1ULL << (f + r * 8));
            }
            break;
        }
        result |= (1ULL << (f + r * 8));
    }

    //UpLeft
    for(r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--){
        if(block & (1ULL << (f + r * 8))){
            if(blockedPieces & (1ULL << (f + r * 8))){
                result |= (1ULL << (f + r * 8));
            }
            break;
        }
        result |= (1ULL << (f + r * 8));
    }
    return result;
}

//the knight can move two squares in any direction vertically, followed by one square horizontally, or two squares in any direction horizontally, followed by one square vertically.
uint64_t Chess::natt(int sq, uint64_t wPieces, uint64_t bPieces){
    uint64_t result = 0ULL;
    int getPiece = _grid[sq/8][sq%8].gameTag();
    bool isBlack = (getPiece > 128) ? ((getPiece -= 128), true) : false;
    uint64_t blockedPieces = isBlack ? wPieces : bPieces;

    //Movement offsets for the Knight Piece
    int movement[8] = {17, 15, 10, 6, -6, -10, -15, -17};

    for (int move : movement){
        int target = sq +move;

        if(target < 0 || target > 63){
            continue;
        }

        //For Edge Cases
        int sqY = sq % 8;
        int targetY = target % 8;
        if(abs(targetY - sqY) > 2) continue;

        uint64_t targetPosition = 1ULL << target;

        //check if target square is blocked by the same color
        if(!(blockedPieces & targetPosition)){
            result |= targetPosition;
        }
    }

    return result;
}

uint64_t Chess::katt(int sq, uint64_t wPieces, uint64_t bPieces){
    uint64_t result = 0ULL;
    int x = sq/8;
    int y = sq%8;
    int getPiece = _grid[x][y].gameTag();
    bool isBlack = (getPiece > 128) ? ((getPiece -= 128), true) : false;
    uint64_t blockedPieces = isBlack ? wPieces : bPieces;

    //Movement offsets for the Knight Piece
    int movement[8] = {7, 8, 9, -1, 1, -9, -8, -7};

    for(int move : movement){
        int target = sq + move;
        if(target < 0 || target > 63){
            continue;
        }

        //for edge cases
        if((y == 0 && (move == -1 || move == -9 || move == 7)) || 
        (y == 7 && (move == -71 || move == -7 || move == 9))){
            continue;
        }
        uint64_t targetPosition = 1ULL << target;
        result |= (blockedPieces & targetPosition) ? 0ULL : targetPosition;
    }
    return result;

}

uint64_t Chess::patt(int sq, uint64_t wPieces, uint64_t bPieces) {
    uint64_t result = 0ULL;
    int rank = sq / 8, file = sq % 8;
    int whatPiece = _grid[rank][file].gameTag();
    bool isBlack = (whatPiece > 128) ? true : false;
    uint64_t block = wPieces | bPieces;
    uint64_t blockPieces = isBlack ? wPieces : bPieces; 

    if(!isBlack){
        if(rank == 1 && !(block & (1ULL << (sq + 8))) && !(block & (1ULL << (sq + 16)))) {
            result |= (1ULL << (sq + 16));
        }
        if(_grid[rank+1][file].gameTag() <= 0) {
            result |= (1ULL << (sq + 8));
        }
        if((block & (1ULL << (sq + 7)))) {
            if(blockPieces & (1ULL << (sq + 7))) { 
                result |= (1ULL << (sq + 7)); 
            }
        }
        if((block & (1ULL << (sq + 9)))) {
            if(blockPieces & (1ULL << (sq + 9))) { 
                result |= (1ULL << (sq + 9)); 
            }
        }
    }
    if(isBlack){
        if(rank == 6 && !(block & (1ULL << (sq - 8))) && !(block & (1ULL << (sq - 16)))) {
            result |= (1ULL << (sq - 16));
        }
        if(_grid[rank-1][file].gameTag() <= 0) {
            result |= (1ULL << (sq - 8));
        }
        if((block & (1ULL << (sq - 7)))) {
            if(blockPieces & (1ULL << (sq - 7))) { 
                result |= (1ULL << (sq - 7)); 
            }
        }
        if((block & (1ULL << (sq - 9)))) {
            if(blockPieces & (1ULL << (sq - 9))) { 
                result |= (1ULL << (sq - 9)); 
            }
        }
    }


    return result;
    
}

//O = white piece, 1 = black piece
void Chess::updatePieces(uint64_t& pieces, int check){
    for(int y = 0; y < 8; ++y){
        for(int x = 0; x < 8; ++x){
            int gameTag = _grid[y][x].gameTag();
            if(check == 0 && gameTag > 0 && gameTag < 128){
                int bitPos = y * 8 + x;
                pieces |= (1ULL << bitPos);
            }
            else if(check == 1 && gameTag > 128){
                int bitPos = y * 8 + x;
                pieces |= (1ULL << bitPos);
            }
        }
    }
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

