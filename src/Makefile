logos: main.o board.o bitboard.o move.o movegen.o
	clang++ -std=c++20 ../out/main.o ../out/board.o ../out/bitboard.o ../out/move.o ../out/movegen.o -o ../out/logos

main.o:
	clang++ -std=c++20 -c main.cpp -o ../out/main.o

board.o:
	clang++ -std=c++20 -c board.cpp -o ../out/board.o

bitboard.o:
	clang++ -std=c++20 -c bitboard.cpp -o ../out/bitboard.o

move.o:
	clang++ -std=c++20 -c move.cpp -o ../out/move.o

movegen.o:
	clang++ -std=c++20 -c movegen.cpp -o ../out/movegen.o
