//
// Created by minha on 11/5/2024.
//

#include "CommandProcessor.h"

#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include "../GameEngine/GameEngine.h"

using std::cin, std::string, std::ifstream;


Command::Command(const string &command) {
    addObserver(loggingObserver);
    this->command = command;
}

Command::Command(Command& command) {
    addObserver(loggingObserver);
    this->command = command.command;

}




void Command::saveEffect(const string& effect) {
    this->effect = effect;
    Notify(*this);
}



string Command::stringToLog() {
    return "Command's Effect: " + this->effect;
}

string Command::getCommand() {
    return this->command;
}

string Command::getEffect() {
    return this->effect;
}

CommandProcessor::CommandProcessor() {
    addObserver(loggingObserver);

}
CommandProcessor::CommandProcessor(CommandProcessor* commandProcessor) {
    this->commands = vector<Command*>(commandProcessor->commands);
    addObserver(loggingObserver);
}


vector<string> CommandProcessor::splitCommand(const string command){
    std::stringstream ss(command);
    vector<string> args;
    string word;
    while(std::getline(ss, word, ' ')){
        if(!word.empty()){
            args.push_back(word);
        }
    }
    return args;
}

string CommandProcessor::getCommand() {
    string command = readCommand();
    saveCommand(command);
    return command;

}

void CommandProcessor::validate(const void *ptr) {
    if (ptr != nullptr) {
        currentCommand->saveEffect("Entering " + currentCommand->getCommand() + " State");
    }
    else {
        currentCommand->saveEffect("Incorrect command");
    }
}

bool CommandProcessor::validate(const string command){
    vector<string> args = splitCommand(command);
    if(args.at(0)=="loadmap"&&(gameEngine.getCurrentState()=="Start"||gameEngine.getCurrentState()=="Map Loaded")&&args.size()>=2){
        currentCommand->saveEffect("Loading Map");
        return true;
    }else if(args.at(0)=="validatemap"&&gameEngine.getCurrentState()=="Map Loaded"){
        currentCommand->saveEffect("Validating Map");
        return true;
    }else if(args.at(0)=="addplayer"&&(gameEngine.getCurrentState()=="Map Validated"||gameEngine.getCurrentState()=="Players Added")&&args.size()>=2){
        currentCommand->saveEffect("Adding Player "+args.at(1));
        return true;
    }else if(args.at(0)=="gamestart"&&gameEngine.getCurrentState()=="Players Added"){
        currentCommand->saveEffect("Starting Game");
        return true;
    }else if(args.at(0)=="replay"&&gameEngine.getCurrentState()=="Win"){
        currentCommand->saveEffect("Replaying Game");
        return true;
    }else if(args.at(0)=="quit"&&gameEngine.getCurrentState()=="Win"){
        currentCommand->saveEffect("Quitting Game");
        return true;
    }else{
        currentCommand->saveEffect("Cannot use command \""+args.at(0)+"\" in state \""+gameEngine.getCurrentState()+"\"");
        return false;
    }
}


string CommandProcessor::readCommand() {
    std::string command;
    std::getline(cin, command);
    return command;
}

void CommandProcessor::saveCommand(const string& command) {
    currentCommand = new Command(command);
    commands.push_back(currentCommand);
    Notify(*this);
}

string CommandProcessor::stringToLog() {
    return "Command: " + currentCommand->getCommand();
}


FileCommandProcessorAdapter::FileCommandProcessorAdapter() = default;

FileCommandProcessorAdapter::FileCommandProcessorAdapter(const string& fileName):file(fileName) {

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
    }
}

string FileCommandProcessorAdapter::readCommand() {

    string line;
    if (getline(file, line)) {
        return line;
    } else {
        return "";
    }
}

CommandProcessor* commandProcessor = nullptr;