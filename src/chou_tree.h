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
#include "bit_encode_decision_tree.h"

/* declare these two classes for references */
class Tree_node_chou;
class Chou_tree;

class Tree_node_chou {

public:

	int 					dev_num_sample;
	int 					cross_num_sample;
	int 					test_num_sample;

	int 					total_dev_num_sample;
	int 					total_cross_num_sample;
	int 					total_test_num_sample;

	int 					num_questions;

	int 					questioned_bit;

	/* boolean to indicate whether current node is terminal or not */
	bool 					is_terminal;

	double 					dev_gini_index;

	double 					cross_gini_index;

	string 					path;

	/* letters list */
	list< char > 			letters;
	
	list< string > 			dev_history;

	list< string > 			cross_history;

	Tree_node_chou			*parent;

	Tree_node_chou			*left;
	Tree_node_chou 			*right;

	Tree_node_chou			*tmp_left;
	Tree_node_chou 			*tmp_right;

	Tree_node_chou 			*candidate_left;
	Tree_node_chou 			*candidate_right;

	Chou_tree			 	*tree;	

	map< string, double > 	dev_probs_map;

	map< string, double > 	cross_probs_map;

	set< char > 			A_set;

	

	/* compute number of sample */
	int 	compute_num_sample( map< string, map< string, int > > four_gram_map, list< string > history );

	/* calculate probs map */
	map< string, double > 	compute_probs_map( map< string, map< string, int > > four_gram_map, int num_sample, list< string > history );

	/* compute Gini index given probability map */
	double 	compute_gini_index( map< string, double > probs_map );

	/* find the best partition */
	void  	partition_best( int question_i );

	/* print tool */
	void print_set( set< char > char_set );

	/* print tool */
	void print_set_history( set< string > string_set );

	/* Default constructor */
	Tree_node_chou( );

};

class Chou_tree {

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

	int 					K;

	/* root */
	Tree_node_chou 			*root;

	/* letters list */
	list< char > 			letters;

	/* list of frontier tree nodes */
	list< Tree_node_chou * > frontier_node_list;

	map< string, map< string, int > > 	dev_4_gram;
	map< string, map< string, int > > 	cross_4_gram;
	map< string, map< string, int > > 	test_4_gram;


	Chou_tree();

	Chou_tree( Agglo_cluster_tree *allgo_tree );

	/* collect development, cross-validation, test data given development percentage */
	void collect_dev_cross_test( double dev_perc );

	/* collect total sample given history set */
	int collect_count_sum( set< string > history_set, map< string, map< string, int > > gram );

	int collect_word_sum( set< string > history_set, map< string, map< string, int > > gram, string word );
	
	/* compute the perplexity of the test data */
	double compute_perplex();

	/* construct root */
	void set_root();

	/* construct tree */
	void grow_tree();



};