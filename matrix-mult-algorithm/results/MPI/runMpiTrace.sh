#!/bin/bash

targets=($(pgrep $1))
for i in "${targets[@]}"
do
	./../../dtraceScripts/profile.d $i > $i.txt &
done
