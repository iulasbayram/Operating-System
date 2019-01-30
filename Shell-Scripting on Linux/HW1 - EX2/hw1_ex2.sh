#!/bin/bash

# Ismail Ulas Bayram - 220201040

echo "Enter a sequence of numbers followed by 'end'"

loop="true" # this is our control flag which is "true"

max=0 # we initialize max variable

while [[ "$loop" != "end" ]]; do # While loop works until our control flag is equal to "end" word
	
	read condition # Taking input from terminal

	if [ $condition = "end" ]; then # If our condition variable is equal to "end" word
   		
   		echo "Maximum : $max" # We print final result
   		
   		loop="end" # Our control flag is equal to "end" word and while loop finishes its job

   	elif [[ "$condition" -gt "$max" ]]; then # If our condition number is greater that max number, so new number is assigned as maximum number
   		max=$condition
	fi
done