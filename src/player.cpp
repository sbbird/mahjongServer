using namespace std;
#include "user.h"
#include "message.h"
#include "player.h"


Player::Player(){
//#ifdef DEBUG
//	LOGSTREAM << "Server::Server\n";
//#endif
	// do nothing
	
	Tehai.reserve(TEHAI_NUMBER);
}

Player::~Player(){

}

void Player::setPoint(int p){
	Point = p;
}
void Player::setPos(int p){
	Pos = p;
}
void Player::haipai(vector<Hai>::iterator it){
	//std::copy(i, i+13, Tehai);
	int i;
	for(i = 0; i< 13; i++){
		Tehai.push_back(*(it+i));
	}
	std::sort(Tehai.begin(), Tehai.end());
	


	// generate xml message
	// TODO 
	stringstream ss;
	ss <<"<game action=\"haipai\" value=\"";
	vector<Hai>::iterator itehai;
	for(itehai = Tehai.begin(); itehai < Tehai.end(); itehai++){
		ss << std::hex << (*itehai);
	}
	ss <<"\" ></game>\n";
	
	p->sendMessage(new Message(new string(ss.str())));
}
int checkTsumoState(){
	// check if can Agari or Kan or Riichi
	return 0;
}

void Player::tsumo(Hai h){
	TsumoHai = h;

	// generate xml message
	
	stringstream ss;
	int state = checkTsumoState();
	
	ss <<"<game action=\"tsumo\" value=\"";	
	ss << std::hex << h;	
	ss <<"\" state=0  ></game:haipai>\n";      
	
}


//void Player::agari()


void Player::pon(Hai h){

}
