
PRGM = run
BASEPATH = .
DESTDIR = $(BASEPATH)/run
PUBINC = $(BASEPATH)/comm/include
PUBLIBPATH = -L./
# PUBLIB = -lpthread
PUBOPTS = -D MARKUP_STL
CC = g++
CXXFLAGS = -Wall -O -g
all = $(PRGM)
VPATH = $(PUBINC)
.SUFFIXES: .cpp .cc .o
SRCSCC := $(wildcard *.cc)
SRCSCPP := $(wildcard *.cpp)
SRCS = $(SRCSCC) $(SRCSCPP)
OBJS = $(addsuffix .o, $(basename $(SRCS)))

.cpp.o:
	$(CC) $(CXXFLAGS) -I$(PUBINC) $(PUBOPTS) -c $< -o $@
.cc.o:
	$(CC) $(CXXFLAGS) -I$(PUBINC) $(PUBOPTS) -c $< -o $@
$(PRGM):$(OBJS)
	$(CC) $(PUBLIBPATH) $(OBJS) -o $(PRGM) $(PUBLIB) 
	#-lthostmduserapi -lhiredis
	#cp ./$(PRGM) $(DESTDIR)
.PHONY:clean
logclean:
	rm -rf Log/*
clean:
	rm -rf *.o $(PRGM)
