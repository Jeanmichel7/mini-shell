#!/bin/bash

echo "Install Homebrow"
mkdir ~/goinfre/homebrew
curl -L https://github.com/Homebrew/brew/tarball/master | tar xz --strip 1 -C ~/goinfre/homebrew
brew install readline
echo ""