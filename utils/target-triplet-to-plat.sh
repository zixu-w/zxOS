#!/bin/sh
if echo "$1" | grep -Eq 'i[[:digit:]]86-'; then
  echo x86
else
  echo "$1" | grep -Eo '^[[:alnum:]_]*'
fi
