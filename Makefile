all:
	g++ http_server.cpp -o http_server
	g++ multi_http_server.cpp -lpthread -o multi_http_server
	g++ http_client.cpp -o http_client
	g++ udp_server.cpp -o udp_server
	g++ udp_client.cpp -o udp_client


