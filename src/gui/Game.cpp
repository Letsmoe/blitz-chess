#include "utils/filesystem.cpp"
#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

enum class PieceType
{
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_PAWN,
    NONE
};

enum class Color {
    NONE,
    WHITE,
    BLACK
};



struct Piece{
    sf::Sprite piece;
    PieceType type;
    int x;
    int y;
    Color color;
    bool enPassant;
};

class Game
{
public:
    sf::RenderWindow window;
    std::string position;           // Position in FEN
    sf::RectangleShape board[8][8]; // Array für die Schachbrettfelder
    sf::Texture pieceTextures[12];  // Array für die Texturen der Figuren
    // Initialize the pieces array with -1
    Piece pieces[8][8] = {{{.piece = sf::Sprite(), .type = PieceType::NONE, .color = Color::NONE, .enPassant = false}}}; // Array für die Schachfiguren
    // Create a transparent texture for empty fields
    sf::Texture emptyTexture;
    bool isAllowed;
    bool castleK=true;
    bool castleQ=true;
    bool castlek=true;
    bool castleq=true;
    std::tuple<int,int> lastPosition;
    sf::Texture leer;
    Color toMove=Color::WHITE;
    int whiteKingX=4;
    int whiteKingY=7;
    int blackKingX=4;
    int blackKingY=0;

