CXX=g++
CXXFLAGS=-std=c++11 -Wall

all: webserver

webserver: src/main.cpp .c.o
	$(CXX) $(CXXFLAGS) src/main.cpp -o out/webserver -lpthread out/connection_directive.o out/content_type.o out/file_helpers.o out/http_message.o out/http_version.o out/request_type.o out/string_utils.o
	ln -s out/webserver ./webserver

.c.o: src/connection_directive.cpp src/content_type.cpp src/file_helpers.cpp src/http_message.cpp src/http_version.cpp src/request_type.cpp src/string_utils.cpp
	mkdir -p out
	$(CXX) $(CXXFLAGS) -c src/connection_directive.cpp -o out/connection_directive.o
	$(CXX) $(CXXFLAGS) -c src/content_type.cpp -o out/content_type.o
	$(CXX) $(CXXFLAGS) -c src/file_helpers.cpp -o out/file_helpers.o
	$(CXX) $(CXXFLAGS) -c src/http_message.cpp -o out/http_message.o
	$(CXX) $(CXXFLAGS) -c src/http_version.cpp -o out/http_version.o
	$(CXX) $(CXXFLAGS) -c src/request_type.cpp -o out/request_type.o
	$(CXX) $(CXXFLAGS) -c src/string_utils.cpp -o out/string_utils.o

clean:
	rm -r out/*
	rm webserver
