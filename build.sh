#!/bin/bash

docker build -f ./Dockerfile -t koda_baseimg .
docker build -f ./powercenter/dockerCenter -t koda_powercenter ./powercenter
docker build -f ./participant/dockerParticipant -t koda_participant ./participant
