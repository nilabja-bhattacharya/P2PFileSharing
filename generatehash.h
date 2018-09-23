#ifndef TEST_H_INCLUDED
#define TEST_H_INCLUDED
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <unistd.h>
#include <openssl/sha.h>
bool generatesha(char *input);
void createmtorrent(const char *filename, const char *mfilename);
#endif