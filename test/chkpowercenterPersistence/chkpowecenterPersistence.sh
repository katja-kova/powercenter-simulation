#!/bin/bash

touch HTTP_ANSWER;
echo "Sending UDP data...";
cat UDP_REQ | nc -u 192.168.0.103 50000;
echo "Waiting for HTTP reply...";
cat HTTP_REQ | nc 192.168.0.103 80 > HTTP_ANSWER;
echo "Expecting HTTP-Reply with Code 200 OK and the previously sent data:";
cat HTTP_ANSWER;
echo "Script finished successfully!";
