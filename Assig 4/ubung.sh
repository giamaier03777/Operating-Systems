#!/bin/bash

for file in "$1"/*; do
    base_name=$(basename "$file")
    new_name="$2/${base_name%.*}.eins"
    mv "$file" "$new_name"
done