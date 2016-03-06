#!/bin/bash
counter=0
delay=1
while [ $counter -lt 1000 ]; do
   cat $1
   sleep $delay
   let counter=counter+1
done
