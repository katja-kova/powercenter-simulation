#!/bin/bash

rm -f A_*;
echo "Expecting Error-Code 400:";
cat 400_REQ | nc 127.0.0.1 80 > A_400;
cat A_400;
echo "Expecting Error-Code 403:";
cat 403_REQ | nc 127.0.0.1 80 > A_403;
cat A_403;
echo "Expecting Error-Code 404:";
cat 404_REQ | nc 127.0.0.1 80 > A_404;
cat A_404;
echo "Expecting Error-Code 405:";
cat 405_REQ | nc 127.0.0.1 80 > A_405;
cat A_405;
echo "Expecting Error-Code 200:";
cat 200_REQ | nc 127.0.0.1 80 > A_200;
cat A_200;
echo "Script finished successfully!";
