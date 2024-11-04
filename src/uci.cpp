#include <iostream>


int main() {
	std::string command;
	while (true) {
		std::cin >> command;
		if (command == "quit") {
			break;
		}
		switch (command) {
			case "uci":
				std::cout << "id name Logos\n";
				std::cout << "id author Julian Kern\n";

				std::cout << "uciok\n";
				break;
			case "isready":
				std::cout << "readyok\n";
				break;

		}
	}
	return 0;
}
