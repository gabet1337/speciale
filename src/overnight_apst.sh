#!/bin/bash
rm -rf out.out
c=1;
while true
do
    echo $c;
    c=$((c+1));
    ./apst > out.out
    rc=$?;
    if [[ $rc != 0 ]];
    then exit $rc; fi;
    rm out.out;
done
