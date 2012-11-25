#include <iostream>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <dirent.h>
#include <map>
#include "udt.h"

using namespace std; 

struct addrinfo addr_hint;

map<string, UDTSOCKET> sub_sock_map;

UDTSOCKET getSocket(const string& name) {
//    cerr << "get consumer of "<<name<<endl;

    DIR *dp;
    struct dirent *ent_ptr;
    dp = opendir(("/var/lib/wissbi/sub/"+name).c_str());
    while((ent_ptr = readdir(dp)) != NULL){
        string addr(ent_ptr->d_name);
        if(addr.compare(".") == 0 || addr.compare("..") == 0) continue;
        
        if(sub_sock_map.find(addr) == sub_sock_map.end()){
            size_t sep_idx = addr.rfind(":");
            istringstream iss(addr.substr(sep_idx + 1, addr.length()));
            int port;
            iss >> port;
            cerr << "connect to " << addr.substr(0, sep_idx) << ":" << port << endl;
            
            sockaddr_in saddr;
			memset(&saddr, 0, sizeof(saddr));
            cerr << "pton res: " << inet_pton(AF_INET, addr.substr(0, sep_idx).c_str(), &saddr.sin_addr) << endl;
            saddr.sin_port = htons(port);

            UDTSOCKET sock = UDT::socket(AF_INET, SOCK_DGRAM, 0);
            if(UDT::ERROR == UDT::connect(sock, (sockaddr*)&saddr, sizeof(saddr))) {
                cerr << "connect error: " << UDT::getlasterror().getErrorMessage() << endl;
                cerr << inet_ntoa(saddr.sin_addr) <<":"<<ntohs(saddr.sin_port)<< endl;
            }
            sub_sock_map[addr] = sock;
            return sock;
        }
        return sub_sock_map[addr];
    }
    closedir(dp);
}

int main(int argc, char* argv[]) {
	UDT::startup();
	memset(&addr_hint, 0, sizeof(struct addrinfo));
	addr_hint.ai_family = AF_INET6;
	addr_hint.ai_socktype = SOCK_DGRAM;
	
	string line;
	UDTSOCKET sock = getSocket(argv[1]);
	unsigned int cnt = 0;
	while(getline(cin, line)) {
	/*	if(++cnt % 100 == 0){
			sock = getSocket(argv[1]);
		}*/
		//cerr << "sock: " << sock << " line: " << line << endl;
		//int res = UDT::sendmsg(sock, line.c_str(), line.size(), -1, false);
		int res = UDT::sendmsg(sock, line.c_str(), line.size(), 100);
        cerr << "send res: " << res <<endl;
	}
    UDT::close(sock);

	return 0;
}
