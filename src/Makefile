logosMain: main.o board.o bitboard.o move.o movegen.o attacks.o evaluation.o search.o play.o testMovegen.o magic.o logos
	clang++ -std=c++20 -g ../out/main.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o ../out/attacks.o ../out/evaluation.o ../out/search.o ../out/play.o ../out/testMovegen.o ../out/magic.o -o ../out/logosMain -Wall -Wextra -pedantic

logos: uci.o board.o bitboard.o move.o movegen.o attacks.o evaluation.o search.o play.o testMovegen.o magic.o
	clang++ -std=c++20 -g ../out/uci.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o ../out/attacks.o ../out/evaluation.o ../out/search.o ../out/play.o ../out/testMovegen.o ../out/magic.o -o ../out/logos -Wall -Wextra -pedantic

uci.o:
	clang++ -std=c++20 -c uci.cpp -o ../out/uci.o -Wall -Wextra -pedantic

main.o:
	clang++ -std=c++20 -c main.cpp -o ../out/main.o -Wall -Wextra -pedantic

board.o:
	clang++ -std=c++20 -c board.cpp -o ../out/board.o -Wall -Wextra -pedantic

bitboard.o:
	clang++ -std=c++20 -c bitboard.cpp -o ../out/bitboard.o -Wall -Wextra -pedantic

move.o:
	clang++ -std=c++20 -c move.cpp -o ../out/move.o -Wall -Wextra -pedantic

movegen.o:
	clang++ -std=c++20 -g -c movegen.cpp -o ../out/movegen.o -Wall -Wextra -pedantic

attacks.o:
	clang++ -std=c++20 -c attacks.cpp -o ../out/attacks.o -Wall -Wextra -pedantic

evaluation.o:
	clang++ -std=c++20 -g -c evaluation.cpp -o ../out/evaluation.o -Wall -Wextra -pedantic

search.o:
	clang++ -std=c++20 -g -c search.cpp -o ../out/search.o -Wall -Wextra -pedantic

play.o:
	clang++ -std=c++20 -g -c play.cpp -o ../out/play.o -Wall -Wextra -pedantic

testMovegen.o:
	clang++ -std=c++20 -g -c testMovegen.cpp -o ../out/testMovegen.o -Wall -Wextra -pedantic

magic.o:
	clang++ -std=c++20 -g -c magic.cpp -o ../out/magic.o -Wall -Wextra -pedantic
