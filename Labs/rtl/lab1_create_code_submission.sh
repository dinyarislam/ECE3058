#!/bin/bash

rm -f ece3058_lab1_submission.tar.gz
tar -czvf ece3058_lab1_submission.tar.gz control.v decode.v execute.v fetch.v memory.v mips.v 
echo "Done!"
echo "The files that will be submitted are:"
tar -ztvf ece3058_lab1_submission.tar.gz