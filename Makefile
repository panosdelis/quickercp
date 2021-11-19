all:quic

quic: quic.o basicFunctions.o recursiveCopyDelete.o outputFunctions.o
	gcc -g quic.c basicFunctions.c recursiveCopyDelete.c outputFunctions.c -o quic

clean:
	rm -f quic quic.o basicFunctions.o recursiveCopyDelete.o outputFunctions.o