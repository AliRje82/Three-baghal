#!/bin/bash

if [[ $1 == "setup" ]];then
    echo "Init the shared memory "
    ./Init
    echo "Done..."
else
    echo "Cleaning up the shared memory!"
    ./Cleanup
    echo "Done..."
fi