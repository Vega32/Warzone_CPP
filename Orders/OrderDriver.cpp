#include "OrderDriver.h"
#include "../Player/Player.h"
#include "../GameEngine/GameEngine.h"
#include "../Map/Map.h"
#include "../Cards/Cards.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <random>
#include <vector>

void testOrderExecution() {

    // set up game environment
    GameEngine gameEngine;
    int* playerId_1 = new int(1);
    int* playerId_2 = new int(2);
    int* playerId_n = new int(3);

    Player* player1 = new Player("Player 1", playerId_1);
    player1->_reinforcementPool=new int(50);
    Player* player2 = new Player("Player 2", playerId_2);
    player2->_reinforcementPool=new int(50);
    Player* player_neutral = new Player("Player N", playerId_n); // neutral for negociate
    player_neutral->_reinforcementPool=new int(50);
    
    std::vector<Territory*> availableTerritories;
    for (auto& territory : gameMap.graph) {
        availableTerritories.push_back(&territory); // pointers looping
    }
    // shuffle the available territories to ensure randomness and NO OVERLAPS 
    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(availableTerritories.begin(), availableTerritories.end(), rng);

    int numTerritoriesToAssign = 3;
    if (availableTerritories.size() >= numTerritoriesToAssign * 3) {  // Ensure there are enough territories

        for (int i = 0; i < numTerritoriesToAssign; ++i) {
            player1->addTerritory(availableTerritories[i]);
            player2->addTerritory(availableTerritories[i + numTerritoriesToAssign]);
            player_neutral->addTerritory(availableTerritories[i + 2 * numTerritoriesToAssign]);
        }

    } else {
        // handle the case where there are not enough territories for assignment
        std::cerr << "Not enough territories available to assign to players!" << std::endl;
    }

    // Player commands for orders
    int playerID_1 = player1->getID();  // Store the result in a local variable
    int playerID_2 = player2->getID();  // Store the result in a local variable

    player1->issueOrder("deploy 10 china", &playerID_1);
    player1->issueOrder("advance 5 china vietnam", &playerID_1);
    player2->issueOrder("blockade vietnam",&playerID_2);
    player1->issueOrder("airlift 5 vietnam japan", &playerID_1);
    player1->issueOrder("bomb japan", &playerID_1);
    player1->issueOrder("negociate 1", &playerID_1);

    // order execution phase in the GameEngine
    std::string input;
    gameEngine.transition(new IssueOrders());  // IssueOrders state -> should be changed to transition

    // Execute orders
    std::cout << "Executing Player1's orders:" << std::endl;
    player1->getOrdersList()->setCurrentOrder(player1->getOrdersList()->getHead()->getNext());
    do{
        player1->getOrdersList()->getCurrentOrder()->execute();
        player1->getOrdersList()->setCurrentOrder(player1->getOrdersList()->getCurrentOrder()->getNext());
    }while(player1->hasMoreOrders());
    
    
    std::cout << "Executing Player2's orders:" << std::endl;
    player2->getOrdersList()->setCurrentOrder(player2->getOrdersList()->getHead()->getNext());
    do{
        player1->getOrdersList()->getCurrentOrder()->execute();
        player1->getOrdersList()->setCurrentOrder(player1->getOrdersList()->getCurrentOrder()->getNext());
    }while(player2->hasMoreOrders());

    // ownership transfers, card awarding, and negotiation effect
    std::cout << "Ownership and card verification:" << std::endl;

    // Player1's territory conquest and card awarding
    for (auto& territory : player1->getTerritories()) {
        if (*territory->owner == player1->getID()) {
            std::cout << "Player1 conquered " << territory->name << ". Ownership transferred." << std::endl;
            player1->getHand()->addCard(Cards()); // Award card for territory conquest
        }
    }

    // blockade orders (neutral player ownership)
    for (auto& territory : player_neutral->getTerritories()) {
        if (*territory->owner == player_neutral->getID()) {
            std::cout << "Blockade successful. Ownership of " << territory->name << " transferred to neutral player." << std::endl;
        }
    }
    
    
}
