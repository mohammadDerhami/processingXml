TARGET = test

SRCS = server.cpp parseXml.cpp database.cpp

CXX = g++
CXXFLAGS = -Wall -g -I/usr/include/libxml2

LIBS = -lxml2 -lsqlite3
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

clean:
	rm -f $(TARGET) *.o
