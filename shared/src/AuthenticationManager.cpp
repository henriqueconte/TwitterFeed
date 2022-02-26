#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "../headers/AuthenticationManager.hpp"


bool AuthenticationManager::login(std::string username) {
    // TODO: verify if there is already two sessions from the same user
    std::cout << "Autenticação do usuário " << username << " realizada com sucesso\n";
    return true;
}