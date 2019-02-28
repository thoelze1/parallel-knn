#include <fstream>

int main(int argc, char **argv) {
	std::ofstream out;
	out.open(argv[1]);
	out<< "Test" << std::endl;
	out.close();
	return 0;
}
