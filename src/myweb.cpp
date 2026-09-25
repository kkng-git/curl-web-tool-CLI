#include <iostream>
#include <typeinfo>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>

using namespace std;

const int BUF_SIZE = 2048;

void freeAllMem(char* hN, char* ipAddr, char* p, char* dP){
    free(hN);
    free(ipAddr);
    free(dP);
    if(p != nullptr){
        free(p);
    }
}

int main(int argc, char* argv[]){
    
    // Get Command Line Args
    char* hostName = nullptr;
    char* ipAddress = nullptr;
    char* port = nullptr;
    char* docPath = nullptr;
    bool headersOnly = false;

    if(argc < 3 || argc > 4){
        cerr << "Invalid number of command line arguments" << endl;
        return -1;
    }

    // Sort IP Address, Port Number, Hostname
    // Check for header option
    if(argc == 4){
        // cout << argv[3] << endl;
        string argv3 = argv[3];
        if(argv3 == "-h"){
            // cout << "Header option provided" << endl;
            headersOnly = true;
        }
        else{
            cerr << "Invalid command line option: " << argv3 << endl;
            return -1;
        }
    }

    int hostNameLength = strlen(argv[1]);
    hostName = (char*) malloc(sizeof(char) * (hostNameLength+1));
    strncpy(hostName, argv[1], hostNameLength);
    hostName[hostNameLength] = '\0';

    // cout << hostName << endl;

    // Isolate IP Address, Port Number, and Doc Path
    char* docPathTemp = strchr(argv[2], '/');
    // cout << docPathTemp << endl;
    int docPathIndex;
    if(docPathTemp == nullptr){
        cerr << "Unable to determine Document Path" << endl;
        free(hostName);
        return -1;
    }
    else{
        docPathIndex = docPathTemp - argv[2];
        int size = strlen(docPathTemp);
        // cout << size << endl;
        docPath = (char*) malloc(sizeof(char) * (size+1));
        strncpy(docPath, argv[2] + docPathIndex, size);
        docPath[size] = '\0';
        // cout << docPath << endl;
        // cout << docPathIndex << endl;
    }

    char* portTemp = strchr(argv[2], ':');
    int portIndex;
    // if port is missing, then get IP Address using docPath
    if(portTemp == nullptr){
        ipAddress = (char*) malloc(sizeof(char) * (docPathIndex+1));
        strncpy(ipAddress, argv[2], docPathIndex);
        ipAddress[docPathIndex] = '\0';
        // cout << ipAddress << endl;
    }
    else{
        // port is available, splice as necessary
        // cout << portTemp << endl;
        portIndex = portTemp - argv[2];
        // cout << portIndex << endl;
        ipAddress = (char*) malloc(sizeof(char) * (portIndex+1));
        strncpy(ipAddress, argv[2], portIndex);
        ipAddress[portIndex] = '\0';
        // cout << ipAddress << endl;
        
        int length = docPathIndex - portIndex - 1;
        //cout << length << endl;
        port = (char*) malloc(sizeof(char) * (length + 1));
        strncpy(port, argv[2] + portIndex+1, length);
        port[length] = '\0';
        // cout << port << endl;
    }

    // Open TCP Connection using IP Address & Port Number

    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        cerr << "Error in creating socket" << endl;
        freeAllMem(hostName, ipAddress, port, docPath);
        return -1;
    }

    struct sockaddr_in serverAddress;

    serverAddress.sin_family = AF_INET;
    if(port != nullptr){
        int portNumber = strtol(port, nullptr, 0);
        serverAddress.sin_port = htons(portNumber);
    }
    else{
        // Default port ?
        serverAddress.sin_port = htons(80);
    }
    if(inet_pton(AF_INET, ipAddress, &serverAddress.sin_addr) <= 0) {
        cerr << "Invalid address / Address not supported" << endl;
        freeAllMem(hostName, ipAddress, port, docPath);
        return -1;
    }
    if(connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Connection failed" << endl;
        freeAllMem(hostName, ipAddress, port, docPath);
        return -1;
    }

    // Construct Request
    string hostString = hostName;
    string pathString = docPath;
    string request;
    if(headersOnly){
        request = "HEAD " + pathString + " HTTP/1.1\r\n";
        request += "Host: " + hostString + "\r\n\r\n";
    }
    else{
        request = "GET " + pathString + " HTTP/1.1\r\n";
        request += "Host: " + hostString + "\r\n\r\n";
    }
    // cout << request << endl;

    // Send dat shit out
    if (send(sock, request.c_str(), request.size(), 0) < 0) {
        std::cerr << "Error sending request" << std::endl;
        freeAllMem(hostName, ipAddress, port, docPath);
        return -1;
    }

    // Handle response
    char response[BUF_SIZE];
    int bytesRead = recv(sock, response, BUF_SIZE, 0);
    ofstream outfile;
    if(!headersOnly){
        outfile.open("output.dat");
    }
    while(bytesRead > 0){
        if(headersOnly){
            cout << response << endl;
        }
        else{
            // Filter out headers
            char* headerEnd = strstr(response, "\r\n\r\n");
            if(headerEnd != nullptr){
                // Offset to get body only
                outfile << (headerEnd + 4);
            }
            else{
                outfile << response;
            }
        }
        if(response[bytesRead] == '\0'){
            break;
        }
        bytesRead = recv(sock, response, BUF_SIZE, 0);
    }
    // free
    outfile.close();
    close(sock);
    freeAllMem(hostName, ipAddress, port, docPath);

    return 0;
}
