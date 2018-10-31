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
#include <set>
#include <queue>


#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MINIMUM_VALUE 	-1E7
#define INF 			1E7

using namespace std;

/* tree node of Agglo_cluster_tree */
class Cluster_node {

public:

	Cluster_node();

	Cluster_node( char letter );

	/* cluster depth */
	int 			depth; 
	/* display level */	 
	int 			level;   

	double 			I_value;
	
	/* current cluster node is visited or not for display */
	bool 			visited;

	/* code after encoding for current cluster */
	/* codeword for root is 1 */
	string 			codeword;
	
	list< char > 	letters;
	
	Cluster_node 	*left;
	Cluster_node 	*right;

};


class Agglo_cluster_tree {


public:
	
	/* public attributes */
	/* train data is */
	char 					*train_data;
	/* test data is */
	char 					*test_data;

	/* train */
	int 					train_size;
	int 					test_size;

	/* number of depth */
	int 					K;

	/* root of Agglo_cluster_tree */
	Cluster_node 			*root;

	/* list of cluster node */
	list< Cluster_node * > 	clusters;

	/* number of clusters */
	int 					num_clusters;

	/* letters list */
	list< char > 			letters;

	/* unimgram map for training and testng data  */
	map< char, int > 		train_unigrams;

	map< char, int > 		test_unigrams;

	/* bigram map for training and testing data */
	map< char, map< char, int > > train_bigrams;

	map< char, map< char, int > > test_bigrams;

	/* default constructor */
	Agglo_cluster_tree( );

	/* constructor given training, testing file */
	Agglo_cluster_tree( string train_file, string test_file );

	/* load necessary data from given file */
	void load_data( string train_file, string test_file );

	/* collect unigram and bigram from training data file and testing file */
	void collect_uni_bi_gram( );

	/* initialize clusters */
	void initialize_clusters( );

	/* grow tree */
	void grow_tree( );

	/* compute I */
	double calculate_I( Cluster_node *cluster_i, int i, Cluster_node *cluster_j, int j );

	/* compute f( C_1, C_2 ) * log f( C_1, C_2 ) / f( C_1 )* f( C_2 )  */
	double calculate_score( Cluster_node *cluster_1, Cluster_node *cluster_2 );

	/* compute f( cluster )*/
	void freq_unigram( Cluster_node cluster );

	/* compute f( C_1, C_2 )*/
	void freq_bigram( Cluster_node cluster_1, Cluster_node cluster_2 );

	/* union two clusters */
	Cluster_node *union_clusters( Cluster_node *cluster_1, Cluster_node *cluster_2 );

	/* find the best N clusters */
	void find_N_best_clusters( int N );

	/* print tools: print unigram map */
	void print_unigram( map< char, int > unigrams );

	/* print tool: print bigram map */
	void print_bigram( map< char, map< char, int > > bigrams );

	/* print tool: print cluster */
	void print_clusters( );
	
	void print_cluster( Cluster_node *cluster );

	/* print tool: print clustering tree by DFS */
	void print_cluster_tree_dfs( Cluster_node *cur_cluster );

	/* print tool: print clustering tree by BFS from cur_cluster node */
	void print_cluster_tree_bfs( Cluster_node *cur_cluster );

};



