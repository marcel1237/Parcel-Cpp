#include <iostream>
#include <unistd.h>

void printHelp() {
    std::cout << "Usage: tool [options]\n"
              << "Options:\n"
              << "  -h      Show help\n"
              << "  -v      Verbose mode\n"
              << "  -n <val> Set number\n";
}

int main(int argc, char* argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "hvn:")) != -1) {
        switch (opt) {
            case 'h': printHelp(); return 0;
            case 'v': std::cout << "Verbose ON\n"; break;
            case 'n': std::cout << "Number: " << optarg << "\n"; break;
            default: printHelp(); return 1;
        }
    }
    return 0;
}
