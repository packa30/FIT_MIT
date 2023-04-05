#!/bin/sh
docker-compose up -d
make
sleep 5
gnome-terminal -- python3 consumer.py
sudo ./producer $1 $2 $3
