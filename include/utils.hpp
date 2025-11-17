#include <string>
#include <vector>  

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

double run_command(const std::string& cmd);
void print_header();
void print_results(const std::string& cmd, const std::vector<double>& times);
