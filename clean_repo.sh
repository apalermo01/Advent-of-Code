#!/bin/bash 

shopt -s globstar
for f in **/*.txt; do 
  git filter-repo --invert-paths --path $f 
  echo "removed $f"
done
