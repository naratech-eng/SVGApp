CFLAGS = -Wall -g -std=c11 #-D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
BIN = bin/
INC = include/
SRC = src/
XML_PATH = /usr/include/libxml2
LIBDIR = ../

# Standard common makefile
parser: $(BIN)libsvgparse.so deploy

deploy: $(BIN)libsvgparse.so
	mv $(BIN)libsvgparse.so $(LIBDIR)

$(BIN)libsvgparse.so: $(BIN)SVGParser.o $(BIN)SVGValid.o $(BIN)LinkedListAPI.o $(BIN)XMLGeneration.o $(BIN)SVGAttributes.o $(BIN)SVGJSON.o $(BIN)SVGInfo.o $(BIN)test.o
	gcc -shared -o $(BIN)libsvgparse.so $(BIN)SVGParser.o $(BIN)SVGValid.o $(BIN)LinkedListAPI.o $(BIN)XMLGeneration.o $(BIN)SVGAttributes.o $(BIN)SVGJSON.o $(BIN)SVGInfo.o $(BIN)test.o -lxml2 -lm

$(BIN)SVGParser.o: $(SRC)SVGParser.c $(INC)LinkedListAPI.h $(INC)SVGParser.h $(INC)ListHelpers.h $(INC)SVGHelpers.h $(INC)SVGShapeHelpers.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGParser.c -o $(BIN)SVGParser.o

$(BIN)SVGValid.o: $(SRC)SVGValid.c $(INC)LinkedListAPI.h $(INC)SVGParser.h $(INC)ListHelpers.h $(INC)SVGHelpers.h $(INC)SVGShapeHelpers.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGValid.c -o $(BIN)SVGValid.o

$(BIN)SVGAttributes.o: $(SRC)SVGAttributes.c $(INC)LinkedListAPI.h $(INC)SVGParser.h $(INC)ListHelpers.h $(INC)SVGHelpers.h $(INC)SVGShapeHelpers.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGAttributes.c -o $(BIN)SVGAttributes.o

$(BIN)LinkedListAPI.o: $(SRC)LinkedListAPI.c $(INC)LinkedListAPI.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)LinkedListAPI.c -o $(BIN)LinkedListAPI.o

$(BIN)XMLGeneration.o: $(SRC)XMLGeneration.c $(INC)XMLGeneration.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)XMLGeneration.c -o $(BIN)XMLGeneration.o

$(BIN)SVGJSON.o: $(SRC)SVGJSON.c $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGJSON.c -o $(BIN)SVGJSON.o

$(BIN)SVGInfo.o: $(SRC)SVGInfo.c $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)SVGInfo.c -o $(BIN)SVGInfo.o

$(BIN)test.o: $(SRC)test.c $(INC)SVGParser.h
	gcc -c -fpic $(CFLAGS) -I$(XML_PATH) -I$(INC) $(SRC)test.c -o $(BIN)test.o

clean:
	rm $(BIN)*.o $(LIBDIR)*.so
