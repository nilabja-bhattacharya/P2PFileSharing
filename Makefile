all: tracker client

tracker: tracker.cpp
		g++ tracker.cpp -o tracker -pthread  

client: generatehash.cpp P2P.cpp
	g++ P2P.cpp generatehash.cpp -o client  -lcrypto -lpthread

clean: 
		rm -rf *.o tracker
		rm -rf *.o client
		rm -rf *.mtorrent