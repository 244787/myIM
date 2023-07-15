#!/bin/bash

for pid in $(ps -ef | grep tcp | awk '{print $2}'); do
    kill -9 $pid
done
