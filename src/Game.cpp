using namespace std;


#include "player.h"
#include "user.h"

#include "Game.h"


Game::Game(){
	
#ifdef DEBUG
//	LOGSTREAM << "Server::Server\n";
#endif
	

  
}

Game::~Game(){
	
#ifdef DEBUG
//	LOGSTREAM << "Server::Server\n";
#endif
//	aPlayers = new std::vector<class User*>();
//	aViewers = new std::vector<class User*>(); 
	Yama.reserve(HAI_NUMBER);
	
}

void Game::start(){
	// start game;




}

int Game::addPlayer(User* p){


}

void Game::initGame(){
	
	int i;
	std::random_shuffle( aPlayers.begin(), aPlayers.end());
	vector<Player*>::iterator iPlayer = aPlayers.begin();
	for( i = 0; iPlayer < aPlayers.end(); iPlayer++){
		(*iPlayer)->setPoint(25000);
		(*iPlayer)->setPos(i);
		i++;
	}      
}

void Game::newRound(){
	initYama();
	initDora();
	iCurrentPlayer = aPlayers.begin();
	
	
	
}

//void Game::goStep(){
	
	
	
	
// start from east;
//}

void Game::dealHai(){
	(*iCurrentPlayer)->tsumo(*iYama++);

       
}




void Game::initDora(){
	Hai dora_show = Yama[DORA_SHOW_INDEX];
	
	Dora.push_back((dora_show & 0xF0) |
		       (( dora_show & 0x0F + 1)%9));
	
	
	
}

void Game::haipai(){
	/*
	int i, j, k;
	for( i = 0; i<3; i++){
		for( j = 0; j<4; j++)
			// take four hai each time;
			for( k = 0; k < 4; k++){
				aPlayers[j].haipai(*iYama);
				iYama++;	
			}
	}

	for( j = 0; j<4; j++){
		aPlayers[j].haipai(*iYama);
		iYama++;
	}
	*/
	int i;
	vector<Player*>::iterator iPlayer;
	for( iPlayer = aPlayers.begin(); iPlayer< aPlayers.end(); iPlayer++){
		(*iPlayer)->haipai(iYama);
		iYama += 13;
	}
}


int myrandom( int i){ return std::rand()%i;}

void Game::initYama(){


	std::copy(HaiData, HaiData + 136, Yama.begin());
	
	random_shuffle(Yama.begin(),Yama.end(), myrandom);

	iYama = Yama.begin();
	
}

void Game::handleAction(string userid , string action, string value){
	
}
	

