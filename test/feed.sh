#!/bin/bash
counter=0
delay=2
while [ $counter -lt 1000 ]; do
   cat test.in
   sleep $delay
   let counter=counter+1
done
