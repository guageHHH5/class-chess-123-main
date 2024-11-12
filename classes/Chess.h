#pragma once
#include "Game.h"
#include "ChessSquare.h"
#include <cstdint>


const int pieceSize = 64;


enum ChessPiece {
    NoPiece = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6
};

//
// the main game class
//
class Chess : public Game
{
public:
    Chess();
    ~Chess();

    // set up the board
    void        setUpBoard() override;

    Player*     checkForWinner() override;
    bool        checkForDraw() override;
    std::string initialStateString() override;
    std::string stateString() override;
    void        setStateString(const std::string &s) override;
    bool        actionForEmptyHolder(BitHolder& holder) override;
    bool        canBitMoveFrom(Bit& bit, BitHolder& src) override;
    bool        canBitMoveFromTo(Bit& bit, BitHolder& src, BitHolder& dst) override;
    void        bitMovedFromTo(Bit &bit, BitHolder &src, BitHolder &dst) override;

    void        stopGame() override;
    BitHolder& getHolderAt(const int x, const int y) override { return _grid[y][x]; }

	void        updateAI() override;
    bool        gameHasAI() override { return true; }
private:
    Bit *       PieceForPlayer(const int playerNumber, ChessPiece piece);
    const char  bitToPieceNotation(int row, int column) const;
    void        loadFromFEN(const std::string &fen);

    ChessSquare      _grid[8][8];
    uint64_t wPieces;
    uint64_t bPieces;
    uint64_t ratt(int sq, uint64_t wPieces, uint64_t bPieces); 
    uint64_t batt(int sq, uint64_t wPieces, uint64_t bPieces); 
    uint64_t katt(int sq, uint64_t wPieces, uint64_t bPieces); 
    uint64_t natt(int sq, uint64_t wPieces, uint64_t bPieces); 
    uint64_t patt(int sq, uint64_t wPieces, uint64_t bPieces); 
    void     updatePieces(uint64_t& pieces, int check);
};

