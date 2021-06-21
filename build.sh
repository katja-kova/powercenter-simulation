#!/bin/bash

docker build --target builder -t koda_basegrpc .
docker build --target base -t koda_baseimg .
docker build --target mqtt -t koda_mqtt -f ./Dockerfile ./mosquitto
docker build -f ./powercenter/dockerCenter -t koda_powercenter ./powercenter
docker build -f ./participant/dockerParticipant -t koda_participant ./participant
docker build -f ./external/Dockerfile -t koda_external ./external
