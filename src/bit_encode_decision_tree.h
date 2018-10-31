/* Author: Tianyi Chen, tchen59@jhu.edu */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <queue>
#include "agglo_cluster.h"

/* declare these two classes for references */
class Bit_encode_dec_tree;
class Tree_node_bit;

class Tree_node_bit {

public:

	/* default constructor */
	Tree_node_bit();

	/* constructor given bit_decision_tree */
	Tree_node_bit( Bit_encode_dec_tree *bit_tree );

	int 					num_questions;

	int 					dev_num_sample;
	int 					cross_num_sample;
	int 					test_num_sample;

	int 					total_dev_num_sample;
	int 					total_cross_num_sample;
	int 					total_test_num_sample;

	int 					questioned_bit;

	int 					*permitted_bits_idx;

	double 					dev_entropy;

	double 					cross_entropy;

	/* boolean to indicate whether current node is terminal or not */
	bool 					is_terminal;

	string 					path;

	list< string > 			dev_history;

	list< string > 			cross_history;

	list< char > 			letters;

	/* */
	Tree_node_bit 			*parent;
	Tree_node_bit			*left;
	Tree_node_bit 			*right;

	Tree_node_bit			*tmp_left;
	Tree_node_bit 			*tmp_right;

	Tree_node_bit 			*candidate_left;
	Tree_node_bit 			*candidate_right;


	Bit_encode_dec_tree 	*tree;	

	map< string, double > 	dev_probs_map;

	map< string, double > 	cross_probs_map;


	/* compute number of sample */
	int 	compute_num_sample( map< string, map< string, int > > four_gram_map, list< string > history );

	/* calculate probs map */
	map< string, double > 	compute_probs_map( map< string, map< string, int > > four_gram_map, int num_sample, list< string > history );

	/* calculate entropy */
	double 	compute_entropy( map< string, double > probs_map );


	/* split current treenode into two leaves */
	void 	partition( int question_bit );

};


class Bit_encode_dec_tree {

public:
	
	/* public attributes */
	/* train data is */
	char 					*train_data;
	/* test data is */
	char 					*test_data;

	int 					train_size;
	int 					dev_size;
	int 					cross_size;
	int 					test_size;
	

	/* K obtained from allo clustering tree */
	int 					K;

	/* root */
	Tree_node_bit 			*root;

	/* letters list */
	list< char > 			letters;

	/* list of frontier tree nodes */
	list< Tree_node_bit * > frontier_node_list;


	map< char, string > 				coding_book_letter;
	map< string, map< string, int > > 	dev_4_gram;
	map< string, map< string, int > > 	cross_4_gram;
	map< string, map< string, int > > 	test_4_gram;

	Bit_encode_dec_tree();

	Bit_encode_dec_tree( Agglo_cluster_tree *allgo_tree );

	/* encode all letter l \in \mathcal{L} */
	void encoding( Cluster_node * node );

	/* collect development, cross-validation, test data given development percentage */
	void collect_dev_cross_test( double dev_perc );

	/* construct tree */
	void grow_tree();

	/* construct root */
	void set_root();

	/* compute the perplexity of the test data */
	double compute_perplex();

	/* print tool: print bit encode tree by BFS from root node */
	void print_bit_tree_bfs( Tree_node_bit *cur_node );

	/* print tools: print 4-gram map */
	void print_4_gram( map< string, int > four_gram );

	/* print tool: print 4-grams map */
	void print_4_grams( map< string, map< string, int > > four_grams );

};
