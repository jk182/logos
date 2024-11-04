# logos: main.o board.o bitboard.o move.o movegen.o attacks.o evaluation.o search.o play.o testMovegen.o magic.o
	# clang++ -std=c++20 -g ../out/main.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o ../out/attacks.o ../out/evaluation.o ../out/search.o ../out/play.o ../out/testMovegen.o ../out/magic.o -o ../out/logos

logos: uci.o board.o bitboard.o move.o movegen.o attacks.o evaluation.o search.o play.o testMovegen.o magic.o
	clang++ -std=c++20 -g ../out/uci.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o ../out/attacks.o ../out/evaluation.o ../out/search.o ../out/play.o ../out/testMovegen.o ../out/magic.o -o ../out/logos

uci.o:
	clang++ -std=c++20 -c uci.cpp -o ../out/uci.o

main.o:
	clang++ -std=c++20 -c main.cpp -o ../out/main.o

board.o:
	clang++ -std=c++20 -c board.cpp -o ../out/board.o

bitboard.o:
	clang++ -std=c++20 -c bitboard.cpp -o ../out/bitboard.o

move.o:
	clang++ -std=c++20 -c move.cpp -o ../out/move.o

movegen.o:
	clang++ -std=c++20 -g -c movegen.cpp -o ../out/movegen.o

attacks.o:
	clang++ -std=c++20 -c attacks.cpp -o ../out/attacks.o

evaluation.o:
	clang++ -std=c++20 -g -c evaluation.cpp -o ../out/evaluation.o

search.o:
	clang++ -std=c++20 -g -c search.cpp -o ../out/search.o

play.o:
	clang++ -std=c++20 -g -c play.cpp -o ../out/play.o

testMovegen.o:
	clang++ -std=c++20 -g -c testMovegen.cpp -o ../out/testMovegen.o

magic.o:
	clang++ -std=c++20 -g -c magic.cpp -o ../out/magic.o
