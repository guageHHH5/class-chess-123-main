#pragma once
#include "Game.h"
#include "ChessSquare.h"
#include <cstdint>


const int pieceSize = 64;

typedef struct {
    int x;
    int y;
} position;

enum ChessPiece {
    NoPiece = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6
};

typedef struct{
    int srcX, srcY;
    int dstX, dstY;
    ChessPiece capturedPiece;
    bool promotionPiece;
    bool enPassant;
} Move;

//
// the main game class
//
class Chess : public Game
{
public:
    Chess();
    ~Chess();

    // set up the board
    void        Place(const ChessPiece c, const int x, const int y, const int player);
    void        FENtoBoard(const std::string &st);
    void        setUpBoard() override;
    void        copyBoard();
    void        makeMove(Move& move);
    void        undoMove(Move& move);
    std::vector<Move> generateMoves(int playerID);

    int         negamax(int depth, int alpha, int beta, int color);
    int         evaluateBoard();
    int         FLIP(int index);


    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder& holder, ChessPiece p);
    position    getPosition(BitHolder& BH);
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    bool        canBitMoveFromToTWO(Bit& bit, BitHolder& src, BitHolder& dst); 
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    void        stopGame() override;
    BitHolder& getHolderAt(const int x, const int y) override { return _grid[y][x]; }

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
private:
    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    const char  bitToPieceNotation(int row, int column) const;
    const char  bitToPieceNotationTWO(int row, int column) const;
    
    ChessSquare      _grid[8][8];
    ChessSquare      _gridTWO[8][8];
    bool        bKingCastle;
    bool        wKingCastle;
    bool        bQueenCastle;
    bool        wQueenCastle;
    BitHolder   *enPassantT;
    int         countHalfMove;
    int         countFullMove;
};

