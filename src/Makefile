logosMain: main.o board.o bitboard.o move.o movegen.o attacks.o evaluation.o search.o play.o testMovegen.o magic.o logos
	clang++ -std=c++20 -g ../out/main.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o ../out/attacks.o ../out/evaluation.o ../out/search.o ../out/play.o ../out/testMovegen.o ../out/magic.o -o ../out/logosMain -Wall -Wextra

logos: uci.o board.o bitboard.o move.o movegen.o attacks.o evaluation.o search.o play.o testMovegen.o magic.o
	clang++ -std=c++20 -g ../out/uci.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o ../out/attacks.o ../out/evaluation.o ../out/search.o ../out/play.o ../out/testMovegen.o ../out/magic.o -o ../out/logos -Wall -Wextra

uci.o:
	clang++ -std=c++20 -c uci.cpp -o ../out/uci.o -Wall -Wextra

main.o:
	clang++ -std=c++20 -c main.cpp -o ../out/main.o -Wall -Wextra

board.o:
	clang++ -std=c++20 -c board.cpp -o ../out/board.o -Wall -Wextra

bitboard.o:
	clang++ -std=c++20 -c bitboard.cpp -o ../out/bitboard.o -Wall -Wextra

move.o:
	clang++ -std=c++20 -c move.cpp -o ../out/move.o -Wall -Wextra

movegen.o:
	clang++ -std=c++20 -g -c movegen.cpp -o ../out/movegen.o -Wall -Wextra

attacks.o:
	clang++ -std=c++20 -c attacks.cpp -o ../out/attacks.o -Wall -Wextra

evaluation.o:
	clang++ -std=c++20 -g -c evaluation.cpp -o ../out/evaluation.o -Wall -Wextra

search.o:
	clang++ -std=c++20 -g -c search.cpp -o ../out/search.o -Wall -Wextra

play.o:
	clang++ -std=c++20 -g -c play.cpp -o ../out/play.o -Wall -Wextra

testMovegen.o:
	clang++ -std=c++20 -g -c testMovegen.cpp -o ../out/testMovegen.o -Wall -Wextra

magic.o:
	clang++ -std=c++20 -g -c magic.cpp -o ../out/magic.o -Wall -Wextra
