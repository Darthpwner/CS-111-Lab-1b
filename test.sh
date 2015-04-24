echo
#! /bin/sh
echo a | b | c
#echo a && b | c | d || e | f
ls -l

echo a;

echo b

echo a

echo a && echo b

echo a || echo b

exec echo a

exec echo a && echo b

(echo a)

(echo a && echo b)
