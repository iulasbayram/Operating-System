#!/bin/bash

# Ismail Ulas Bayram - 220201040

X=$1 # First, we assign input to our variable called as X.

if [ -f $X ]; then # Then we check whether file exist or not.

	while IFS='' read -r line || [[ -n "$line" ]]; do # The file is read line by line

   		for ((i=0; i<$line; i++)){ # For loop works number of line times
   			echo -ne "*" # Then every star concatinates with the other number of line times by using -ne expression
		}
		echo
	done < "$1"
else
	echo "File does not exist" # If the file does not exist
fi
