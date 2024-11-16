
#include "Player.h"
#include <algorithm>
#include <iostream>
#include <sstream> //spliting the command line
#include <utility>
#include "../Orders/Orders.h"
#include "..\PlayerStrategies/PlayerStrategies.h"


//Player::Player(){}; 
Player::Player() : _orderList(new OrdersList()) {} // Added - K - A2

Player::Player(const string &name, int* id){
    this->_name = name;
    this->_id=new int(*id);
    this->_orderList=new OrdersList();
}

//copy constructor
Player::Player(const Player& other)
{
    this->_name = other._name;
    this->_territories = other._territories;
    this->_handCard = other._handCard;
    this->_playerterritories = other._playerterritories;
    _orderList = new OrdersList(*other._orderList);
}

//destructor
Player::~Player()
{
    this->_name.clear();
    this->_territories.clear();
    delete _handCard;
    delete _orderList; // changed - K - A2
}

// Assignment operator
Player& Player::operator=(const Player& other) {
    if (this == &other) return *this; // Self-assignment check

    delete _id;
    delete _handCard;
    delete _orderList;

    _name = other._name;
    _territories = other._territories;
    _handCard = other._handCard;
    _playerterritories = other._playerterritories;

    _orderList = new OrdersList(*other._orderList); // Deep copy

    return *this;
}

void Player::setPlayerStrategy(const string& strategy) {
        if (strategy=="Human"){
            delete playerStrategy;
            playerStrategy = new HumanPlayerStrategy(this);
        }

        else if (strategy=="Aggressive"){
            delete playerStrategy;
            playerStrategy = new AggressivePlayerStrategy(this);
        }

        else if (strategy =="Benevolent"){
            delete playerStrategy;
            playerStrategy = new BenevolentPlayerStrategy(this);
        }

        else if (strategy=="Neutral"){
            delete playerStrategy;
            playerStrategy = new NeutralPlayerStrategy(this);
        }

        else if (strategy=="Cheater"){
            delete playerStrategy;
            playerStrategy = new CheaterPlayerStrategy(this);
        }
}


void Player::printOrder() const { // changed - K - A2
    if (_orderList->getSize() == 0) {
        std::cout << "Order list is empty." << std::endl;
        return;
    }

    std::cout << "Here is your list of current orders, Commander:" << std::endl;
    Order* currentOrder = _orderList->getHead()->getNext(); // Start from the first actual order
    int index = 1;

    while (currentOrder != _orderList->getTail()) {
        std::cout << "(" << index++ << ") " << currentOrder->getOrderName() << std::endl;
        currentOrder = currentOrder->getNext();
    }
}

// check if the player has more orders to issue
bool Player::hasMoreOrders() const {
    if(_orderList->getCurrentOrder()->getNext()==nullptr){
        return true;
    }  // Returns true if there are orders in the list
    return false;
}

// Return the OrdersList
OrdersList* Player::getOrdersList() { // changed - K - A2
    return _orderList;
}

// Print the hand of cards for Player
void Player::printHand() const {
    std::cout << _name << "'s ";
    std::cout << *_handCard;
}

void Player::setHand(Hand* hand) {
    if (_handCard==nullptr) {
        delete _handCard;  // Clean up existing hand 
    }
    _handCard = hand;      // Assign new hand
}

Hand* Player::getHand(){
    return this->_handCard;
}

int Player::getID(){
    return *this->_id;
}


// ------------------------------------------------
//            Find Territory by name
// ------------------------------------------------
Territory* Player::findTerritoryByName(const std::string& territoryName) {  //allTerritoriesInMap is a vector that contains all the territories
    for (Territory &territory : gameMap.graph) {
        if (*(territory.name) == territoryName) {
            return &territory;  // Territory found
        }
    }
    return nullptr;  // Territory not found
} // when a player enters a territory name, this function should be called to check if the territory exists


// -------------------------------------------------
//   If player choice of card exists in their hand
// -------------------------------------------------
bool Player::hasCardType(const std::string& cardType) {
    Hand* hand = getHand();
    if (!hand) {
        std::cout << "Player has no hand." << std::endl;
        return false;
    }

    const std::vector<Cards>& cardsInHand = hand->getCards();
    for (const auto& card : cardsInHand) {
        if (card.getType() == cardType) {
            return true;
        }
    }
    return false;
}


// --------------------------------------------------
//                 Issue Order
// --------------------------------------------------

void Player::issueOrder(){
    //--------------PLAYER STRATEGY----------------------
    playerStrategy->issueOrder();
    //---------------------------------------------------

}


//Defend changed - K - A2
// --------------------------------------------------
//              Territories to attack
// --------------------------------------------------
std::vector<Territory*> Player::toAttack() const {
    //--------------PLAYER STRATEGY----------------------
    //playerStrategy->toAttack();
    //---------------------------------------------------

    std::vector<Territory*> territoriesToAttack;

    // Loop through all territories in the game map
    for (int i=0; i<gameMap.graph.size();i++) {
        // Check if the territory is not owned by the player
        if (*gameMap.graph.at(i).owner!=*_id) {
            // Territory is available for attack, push back the pointer to the territory
            territoriesToAttack.push_back(&gameMap.graph.at(i));
        }
    }

    return territoriesToAttack;
}


//Defend changed - K - A2
// --------------------------------------------------
//              Territories to defend
// --------------------------------------------------
std::vector<Territory*> Player::toDefend() const {
    //--------------PLAYER STRATEGY----------------------
    //playerStrategy->toDefend();
    //---------------------------------------------------
    std::vector<Territory*> territoriesToDefend;

    // Add all player-owned territories to the vector
    for (int i=0; i<gameMap.graph.size();i++) { // Each time the player concquer a territory, it should be added in _playerTerritory
        if (*gameMap.graph.at(i).owner==*_id) {
            territoriesToDefend.push_back(&gameMap.graph.at(i));
        }
    }

    return territoriesToDefend;
}


// --------------------------------------------------
//           Territories to add from map
// --------------------------------------------------
void Player::addTerritory(Territory* territory) {
    *territory->owner=*this->_id;
    _playerterritories.push_back(territory);
}

std::vector<Territory*> Player::getTerritories() const { // new - A2 - K
    return _playerterritories;
}

// Operator to display player information (optional)
ostream& operator<<(ostream &out, const Player &player) {
    out << "Player Name: " << player.getName();
    return out;
}

// Implementing the getName() method with the const qualifier
std::string Player::getName() const {
    return _name;
}

std::vector<Player*> playerList;