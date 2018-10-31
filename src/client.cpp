#include "client.h"

/* Author: Tianyi Chen, tchen59@jhu.edu */

int main( int argc, char **argv ){ 

	parse_command_line( argc, argv );

	initialize( );

	/* construct agglomerative clustering tree */
	if ( mode == 1 ) {

		cout<< "Initialize agglomerative clustering tree..."<< endl;
		Agglo_cluster_tree 	agg_cluster = Agglo_cluster_tree( train_file, test_file );
		
		cout<< "Agglomerative clustering tree is growing..."<< endl;
		agg_cluster.grow_tree( );

		/* Print out growed agglomerative clustering tree */
		cout<< "Print out growed agglomerative clustering tree..." << endl;		
		agg_cluster.print_cluster_tree_bfs( agg_cluster.root );

	} 
	/* construct Bit-encoding based decision tree */
	else if ( mode == 2 ) {


		cout<< "Constructing agglomerative clustering tree..."<< endl;
		Agglo_cluster_tree 	agg_cluse_2 = Agglo_cluster_tree( train_file, test_file );
		agg_cluse_2.grow_tree( );

		/* initialize Bit encoding decision tree */
		cout<< "Construct bit encoding decision tree..."<< endl;
		Bit_encode_dec_tree bit_tree 	= Bit_encode_dec_tree( &agg_cluse_2 );

		/* encoding l1, l2, l3, l4 */
		cout<< "Encoding l1, l2, l3, l4..." << endl;
		bit_tree.encoding( agg_cluse_2.root );

		/* build development, and held-out data sets */
		cout<< "Build development, and held-out data sets..."<< endl;
		bit_tree.collect_dev_cross_test( 0.8 );

		/* Bit encoding decision tree is growing */
		cout<< "Bit encoding decision tree is growing..." << endl;
		fprintf(stderr, "Process: " );
		bit_tree.grow_tree();

		/* Print out growed bit encoding decision tree*/
		cout<< "Print out growed bit encoding decision tree..." << endl;
		bit_tree.print_bit_tree_bfs( bit_tree.root );

		/* Calculate perplexity on testing data set */
		cout<< "Calculate perplexity on testing data set..." << endl;
		double perplexity_bit_tree;
		perplexity_bit_tree 			= bit_tree.compute_perplex();
		cout<< "perplexity is: "<< perplexity_bit_tree <<endl;	

	}
	/* Construct Chou's decision tree */
	else if ( mode == 3 ) {

		cout<< "Constructing agglomerative clustering tree..."<< endl;
		Agglo_cluster_tree 	agg_cluse_3 = Agglo_cluster_tree( train_file, test_file );

		/* initialize Chou's decision tree */
		cout<< "Initialize Chou's decision tree..."<< endl;
		Chou_tree 			chou_tree 	= Chou_tree( &agg_cluse_3 );	

		/* build development, and held-out data sets */
		cout<< "Build development, and held-out data sets..."<< endl;		
		chou_tree.collect_dev_cross_test( 0.8 );

		/* Chou's decision tree is growing */
		cout<< "Chou's decision tree is growing...(A little slow, allow several minutes to one hour to complete...)" << endl;
		fprintf(stderr, "Process: " );
		chou_tree.grow_tree();

		/* Calculate perplexity on testing data set */
		cout<< "Calculate perplexity on testing data set..." << endl;	
		double perplexity_chou_tree;
		perplexity_chou_tree 			= chou_tree.compute_perplex();
		cout<< "perplexity is: "<< perplexity_chou_tree <<endl;	

	}

	return 1;
}

/* command line parser */
void parse_command_line( int argc, char **argv ){ 

	int i;

	for ( i = 1; i < argc; i++ ){ 
		if( argv[i][0] != '-' ) break;
		if( ++i >= argc ){ 
			perror("Invalid arguments.");
			exit(0);
		}
		switch( argv[i-1][1] ){ 
			case 'm':
				mode 			= atoi( argv[i] );
				break;
		}
	}
}


/* intialize parameter */
void initialize( ) {

	train_file	= "data/textA.txt";

	test_file 	= "data/textB.txt";

}