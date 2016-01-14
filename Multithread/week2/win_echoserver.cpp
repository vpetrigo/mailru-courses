/*
 *  Author: vpetrigo
 *  Task: implement simple echo TCP-server
 */

#include <iostream>
#include <stdexcept>
#include <memory>
#include <array>
#include <winsock2.h>
#include <ws2tcpip.h>

constexpr int sock_min_version_req = 2;
constexpr int sock_maj_version_req = 2;

std::unique_ptr<addrinfo> init_address() {
    addrinfo *result = nullptr;
    addrinfo hints;
    const char *default_port = "12345";
    
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    
    if (getaddrinfo(NULL, default_port, &hints, &result)) {
        WSACleanup();
        throw std::runtime_error{"getaddrinfo failed"};
    }
    
    return std::unique_ptr<addrinfo>{result};
}

int main() {
    WSADATA data;
    
    if (WSAStartup(MAKEWORD(sock_min_version_req, sock_maj_version_req), &data)) {
        throw std::runtime_error{"Cannot initialize socket"};
    }
    
    if (LOBYTE(data.wVersion) >= sock_min_version_req && HIBYTE(data.wVersion) >= sock_maj_version_req) {
        std::cout << "All is fine" << std::endl;
    }
    else {
        WSACleanup();
        throw std::runtime_error{"Incorrect version of winsock"};
    }
    
    auto result = init_address();
    SOCKET main_socket = socket(result.get()->ai_family, result.get()->ai_socktype, result.get()->ai_protocol);
    
    if (main_socket == INVALID_SOCKET) {
        freeaddrinfo(result.release());
        WSACleanup();
        throw std::runtime_error{"Can not init socket"};
    }
    
    if (bind(main_socket, result.get()->ai_addr, static_cast<int> (result.get()->ai_addrlen))) {
        freeaddrinfo(result.release());
        closesocket(main_socket);
        WSACleanup();
        throw std::runtime_error{"Can not bind socket"};
    }
    freeaddrinfo(result.release());
    
    if (listen(main_socket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(main_socket);
        WSACleanup();
        throw std::runtime_error{"Listening error"};
    }
    
    SOCKET client_socket = accept(main_socket, nullptr, nullptr);
    
    if (client_socket == INVALID_SOCKET) {
        closesocket(client_socket);
        WSACleanup();
        throw std::runtime_error{"Cannot accept incoming connection"};
    }
    
    std::array<char, 512> recv_buffer;
    int bytes_received = 0;
    
    while ((bytes_received = recv(client_socket, recv_buffer.begin(), recv_buffer.size(), 0))) {
        for (size_t i = 0; i < bytes_received; ++i) {
            std::cout << recv_buffer[i];
        }
        
        send(client_socket, recv_buffer.begin(), bytes_received, 0);
    }
    
    shutdown(client_socket, SD_BOTH);
    closesocket(client_socket);
    closesocket(main_socket);
    WSACleanup();
 
    return 0;
}