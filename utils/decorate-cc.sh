#!/bin/bash
if echo "$1" | grep -Eq '-elf($|-)'; then
  echo "-isystem=$2"
else
  echo ""
fi
