This is the documentation file describing the usage and internal design of the "myweb" software tool. 

The tool is meant to mimic the functionality of "curl", with slightly different command line options. 
To generate an executable version of the tool, run "make" at the top level of the lab directory. 
This will place an executable file in the bin folder. 
You can run the tool from the top directory or in the bin directory. 
In the case that you are in the bin directory, the tool's usage would be as followed:

    ./myweb <hostName> <ipAddress>[:<port>]/<documentPath> [-h]

The options in brackets are optional arguments.

Internally, the tool is programmed in C++. I start by fetching and parsing the command line arguments.
I first verify that there are a proper number of command line arguments as well as the validity of the optional "-h" argument.
Following that, I parse the command line arguments to isolate the Host Name, IP Address, Port Number (if present),
and the document path.
Following that, I use those inputs to open a socket. At this step, I also verify the validity of the IP Address,
and set a default port number if a port number is not provided by input.
I then attempt to make connection to server using the details provided by the input. If successful,
I construct the request line. After sending the request, I receive the response from the server.
I handle the response based on the command line options provided, ensuring the proper behavior.
I finish up by freeing all my used memory and closing the output file and socket.

As for test cases, I tested using 5 cases:

1. General functionality: ./bin/myweb www.example.com 93.184.216.34:80/index.html
    - I compared this output to the output from the example in the lab document: 
        curl 93.184.216.34:80/index.html -H "Host: www.example.com" -o output.dat

2. Command line option "-h": ./bin/myweb www.example.com 93.184.216.34:80/index.html -h
    - I compared this output to the output from the example in the lab document: 
        curl 93.184.216.34/index.html -I -H "Host: www.example.com"

3. Testing Optional Port: ./bin/myweb neverssl.com 34.223.124.45/index.html
    -  I wanted to make sure that even without a port input, the program could execute as expected

4. Improper command line inputs:
    - I did multiple tests for this:
        - Invalid IP: ./bin/myweb neverssl.com 34.34.1/index.html
            - Response: Invalid Address / Address not supported
        - Missing Doc Path: ./bin/myweb neverssl.com 34.223.124.45
            - Response: Unable to determine Document Path
        - Missing HostName: ./bin/myweb 34.223.124.45/index.html
            - Response: Invalid number of command line arguments
        - Invalid options: ./bin/myweb neverssl.com 34.223.124.45/index.html -random
            - Response: Invalid command line option: -random

5. Additional functionality tests:
    - I did tests on the following examples:
        - http://neverssl.com/
        – http://www.softwareqatest.com/
        – http://www.testingmcafeesites.com/
        – http://pudim.com.br/
        – http://www.worldslongestwebsite.com/

Everything was also tested with valgrind to ensure that there was no memory loss.

Some potential shortcomings could be efficiency, as in some cases the program takes more than 1 second to complete.
Another shortcoming could be if the socket connection hangs, as my implementation does not automatically detect it,
but I have not been able to test that case.
