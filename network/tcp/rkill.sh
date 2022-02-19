#!/bin/bash

server_name='my-redis-server'
ps -ef | grep -i $server_name | grep -v grep | awk '{if ($3 > 1) print $3;}' | xargs sudo kill -9
num=`ps -ef | grep -i $server_name | grep -v grep | awk '{if ($3 > 1) print $3;}' | wc -l`
echo "redis instances: $num"