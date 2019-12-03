#!/bin/bash
for((i=16; i<256; i++)); do
    printf "\e[48;5;${i}m%03d" $i;
    printf '\e[0m';
    [ ! $((($i - 15) % 6)) -eq 0 ] && printf ' ' || printf '\n'     
done

printf '\n'   

# 第二个参数有效果， 1变粗  2斜体 5闪烁
for((i=16; i<256; i++)); do
    printf "\e[0;1;${i}m%03d" $i;
    printf '\e[0m';
    [ ! $((($i - 15) % 6)) -eq 0 ] && printf ' ' || printf '\n'     
done

printf '\n'   