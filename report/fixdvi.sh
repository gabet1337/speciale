#!/bin/bash

dvips -t letter -o $1.tmp.ps $1.dvi || exit 1
sed '/^SDict begin \[$/,/^end$/d' $1.tmp.ps > $1.ps
rm $1.tmp.ps

exit 0
