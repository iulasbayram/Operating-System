#!/bin/bash

# Ismail Ulas Bayram - 220201040

count=0 # Number of zero byte file

if ! [ -z "$1" ]; then # If argument is not empty

	for file in "$1"/*; do # Looking each file in directory

		if [[ -f $file ]] && [[ ! -s $file ]]; then # If size of file is zero

			rm -f "$file" # Deleting zero byte file
			
			((count++)) # Incrementing count
		fi 
	done

	echo "$count zero-length files are removed from the directory: $1"
else
	echo "Argument is empty." # If argument is empty
fi

