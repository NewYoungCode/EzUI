#include "os.h"
#include "WebClient.h"
int main(int count, char* args[]) {

	std::string resp;
	WebClient wc;
	
	wc.HttpGet("www.google.com",resp);

	return 0;

}