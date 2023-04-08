OBJECTS= ./compiler/build/compiler.o ./compiler/build/compile_process.o ./compiler/build/scanner.o ./compiler/build/token.o ./compiler/build/scan_process.o ./compiler/build/helpers/buffer.o ./compiler/build/helpers/vector.o 
INCLUDES= -I./

all: ${OBJECTS}
	gcc main.c ${INCLUDES} ${OBJECTS} -g -o ./main 


./compiler/build/compiler.o: ./compiler/compiler.c
	gcc compiler/compiler.c ${INCLUDES} -o ./compiler/build/compiler.o -g -c


./compiler/build/compile_process.o: ./compiler/compile_process.c
	gcc ./compiler/compile_process.c ${INCLUDES} -o ./compiler/build/compile_process.o -g -c


./compiler/build/scanner.o: ./compiler/scanner.c
	gcc compiler/scanner.c ${INCLUDES} -o ./compiler/build/scanner.o -g -c


./compiler/build/token.o: ./compiler/token.c
	gcc compiler/token.c ${INCLUDES} -o ./compiler/build/token.o -g -c


./compiler/build/scan_process.o: ./compiler/scan_process.c
	gcc compiler/scan_process.c ${INCLUDES} -o ./compiler/build/scan_process.o -g -c


./compiler/build/helpers/buffer.o: ./compiler/helpers/buffer.c
	gcc ./compiler/helpers/buffer.c ${INCLUDES} -o ./compiler/build/helpers/buffer.o -g -c


./compiler/build/helpers/vector.o: ./compiler/helpers/vector.c
	gcc ./compiler/helpers/vector.c ${INCLUDES} -o ./compiler/build/helpers/vector.o -g -c

clean:
	rm ./main
	rm -rf ${OBJECTS}
