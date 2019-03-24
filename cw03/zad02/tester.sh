#!/bin/bash
while true
do 
    let SEC=`shuf -i $2-$3 -n 1`
    echo $$ $SEC `date` `head -c $4 /dev/urandom`>> $1
    sleep $SEC
done