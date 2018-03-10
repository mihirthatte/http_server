****Run make****

## To run single threaded server -
command -
```
./http_server [PORT_NUMBER]
```
example command -
```./http_server 20038```
(Try different port if server fails to bind)

## Brower Client-
From the broswer,

enter url - **http://burrow.soic.indiana.edu:20038/HelloWorld.html **

## http client -
command -
```./http_client [HOST_NAME] [PORT] [Connection-Type] [filenames]```
example command -
```./http_client burrow.soic.indiana.edu 20038 NP sample1.txt sample2.txt sample3.txt```
Connection-Type -> NP - Non persistent Connection & P - Persistent Connection
where number of files can be 1 to n

## To run multi threaded server -
command -
```./multi_http_server [PORT_NUMBER]```
Example -
```./http_server 20038```

## To run UDP server -
command -
```./udp_server [PORT_NUMBER]```
Example -
```./udp_server 20038```

## UDP Client -
command -
```./udp_client [HOST_NAME] [PORT] [filenames]```
Example command -    
```./udp_client burrow.soic.indiana.edu 20038 sample1.txt sample2.txt```
