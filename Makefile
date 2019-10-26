CXX = g++
CXXFLAGS = -std=c++14 -Wall `pkg-config --cflags libndn-cxx` -g
LIBS = `pkg-config --libs libndn-cxx`
DESTDIR ?= /usr/local
SOURCE_OBJS = subscriber.o publisher.o producer.o consumer.o
PROGRAMS = producer consumer

all: $(PROGRAMS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $< $(LIBS)

producer: $(SOURCE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ publisher.o producer.o $(LIBS)

consumer: $(SOURCE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ subscriber.o consumer.o $(LIBS)

clean:
	rm -f $(PROGRAMS) *.o

install: all
	cp $(PROGRAMS) $(DESTDIR)/bin/

uninstall:
	cd $(DESTDIR)/bin && rm -f $(PROGRAMS)
