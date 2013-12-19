all:
	g++ -o server server.cpp
	g++ -o client client.cpp
	g++ -o epi epi.cpp -lgmp -lmpfr
	g++ -o pi pi.cpp -lgmp
	g++ -o e e.cpp -lgmp
