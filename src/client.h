/* Author: Tianyi Chen, tchen59@jhu.edu */
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include "chou_tree.h"

using namespace std;

string 	train_file;

string 	test_file;

/* select which mode to run */
int 	mode;

/* command line parser */
void 	parse_command_line( int argc, char **argv );

void 	initialize( );