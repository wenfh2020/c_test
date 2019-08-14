#!/bin/bash

rsync -az /mnt/c/src/other/c_test /home/work --exclude=.git --exclude=.vscode --exclude=*.log --exclude=*.exe