FROM docker.io/ubuntu:latest
RUN apt-get update && apt-get upgrade -y
RUN apt-get install make g++ -y
