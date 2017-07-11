#!/bin/bash

set -e

for PROJ in agent consumer provider; do
  echo "BUILD/INSTALL $PROJ" 
  cd $PROJ
  ./configure
  make
  sudo make install
  cd ..
done
