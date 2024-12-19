 #!/bin/bash

   # Check if the parameter is provided
   if [ -z "$1" ]; then
       echo "No input provided."
       exit 1
   fi

   # Check if the parameter is a number
   if [[ "$1" =~ ^-?[0-9]+$ ]]; then
       echo "Invalid input: Number provided."
   else
       echo "Parameter value: $1"
   fi
