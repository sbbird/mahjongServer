#ifndef _PLAYER_H_
#define _PLAYER_H_

#include <vector>
#include <iostream>
#include <string.h>
#include <algorithm>
#include <sstream>
#include <string>
using namespace std;

enum {
	TEHAI_NUMBER=13
};


// type of mahjong tile
typedef unsigned Hai;

enum Position {
	EAST = 0,
	SOUTH = 1,
	WEST = 2,
	NORTH = 3,
};

using namespace std;	
class Player{
private:
	class User *p;        // connecting 
	string Name;
	int Point;
        vector<Hai> Tehai;    // tiles in hand
	    
	bool Riichi;          // Ready Hand declaration
	int Pos;              // EAST SOUTH WEST NORTH
	char* Kawa;           // river of discarded tiles
	Hai TsumoHai;         // the drew tile in this round
	
	
public:
	Player();
	~Player();
	void setPoint(int p);
	void setPos(int p);
	void haipai(vector<Hai>::iterator );
	void tsumo(Hai h);
	void pon(Hai h);
	void chi(Hai h);
	void kan(Hai h);
	bool canAgari();      // win?
	bool canKan();        
	bool canRiichi();    
	
};

#endif