    Game()
        : window(sf::VideoMode(800, 800), "Blitz Chess"), position("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
    {
        loadTextures();
        initializeBoard();
    }

    void printMoves(const std::vector<std::tuple<int, int>>& moves) {
        for(const auto& move : moves) {
            std::cout << "(" << std::get<0>(move) << ", " << std::get<1>(move) << ") ";
        }
        std::cout << std::endl;
    }

    void loadTextures()
    {
        for (int i = 0; i < 12; i++)
        {
            std::filesystem::path executableDirectory = getAbsoluteExecutableDirectory();
            pieceTextures[i].loadFromFile(executableDirectory / "assets" / (std::to_string(i) + ".png"));
        }
    }

    void initializeBoard()
    {
        int windowWidth = window.getSize().x;
        int windowHeight = window.getSize().y;

        // Since we need 64 equally sized fields, we need to determine which size is smaller
        float fieldSize = windowWidth < windowHeight ? windowWidth / 8 : windowHeight / 8;

        // We want to center the board in the middle of the window
        int boardY = (windowHeight - fieldSize * 8) / 2;
        int boardX = (windowWidth - fieldSize * 8) / 2;

        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                board[i][j] = sf::RectangleShape(sf::Vector2f(fieldSize, fieldSize));
                board[i][j].setPosition(boardX + j * fieldSize, boardY + i * fieldSize);
                // Check if this field should be white
                if ((i + j) % 2 == 0)
                {
                    board[i][j].setFillColor(sf::Color(238, 238, 210));
                }
                else
                {
                    board[i][j].setFillColor(sf::Color(118, 150, 86)); // Dunkelgrün
                }
            }
        }
    }

    void interpret()
    {
        leer.create(1, 1);
        int row = 0; // Starten Sie bei der letzten Reihe (Zeile 8)
        int col = 0; // Starten Sie in der ersten Spalte (A)

        for (char c : position)
        {
            if (c == ' ')
            {
                // Informationen über die Möglichkeit der Rochade etc. erreicht
                break;
            }
            else if (c == '/')
            {
                // Neue Zeile erreicht, gehen Sie zur nächsten Zeile und setzen Sie die Spalte auf 0 zurück
                row++;
                col = 0;
            }
            else
            {
                
                if (isdigit(c)) {
                    for (int i = 0; i < c - '0'; i++) {
                        pieces[row][col].type = PieceType::NONE;
                        pieces[row][col].color = Color::NONE;
                        pieces[row][col].piece.setTextureRect(sf::IntRect());
                        pieces[row][col].x = col;
                        pieces[row][col].y = row;
                        col++;
                    }
                    
                    continue;
                } else {
                    switch (c)
                    {
                    case 'k':
                        pieces[row][col].piece.setTexture(pieceTextures[0]); // z. B. Textur für schwarzen König
                        pieces[row][col].type=PieceType::BLACK_KING;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::BLACK;
                        break;
                    case 'K':
                        pieces[row][col].piece.setTexture(pieceTextures[6]); // z. B. Textur für weißen König
                        pieces[row][col].type=PieceType::WHITE_KING;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;   
                        pieces[row][col].color = Color::WHITE;
                        break;
                    case 'q':
                        pieces[row][col].piece.setTexture(pieceTextures[1]); // z. B. Textur für schwarze Dame
                        pieces[row][col].type=PieceType::BLACK_QUEEN;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::BLACK;
                        break;
                    case 'Q':
                        pieces[row][col].piece.setTexture(pieceTextures[7]); // z. B. Textur für weiße Dame
                        pieces[row][col].type=PieceType::WHITE_QUEEN;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::WHITE;
                        break;
                    case 'r':
                        pieces[row][col].piece.setTexture(pieceTextures[2]); // z. B. Textur für schwarzen Turm
                        pieces[row][col].type=PieceType::BLACK_ROOK;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::BLACK;
                        break;
                    case 'R':
                        pieces[row][col].piece.setTexture(pieceTextures[8]); // z. B. Textur für weißen Turm
                        pieces[row][col].type=PieceType::WHITE_ROOK;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::WHITE;
                        break;
                    case 'b':
                        pieces[row][col].piece.setTexture(pieceTextures[3]); // z. B. Textur für schwarzen Läufer
                        pieces[row][col].type=PieceType::BLACK_BISHOP;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::BLACK;
                        break;
                    case 'B':
                        pieces[row][col].piece.setTexture(pieceTextures[9]); // z. B. Textur für weißen Läufer
                        pieces[row][col].type=PieceType::WHITE_BISHOP;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::WHITE;
                        break;
                    case 'n':
                        pieces[row][col].piece.setTexture(pieceTextures[4]); // z. B. Textur für schwarzen Springer
                        pieces[row][col].type=PieceType::BLACK_KNIGHT;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::BLACK;
                        break;
                    case 'N':
                        pieces[row][col].piece.setTexture(pieceTextures[10]); // z. B. Textur für weißen Springer
                        pieces[row][col].type=PieceType::WHITE_KNIGHT;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::WHITE;
                        break;
                    case 'p':
                        pieces[row][col].piece.setTexture(pieceTextures[5]); // z. B. Textur für schwarzen Bauer
                        pieces[row][col].type=PieceType::BLACK_PAWN;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::BLACK;
                        break;
                    case 'P':
                        pieces[row][col].piece.setTexture(pieceTextures[11]); // z. B. Textur für weißen Bauer
                        pieces[row][col].type=PieceType::WHITE_PAWN;
                        pieces[row][col].x=col;
                        pieces[row][col].y=row;
                        pieces[row][col].color = Color::WHITE;
                        break;
                    default:
                        pieces[row][col].type = PieceType::NONE;
                        pieces[row][col].color = Color::NONE;
                        break;
                    }

                    int fieldSize = window.getSize().x < window.getSize().y ? window.getSize().x / 8 : window.getSize().y / 8;

                    int boardY = (window.getSize().y - fieldSize * 8) / 2;
                    int boardX = (window.getSize().x - fieldSize * 8) / 2;

                    float scale = (float)fieldSize / pieceTextures[0].getSize().x;

                    pieces[row][col].piece.setScale(scale, scale); // Skalieren Sie die Größe der Sprites
                    pieces[row][col].piece.setPosition(boardX + col * fieldSize, boardY + row * fieldSize);
                    col++;
                }
            }
        }
    }

    std::string reinterpret(){
        std::string token = position.substr(position.find(' '),position.length()-1);
        int count=1;
        std::string tmp="";
        for (int i=0;i<8;i++){
            for (int j=0;j<8;j++){
                    switch (pieces[i][j].type){
                        case PieceType::NONE:
                            break;
                        case PieceType::WHITE_KING:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="K";
                           count=0;
                           break; 
                        case PieceType::WHITE_QUEEN:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="Q";
                           count=0;
                           break;
                        case PieceType::WHITE_ROOK:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="R";
                           count=0;
                           break;
                        case  PieceType::WHITE_BISHOP:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="B";
                           count=0;
                           break;
                        case PieceType::WHITE_KNIGHT:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="N";
                           count=0;
                           break;
                        case PieceType::WHITE_PAWN:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="P";
                           count=0;
                           break;
                        case PieceType::BLACK_KING:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="k";
                           count=0;
                           break; 
                        case PieceType::BLACK_QUEEN:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="q";
                           count=0;
                           break;
                        case PieceType::BLACK_ROOK:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="r";
                           count=0;
                           break;
                        case PieceType::BLACK_BISHOP:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="b";
                           count=0;
                           break;
                        case PieceType::BLACK_KNIGHT:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="n";
                           count=0;
                           break;
                        case PieceType::BLACK_PAWN:
                           count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                           tmp+="p";
                           count=0;
                           break;
                        default:
                            PieceType::NONE;
                }
                count++;
                if (j==7){
                    count!=1 ? tmp+=std::to_string(count-1) : tmp+="";
                    i!=7 ? tmp+="/" : tmp+="";
                    count=1;
                }
            }
        }
        tmp+=token;
        return tmp;
    }

    bool isMoveInList(const std::vector<std::tuple<int, int>>& moves, int target_x, int target_y) {
        return std::find_if(moves.begin(), moves.end(), [&](const auto& tuple) {
            return std::get<0>(tuple) == target_x && std::get<1>(tuple) == target_y;
        }) != moves.end();
    }

    std::vector<std::tuple<int, int>> generatePossibleMoves(Piece& piece){
        std::vector<std::tuple<int, int>> moves;

        Color oppositeColor = piece.color == Color::WHITE ? Color::BLACK : Color::WHITE;

        switch (piece.type){
            case PieceType::WHITE_KING:
            case PieceType::BLACK_KING:
                if (pieces[piece.y-1][piece.x].color != piece.color && piece.y-1>=0 ){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x));
                }
                if (pieces[piece.y-1][piece.x-1].color != piece.color && piece.y-1>=0 && piece.x-1 >= 0){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x-1));
                }
                if (pieces[piece.y-1][piece.x+1].color != piece.color && piece.y-1>=0 && piece.x+1<8){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x+1));
                }
                if (pieces[piece.y][piece.x-1].color != piece.color && piece.x-1>=0){
                    moves.push_back(std::make_tuple(piece.y, piece.x-1));
                }
                if (pieces[piece.y][piece.x+1].color != piece.color && piece.x+1<8){
                    moves.push_back(std::make_tuple(piece.y, piece.x+1));
                }
                if (pieces[piece.y+1][piece.x].color != piece.color && piece.y+1<8){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x));
                }
                if (pieces[piece.y+1][piece.x+1].color != piece.color && piece.y+1<8 && piece.x+1<8){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x+1));
                }
                if (pieces[piece.y+1][piece.x-1].color != piece.color && piece.y+1<8 && piece.x-1>=0){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x-1));
                }
                if (piece.type==PieceType::BLACK_KING && castlek && pieces[piece.y][piece.x+1].type == PieceType::NONE && pieces[piece.y][piece.x+2].type == PieceType::NONE){
                    moves.push_back(std::make_tuple(piece.y, piece.x+2));
                }
                if (piece.type==PieceType::BLACK_KING && castleq && pieces[piece.y][piece.x-1].type == PieceType::NONE && pieces[piece.y][piece.x-2].type == PieceType::NONE && pieces[piece.y][piece.x-3].type == PieceType::NONE){
                    moves.push_back(std::make_tuple(piece.y, piece.x-2));
                }
                if (piece.type==PieceType::WHITE_KING && castleK && pieces[piece.y][piece.x+1].type == PieceType::NONE && pieces[piece.y][piece.x+2].type == PieceType::NONE){
                    moves.push_back(std::make_tuple(piece.y, piece.x+2));
                }
                if (piece.type==PieceType::WHITE_KING && castleQ && pieces[piece.y][piece.x-1].type == PieceType::NONE && pieces[piece.y][piece.x-2].type == PieceType::NONE && pieces[piece.y][piece.x-3].type == PieceType::NONE){
                    moves.push_back(std::make_tuple(piece.y, piece.x-2));
                }
                break;
            case PieceType::WHITE_ROOK:
            case PieceType::BLACK_ROOK:
                for (int i=piece.x+1;i<8;i++){
                    if (pieces[piece.y][i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y, i)); 
                    }
                    else if(pieces[piece.y][i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y, i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=piece.x-1;i>=0;i--){
                    if (pieces[piece.y][i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y, i)); 
                    }
                    else if(pieces[piece.y][i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y, i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=piece.y+1;i<8;i++){
                    if (pieces[i][piece.x].color == Color::NONE){
                        moves.push_back(std::make_tuple(i, piece.x)); 
                    }
                    else if(pieces[i][piece.x].color == oppositeColor){
                        moves.push_back(std::make_tuple(i, piece.x));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=piece.y-1;i>=0;i--){
                    if (pieces[i][piece.x].color == Color::NONE){
                        moves.push_back(std::make_tuple(i, piece.x)); 
                    }
                    else if(pieces[i][piece.x].color == oppositeColor){
                        moves.push_back(std::make_tuple(i, piece.x));
                        break;
                    }
                    else{
                        break;
                    }
                }
                break;
            case PieceType::WHITE_BISHOP:
            case PieceType::BLACK_BISHOP:
                for (int i=1;i<8;i++){//Unten Rechts
                    if (piece.y+i > 7 || piece.x+i > 7){
                        break;
                    }
                    else if (pieces[piece.y+i][piece.x+i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x+i)); 
                    }
                    else if(pieces[piece.y+i][piece.x+i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x+i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Unten Links
                    if (piece.y+i > 7 || piece.x-i < 0){
                        break;
                    }
                    else if (pieces[piece.y+i][piece.x-i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x-i)); 
                    }
                    else if(pieces[piece.y+i][piece.x-i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x-i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Oben Rechts
                    if (piece.y-i < 0 || piece.x+i > 7){
                        break;
                    }
                    else if (pieces[piece.y-i][piece.x+i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x+i)); 
                    }
                    else if(pieces[piece.y-i][piece.x+i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x+i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Oben Links
                    if (piece.y-i < 0 || piece.x-i < 0){
                        break;
                    }
                    else if (pieces[piece.y-i][piece.x-i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x-i)); 
                    }
                    else if(pieces[piece.y-i][piece.x-i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x-i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                break;
            case PieceType::WHITE_KNIGHT:
            case PieceType::BLACK_KNIGHT:
                if (pieces[piece.y-1][piece.x+2].color != piece.color && piece.y-1>=0 && piece.x+2<8){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x+2));
                }
                if (pieces[piece.y-1][piece.x-2].color != piece.color && piece.y-1>=0 && piece.x-2 >= 0){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x-2));
                }
                if (pieces[piece.y+1][piece.x+2].color != piece.color && piece.y+1<8 && piece.x+2<8){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x+2));
                }
                if (pieces[piece.y+1][piece.x-2].color != piece.color && piece.y+1<8 && piece.x-2>=0){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x-2));
                }
                if (pieces[piece.y-2][piece.x+1].color != piece.color && piece.y-2>=0 && piece.x+1<8){
                    moves.push_back(std::make_tuple(piece.y-2, piece.x+1));
                }
                if (pieces[piece.y-2][piece.x-1].color != piece.color && piece.y-2>=0 && piece.x-1 >= 0){
                    moves.push_back(std::make_tuple(piece.y-2, piece.x-1));
                }
                if (pieces[piece.y+2][piece.x+1].color != piece.color && piece.y+2<8 && piece.x+1<8){
                    moves.push_back(std::make_tuple(piece.y+2, piece.x+1));
                }
                if (pieces[piece.y+2][piece.x-1].color != piece.color && piece.y+2<8 && piece.x-1>=0){
                    moves.push_back(std::make_tuple(piece.y+2, piece.x-1));
                }
                break;
            case PieceType::WHITE_QUEEN:
            case PieceType::BLACK_QUEEN:
                for (int i=piece.x+1;i<8;i++){
                    if (pieces[piece.y][i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y, i)); 
                    }
                    else if(pieces[piece.y][i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y, i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=piece.x-1;i>=0;i--){
                    if (pieces[piece.y][i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y, i)); 
                    }
                    else if(pieces[piece.y][i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y, i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=piece.y+1;i<8;i++){
                    if (pieces[i][piece.x].color == Color::NONE){
                        moves.push_back(std::make_tuple(i, piece.x)); 
                    }
                    else if(pieces[i][piece.x].color == oppositeColor){
                        moves.push_back(std::make_tuple(i, piece.x));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=piece.y-1;i>=0;i--){
                    if (pieces[i][piece.x].color == Color::NONE){
                        moves.push_back(std::make_tuple(i, piece.x)); 
                    }
                    else if(pieces[i][piece.x].color == oppositeColor){
                        moves.push_back(std::make_tuple(i, piece.x));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Unten Rechts
                    if (piece.y+i > 7 || piece.x+i > 7){
                        break;
                    }
                    else if (pieces[piece.y+i][piece.x+i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x+i)); 
                    }
                    else if(pieces[piece.y+i][piece.x+i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x+i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Unten Links
                    if (piece.y+i > 7 || piece.x-i < 0){
                        break;
                    }
                    else if (pieces[piece.y+i][piece.x-i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x-i)); 
                    }
                    else if(pieces[piece.y+i][piece.x-i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y+i, piece.x-i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Oben Rechts
                    if (piece.y-i < 0 || piece.x+i > 7){
                        break;
                    }
                    else if (pieces[piece.y-i][piece.x+i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x+i)); 
                    }
                    else if(pieces[piece.y-i][piece.x+i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x+i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                for (int i=1;i<8;i++){//Oben Links
                    if (piece.y-i < 0 || piece.x-i < 0){
                        break;
                    }
                    else if (pieces[piece.y-i][piece.x-i].color == Color::NONE){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x-i)); 
                    }
                    else if(pieces[piece.y-i][piece.x-i].color == oppositeColor){
                        moves.push_back(std::make_tuple(piece.y-i, piece.x-i));
                        break;
                    }
                    else{
                        break;
                    }
                }
                break;
            case PieceType::WHITE_PAWN:
                if (piece.y==6 && pieces[piece.y-2][piece.x].type == PieceType::NONE && pieces[piece.y-1][piece.x].type == PieceType::NONE){
                    moves.push_back(std::make_tuple(piece.y-2, piece.x)); 
                }
                if (pieces[piece.y-1][piece.x].type == PieceType::NONE && (piece.y-1)>=0){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x));
                }
                if (pieces[piece.y-1][piece.x+1].color == oppositeColor && (piece.y-1)>=0){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x+1));
                }
                if (pieces[piece.y-1][piece.x-1].color == oppositeColor && (piece.y-1)>=0){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x-1));
                }
                if (pieces[piece.y][piece.x+1].color == oppositeColor && (piece.y-1)>0 && pieces[piece.y][piece.x+1].enPassant==true){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x+1));
                }
                if (pieces[piece.y][piece.x-1].color == oppositeColor && (piece.y-1)>0 && pieces[piece.y][piece.x-1].enPassant==true){
                    moves.push_back(std::make_tuple(piece.y-1, piece.x-1));
                }
                break;
            case PieceType::BLACK_PAWN:
                if (piece.y==1 && pieces[piece.y+2][piece.x].type == PieceType::NONE && pieces[piece.y+1][piece.x].type == PieceType::NONE){
                    moves.push_back(std::make_tuple(piece.y+2, piece.x)); 
                }
                if (pieces[piece.y+1][piece.x].type == PieceType::NONE && (piece.y+1)>0){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x));
                }
                if (pieces[piece.y+1][piece.x+1].color == oppositeColor && (piece.y+1)>0){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x+1));
                }
                if (pieces[piece.y+1][piece.x-1].color == oppositeColor && (piece.y+1)>0){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x-1));
                }
                if (pieces[piece.y][piece.x+1].color == oppositeColor && (piece.y+1)>0 && pieces[piece.y][piece.x+1].enPassant==true){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x+1));
                }
                if (pieces[piece.y][piece.x-1].color == oppositeColor && (piece.y+1)>0 && pieces[piece.y][piece.x-1].enPassant==true){
                    moves.push_back(std::make_tuple(piece.y+1, piece.x-1));
                }
                break;
        }  
        return moves; 
    }

    bool allowedMove(Piece& piece, int target_x, int target_y, std::vector<std::tuple<int, int>> moves){
        if (isMoveInList(moves, target_y, target_x)) {
            if(piece.color==Color::BLACK && toMove==Color::WHITE || piece.color==Color::WHITE && toMove==Color::BLACK){
                return false;
            }
            if(piece.x==target_x && piece.y==target_y){
                return false;
            }
            // Reset old position to color none
            if(piece.type == PieceType::BLACK_PAWN && piece.y == target_y-2){
                piece.enPassant=true;
            }
            else if(piece.type == PieceType::WHITE_PAWN && piece.y == target_y+2){
                piece.enPassant=true;
            }
            if(piece.type == PieceType::WHITE_PAWN && target_x==piece.x+1 &&  pieces[get<0>(lastPosition)][get<1>(lastPosition)].enPassant==true || piece.type == PieceType::WHITE_PAWN && target_x==piece.x-1 && pieces[get<0>(lastPosition)][get<1>(lastPosition)].enPassant==true){
                pieces[get<0>(lastPosition)][get<1>(lastPosition)].type = PieceType::NONE;
                pieces[get<0>(lastPosition)][get<1>(lastPosition)].color = Color::NONE;
            }
            if(piece.type == PieceType::BLACK_PAWN && target_x==piece.x+1 && pieces[get<0>(lastPosition)][get<1>(lastPosition)].enPassant==true || piece.type == PieceType::BLACK_PAWN && target_x==piece.x-1 && pieces[get<0>(lastPosition)][get<1>(lastPosition)].enPassant==true){
                pieces[get<0>(lastPosition)][get<1>(lastPosition)].type = PieceType::NONE;
                pieces[get<0>(lastPosition)][get<1>(lastPosition)].color = Color::NONE;
            }
            if(piece.type == PieceType::BLACK_KING && target_x==piece.x+2){//k
                pieces[0][5] = pieces[0][7];
                pieces[0][7].color = Color::NONE;
                pieces[0][7].type = PieceType::NONE;
            }
            if(piece.type == PieceType::BLACK_ROOK && piece.x==7){
                castlek=false;
            }
            if(piece.type == PieceType::BLACK_KING && target_x==piece.x-2){//q
                pieces[0][3] = pieces[0][0];
                pieces[0][0].color = Color::NONE;
                pieces[0][0].type = PieceType::NONE;
            }
            if(piece.type == PieceType::BLACK_ROOK && piece.x==0){
                castlek=false;
            }
            if(piece.type == PieceType::WHITE_KING && target_x==piece.x+2){//K
                pieces[7][5] = pieces[7][7];
                pieces[7][7].color = Color::NONE;
                pieces[7][7].type = PieceType::NONE;
            }
            if(piece.type == PieceType::WHITE_ROOK && piece.x==7){
                castleK=false;
            }
            if(piece.type == PieceType::WHITE_KING && target_x==piece.x-2){//Q
                pieces[7][3] = pieces[7][0];
                pieces[7][0].color = Color::NONE;
                pieces[7][0].type = PieceType::NONE;
            }
            if(piece.type == PieceType::WHITE_ROOK && piece.x==0){
                castleQ=false;
            }
            if(piece.type == PieceType::BLACK_KING){
                castlek=false;
                castleq=false;
                blackKingX = target_x;
                blackKingY = target_y;
            }
            if(piece.type == PieceType::WHITE_KING){
                castleK=false;
                castleQ=false;
                whiteKingX = target_x;
                whiteKingY = target_y;
            }
            pieces[target_y][target_x].color = piece.color;
            pieces[target_y][target_x].type = piece.type;
            pieces[piece.y][piece.x].color = Color::NONE;
            pieces[piece.y][piece.x].type = PieceType::NONE;
            Color tmp = toMove==Color::BLACK ? Color::WHITE : Color::BLACK;
            std::vector<std::tuple<int, int>> check = movesList(tmp);
            if(toMove==Color::BLACK && isMoveInList(check, blackKingY, blackKingX)){
                return false;
            }
            if(toMove==Color::WHITE && isMoveInList(check, whiteKingY, whiteKingX)){
                return false;
            }
            pieces[get<0>(lastPosition)][get<1>(lastPosition)].enPassant = false;
            lastPosition = std::make_tuple(target_y,target_x);
            position = reinterpret();
            toMove == Color::BLACK ? toMove=Color::WHITE : toMove=Color::BLACK;
            for(int i=0;i<8;i++){
                for(int j=0;j<8;j++){
                    std::cout << static_cast<int>(pieces[i][j].type) << " ";
                }
                cout<<endl;
            }
            cout<<"-----------------" << endl;
            return true;  
        }
        // NOTE: Maybe important???????
        // moves.clear();
        return false;
    }

    std::vector<std::tuple<int, int>> movesList(Color color){
        std::vector<std::tuple<int,int>> tmp;
        std::vector<std::tuple<int,int>> list;
        for(int i=0;i<8;i++){
            for(int j=0;j<8;j++){
                if (pieces[i][j].color==color){
                    tmp=generatePossibleMoves(pieces[i][j]);
                    list.insert(list.end(), tmp.begin(), tmp.end());
                }
            }
        }
        return list;
    }
    
    void drawBoard()
    {
        for (int i = 0; i < 8; ++i)
        {
            for (int j = 0; j < 8; ++j)
            {
                window.draw(board[i][j]);
            }
        }
    }

    void drawPieces()
    {
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                window.draw(pieces[i][j].piece);
            }
        }
    }

    sf::Vector2i getCoordinateIndex(int x, int y)
    {
        float fieldSize = window.getSize().x < window.getSize().y ? window.getSize().x / 8 : window.getSize().y / 8;
        int boardY = (window.getSize().y - fieldSize * 8) / 2;
        int boardX = (window.getSize().x - fieldSize * 8) / 2;

        int row = (y - boardY) / fieldSize;
        int col = (x - boardX) / fieldSize;

        return sf::Vector2(col, row);
    }

    void run()
    {
        interpret();
        sf::Event event;
        bool isMoving = false;
        Piece* movingPiece = nullptr;
        int pieceStartPositionX;
        int pieceStartPositionY;
        int pieceStartIndexX;
        int pieceStartIndexY;
        sf::RectangleShape* lastHighlightedField = nullptr;
        sf::Color lastHighlightedFieldColor;


        while (window.isOpen())
        {
            window.clear();
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect view(0, 0, event.size.width, event.size.height);
                    window.setView(sf::View(view));
                    initializeBoard();
                    interpret();
                }

                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }

                // Listen for mouse drag
                if (event.type == sf::Event::MouseButtonPressed && !isMoving)
                {
                    //initializeBoard();
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    // Get piece size
                    int fieldSize = window.getSize().x < window.getSize().y ? window.getSize().x / 8 : window.getSize().y / 8;

                    for (int i = 0; i < 8; i++)
                    {
                        for (int j = 0; j < 8; j++)
                        {
                            if (pieces[i][j].type != PieceType::NONE && pieces[i][j].piece.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                            {
                                isMoving = true;
                                movingPiece = &pieces[i][j];
                                pieceStartPositionX = movingPiece->piece.getPosition().x;
                                pieceStartPositionY = movingPiece->piece.getPosition().y;
                                pieceStartIndexX = i;
                                pieceStartIndexY = j;

                                std::vector<std::tuple<int, int>> moves = generatePossibleMoves(*movingPiece);

                                for (const auto& move : moves) {
                                    // Highlight the move field
                                    sf::RectangleShape& field = board[get<0>(move)][get<1>(move)];
                                    field.setFillColor(sf::Color(186,202,68));
                                }
                            }
                        }
                    }
                }


                if (isMoving){
                    // sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
                    // movingPiece->piece.setPosition(mousePosition.x - movingPiece->piece.getGlobalBounds().width / 2, mousePosition.y - movingPiece->piece.getGlobalBounds().height / 2);
                    
                    // sf::Vector2i vector = getCoordinateIndex(mousePosition.x, mousePosition.y);

                    // sf::RectangleShape* highlightedField = &board[vector.y][vector.x];

                    // highlightedField->setFillColor(sf::Color(211, 221, 135));
                    
                    // if (lastHighlightedField != nullptr) {
                    //     lastHighlightedField->setFillColor(lastHighlightedFieldColor);
                    // }

                    // lastHighlightedField = highlightedField;
                    // lastHighlightedFieldColor = highlightedField->getFillColor();
                }

                if (event.type == sf::Event::MouseButtonReleased)
                {
                    isMoving = false;
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                    // Get the target field given the current mouse position
                    int targetRow = (mousePosition.y - (window.getSize().y - window.getSize().x) / 2) / (window.getSize().x / 8);
                    int targetCol = (mousePosition.x - (window.getSize().x - window.getSize().y) / 2) / (window.getSize().y / 8);

                    isAllowed = false;
                    isAllowed = allowedMove(*movingPiece, targetCol, targetRow, generatePossibleMoves(*movingPiece));

                    // Check if the target field is valid
                    if (isAllowed)
                    {
                        // TODO: Check if the move is valid
                        // Move the piece to the target field
                        movingPiece->piece.setPosition(targetCol * window.getSize().x / 8, targetRow * window.getSize().y / 8);
                        //TODO: Empty does not work!
                        // if (pieces[targetRow][targetCol].type == PieceType::NONE && pieceStartIndexX != targetRow && pieceStartIndexY != targetCol) {
                            pieces[targetRow][targetCol] = *movingPiece;
                            pieces[targetRow][targetCol].piece.setPosition(targetCol * window.getSize().x / 8, targetRow * window.getSize().y / 8);
                            pieces[targetRow][targetCol].piece.setScale((float)window.getSize().x / 8 / pieceTextures[0].getSize().x, (float)window.getSize().y / 8 / pieceTextures[0].getSize().y);
                            movingPiece->type = PieceType::NONE;
                            movingPiece->piece.setPosition(pieceStartPositionX, pieceStartPositionY);
                            movingPiece->piece.setTextureRect(sf::IntRect());
                        // }
                    }
                    else
                    {
                        // Move the piece back to its original position
                        movingPiece->piece.setPosition(pieceStartPositionX, pieceStartPositionY);
                    }
                    initializeBoard();
                    movingPiece = nullptr;
                    interpret();    
                }
            }
            drawBoard();
            drawPieces();
            window.display();
        }
    }
};

int main()
{
    Game main;
    main.run();
    return 0;
}