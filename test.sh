#!/bin/bash

# Define the server details
SERVER="127.0.0.1"
PORT="3333"

# Connect to the server
exec 3<>/dev/tcp/$SERVER/$PORT

# Check if the connection was successful
if [ $? -ne 0 ]; then
  echo "Failed to connect to the server"
  exit 1
fi

# Read the initial response from the server
read -r response <&3
echo "Server response: $response"

# Send a test SMTP command
echo "EHLO" >&3

# Read the response from the server
read -r response <&3
echo "Server response: $response"

# Close the connection
echo "QUIT" >&3

# Read the final response from the server
read -r response <&3
echo "Server response: $response"

# Close the connection
exec 3<&-
exec 3>&-
