/* Author: Tianyi Chen, tchen59@jhu.edu */
#include "agglo_cluster.h"

Cluster_node::Cluster_node( ) {

	this->level 	= 0;
	this->left 		= NULL;
	this->right 	= NULL;
	this->I_value 	= MINIMUM_VALUE;
	this->visited 	= false;

}


Cluster_node::Cluster_node( char letter ) {

	this->letters.push_back( letter );
	this->left 		= NULL;
	this->right 	= NULL;
	this->I_value 	= MINIMUM_VALUE;
	this->level 	= 0;
	this->visited 	= false;

}


Agglo_cluster_tree::Agglo_cluster_tree( ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );

}


Agglo_cluster_tree::Agglo_cluster_tree( string train_file, string test_file ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );


	/* load data from train_file, and test_file */
	this->load_data( train_file, test_file );


	/* collect unigram and bigram from training data and testing data */
	this->collect_uni_bi_gram( );

	/* initialize clusters */
	this->initialize_clusters( );

	// this->print_clusters();

}

/* load data from training and testing file */
void Agglo_cluster_tree::load_data( string train_file, string test_file ) {

	int 		i;

	ifstream 	train_input_file( train_file );
	ifstream 	test_input_file( test_file );

	string 		train_line;
	string 		test_line;

	/* read from training data */
	if ( train_input_file.is_open() ) {

		while ( getline( train_input_file, train_line ) ) {
		}

		this->train_size 	= train_line.length();
		this->train_data 	= Malloc( char, this->train_size );

		for ( i = 0; i < this->train_size; i++ ) {
			this->train_data[i] 	= train_line.at(i);
		}

	}

	/* read from testing data */
	if ( test_input_file.is_open() ) {

		while ( getline( test_input_file, test_line ) ) {
		}

		this->test_size 	= test_line.length();
		this->test_data 	= Malloc( char, this->test_size );

		for ( i = 0; i < this->test_size; i++ ) {
			this->test_data[i] 	= test_line.at(i);
		}
		
	}

	train_input_file.close();
	test_input_file.close();

}

/* collect unigram and bigram from training data and testing data */
void Agglo_cluster_tree::collect_uni_bi_gram( ) {

	int 	i;
	char 	cur_char;
	char 	next_char;
	map< char, int >::iterator 					it_unigram;
	map< char, map< char, int > >::iterator 	it_bigram;


	/* collect unigram and bigram in training data */
	for ( i = 0; i < this->train_size; i++ ) {

		cur_char 				= this->train_data[i];
		/* collect unigram */
		if ( this->train_unigrams.count( cur_char ) == 0 ) {
			this->train_unigrams.insert( make_pair( cur_char, 1 ) );
		} else {
			it_unigram 			= this->train_unigrams.find( cur_char );
			it_unigram->second 	+= 1;
		}

		/* collect bigram */
		if ( i == this->train_size - 1 ) {
			/* skip the last character */
			break;
		}

		next_char 				= this->train_data[ i + 1 ];

		if ( this->train_bigrams.count( cur_char ) == 0 ) {
			map< char, int > 	new_bigram;
			new_bigram.insert( make_pair( next_char, 1 ) );
			this->train_bigrams.insert( make_pair( cur_char, new_bigram ) );
		} else {
			map< char, int > 	old_bigram 	= this->train_bigrams.find( cur_char )->second;
			if ( old_bigram.count( next_char ) == 0 ) {
				this->train_bigrams.find( cur_char )->second.insert( make_pair( next_char, 1 ) );
			} else {
				this->train_bigrams.find( cur_char )->second.find( next_char )->second 	+= 1;
			}
		}
	}

	/* collect unigram and bigram in testing data */
	for ( i = 0; i < this->test_size; i++ ) {

		cur_char 				= this->test_data[i];
		/* collect unigram */
		if ( this->test_unigrams.count( cur_char ) == 0 ) {
			this->test_unigrams.insert( make_pair( cur_char, 1 ) );
		} else {
			it_unigram 			= this->test_unigrams.find( cur_char );
			it_unigram->second 	+= 1;
		}

		/* collect bigram */
		if ( i == this->test_size - 1 ) {
			/* skip the last character */
			break;
		}		

		next_char 				= this->test_data[ i + 1 ];

		if ( this->test_bigrams.count( cur_char ) == 0 ) {
			map< char, int > 	new_bigram;
			new_bigram.insert( make_pair( next_char, 1 ) );
			this->test_bigrams.insert( make_pair( cur_char, new_bigram ) );
		} else {
			map< char, int > 	old_bigram 	= this->test_bigrams.find( cur_char )->second;
			if ( old_bigram.count( next_char ) == 0 ) {
				this->test_bigrams.find( cur_char )->second.insert( make_pair( next_char, 1 ) );
			} else {
				this->test_bigrams.find( cur_char )->second.find( next_char )->second 	+= 1;
			}
		}

	}	

}


/* initialize clusters */
void Agglo_cluster_tree::initialize_clusters( ) {

	int 	i;
	list< char >::iterator 	it_letters;

	/* at first, each cluster contain one letter */
	for ( it_letters = this->letters.begin(); it_letters != this->letters.end(); it_letters++ ) {
		// cout<< *it_letters << endl;
		this->clusters.push_back( new Cluster_node( *it_letters ) );
	}

}

/* cluster tree */
void Agglo_cluster_tree::grow_tree( ) {

	int 								i_star;
	int 								j_star;
	int 								i;
	int 								j;
	double 								best_I; 
	double 								curr_I;
	list< Cluster_node * >::iterator 	it_cluster_i;
	list< Cluster_node * >::iterator 	it_cluster_j;
	Cluster_node 						*cluster_i_star;
	Cluster_node 						*cluster_j_star;
	Cluster_node 						*union_clusters;

	/* cluster */
	while( true ) {

		this->num_clusters 	= this->clusters.size();

		/* terminatiion condition */
		if ( this->num_clusters == 1 ) {
			break;
		}

		i_star 		= -1;
		j_star 		= -1;

		best_I	 	= MINIMUM_VALUE;

		for ( i = 0, it_cluster_i = this->clusters.begin(); it_cluster_i != this->clusters.end(); it_cluster_i++, i++ ) {
			for ( j = 0, it_cluster_j = this->clusters.begin(); it_cluster_j != this->clusters.end(); it_cluster_j++, j++ ) {
				if ( i != j ) {
					curr_I 		= this->calculate_I( *it_cluster_i, i, *it_cluster_j, j );
					/* greedy idea */
					if ( curr_I >= best_I ) {
						best_I	= curr_I;
						i_star 	= i;
						j_star 	= j;
					}
				}
			}
		}

		/* update clusters */
		for ( i = 0, it_cluster_i = this->clusters.begin(); i < this->num_clusters;  ) {

			if ( i == i_star ) {
				cluster_i_star 	= *it_cluster_i;
				it_cluster_i 	= this->clusters.erase( it_cluster_i );
				i++;
			} 

			if ( i == j_star ) {
				cluster_j_star 	= *it_cluster_i;
				it_cluster_i 	= this->clusters.erase( it_cluster_i );
				i++;
			}

			if ( i != i_star && i != j_star ) {
				it_cluster_i++;
				i++;
			}
		
		}

		/* merge C_i_star and C_j_star and add it into clusters list */
		union_clusters 				= this->union_clusters( cluster_i_star, cluster_j_star );
		union_clusters->I_value 	= best_I;
		this->clusters.push_back( union_clusters );
		// this->print_clusters();

	}

	this->root 				= this->clusters.front();
	this->root->codeword 	= "";
	this->K 				= this->root->level;

}


/* compute I */
double Agglo_cluster_tree::calculate_I( Cluster_node *cluster_i, int i, Cluster_node *cluster_j, int j ) {

	int 	k;
	int 	m;
	double 	I_i_j;

	list< Cluster_node * >::iterator it_cluster_k;
	list< Cluster_node * >::iterator it_cluster_m;

	I_i_j 	= 0.0;

	for ( k = 0, it_cluster_k = this->clusters.begin(); it_cluster_k != this->clusters.end(); k++, it_cluster_k++ ) {
		
		if ( k != i && k !=j ) {

			/* compute C_k and C_i \cup C_j */
			I_i_j 	+= this->calculate_score( *it_cluster_k, this->union_clusters( cluster_i, cluster_j ) );
			
			/* compute the C_k, C_m */
			for ( m = 0, it_cluster_m = this->clusters.begin(); it_cluster_m != this->clusters.end(); m++, it_cluster_m++ ) {
				if ( m != i && m != j ) {
					I_i_j 	+= this->calculate_score( *it_cluster_k, *it_cluster_m );
				}
			}
		}
	}

	/* compute on C_i \cup C_j and C_m*/
	for ( m = 0, it_cluster_m = this->clusters.begin(); it_cluster_m != this->clusters.end(); m++, it_cluster_m++ ) {
		if ( m != i && m != j ) {
			I_i_j 	+= this->calculate_score( this->union_clusters( cluster_i, cluster_j ), *it_cluster_m );
		}
	}

	/* plus C_i \cup C_j */
	I_i_j 	+= this->calculate_score( this->union_clusters( cluster_i, cluster_j ), this->union_clusters( cluster_i, cluster_j ) );

	// cout << I_i_j << endl;
	return I_i_j;
}


/* compute f( C_1, C_2 ) * log f( C_1, C_2 ) / f( C_1 )* f( C_2 )  */
double Agglo_cluster_tree::calculate_score( Cluster_node *cluster_1, Cluster_node *cluster_2 ) {

	double 					f_cluster_1;
	double 					f_cluster_2;
	double 					f_cluster_1_2;
	double 					result;
	list< char >::iterator 	it_letter_1;
	list< char >::iterator 	it_letter_2;

					
	/* calculate f( C_1 ) and f( C_2 )*/
	f_cluster_1 			= 0.0;
	f_cluster_2 			= 0.0;
	for ( it_letter_1 = cluster_1->letters.begin(); it_letter_1 != cluster_1->letters.end(); it_letter_1++ ) {
		f_cluster_1 		+= (double) this->train_unigrams.find( *it_letter_1 )->second;
	}
	for ( it_letter_2 = cluster_2->letters.begin(); it_letter_2 != cluster_2->letters.end(); it_letter_2++ ) {
		f_cluster_2 		+= (double) this->train_unigrams.find( *it_letter_2 )->second;
	}

	/* calculate f( C_1, C_2 ) */
	f_cluster_1_2 			= 0.0;

	for ( it_letter_1 = cluster_1->letters.begin(); it_letter_1 != cluster_1->letters.end(); it_letter_1++ ) {
		for ( it_letter_2 = cluster_2->letters.begin(); it_letter_2 != cluster_2->letters.end(); it_letter_2++ ) {
			if ( this->train_bigrams.count( *it_letter_1 ) != 0 ) {
				if ( this->train_bigrams.find( *it_letter_1 )->second.count( *it_letter_2 ) != 0 ) {
					f_cluster_1_2 += (double) this->train_bigrams.find( *it_letter_1 )->second.find( *it_letter_2 )->second;
				}			
			}
		}
	}

	/* compute f( C_1, C_2 ) * log_2 f( C_1, C_2 ) / f( C_1 )* f( C_2 ) */
	result 					= f_cluster_1_2 > 0.0 ? f_cluster_1_2 * log2( f_cluster_1_2 / ( f_cluster_1 * f_cluster_2 ) ) : 0.0;

	return result;

}

/* union two clusters */
Cluster_node *Agglo_cluster_tree::union_clusters( Cluster_node *cluster_1, Cluster_node *cluster_2 ) {

	Cluster_node *cluster_union = new Cluster_node();

	list< char >::iterator 	it_letter;

	for ( it_letter = cluster_1->letters.begin(); it_letter != cluster_1->letters.end(); it_letter++ ) {
		cluster_union->letters.push_back( *it_letter );
	}

	for ( it_letter = cluster_2->letters.begin(); it_letter != cluster_2->letters.end(); it_letter++ ) {
		cluster_union->letters.push_back( *it_letter );
	}

	/* set left branch, right branch and level */
	cluster_union->left 		= cluster_1;
	cluster_union->right 		= cluster_2;
	cluster_union->level 		= MAX( cluster_1->level, cluster_2->level ) + 1;

	return cluster_union;

}

/* print tools: print unigram map */
void Agglo_cluster_tree::print_unigram( map< char, int > unigrams ) {

	map< char, int >::iterator 	it_unigram;

	for ( it_unigram = unigrams.begin(); it_unigram != unigrams.end(); it_unigram++ ) {

		cout<< it_unigram->first << " , " << it_unigram->second << endl;

	}

}

/* print tool: print bigram map */
void Agglo_cluster_tree::print_bigram( map< char, map< char, int > > bigrams ) {

	map< char, map< char, int > >::iterator 	it_bigram;

	for ( it_bigram = bigrams.begin(); it_bigram != bigrams.end(); it_bigram++ ) {

		cout << "================================" << endl;
		cout << "Current character: " << it_bigram->first << endl;
		cout << "--------------------------------" << endl;

		this->print_unigram( it_bigram->second );

	}

}

/* print tool: print clusters */
void Agglo_cluster_tree::print_clusters( ) {

	list< Cluster_node * >::iterator 	it_cluster;

	cout<< "===================================" << endl;
	for ( it_cluster = this->clusters.begin(); it_cluster != this->clusters.end(); it_cluster++ ) {

		this->print_cluster( *it_cluster );

	}
}

void Agglo_cluster_tree::print_cluster( Cluster_node *cluster ) {

	list< char >::iterator 			it_letter;
	for ( it_letter = cluster->letters.begin(); it_letter != cluster->letters.end(); it_letter++ ) { 
		cout<< *it_letter << ", ";
	}
	cout<<endl;

}

/* print tool: print clustering tree */
void Agglo_cluster_tree::print_cluster_tree_dfs( Cluster_node *cur_cluster ) {

	this->print_cluster( cur_cluster );

	if ( cur_cluster->left == NULL && cur_cluster->right == NULL ) {
		return ;
	}

	if ( cur_cluster->left != NULL ) {
		this->print_cluster_tree_dfs( cur_cluster->left );
	}


	if ( cur_cluster->right != NULL ) {
		this->print_cluster_tree_dfs( cur_cluster->right );
	}

}



/* print tool: print clustering tree by BFS from cur_cluster node */
void Agglo_cluster_tree::print_cluster_tree_bfs( Cluster_node *cur_cluster ) {

	/* */
	queue< Cluster_node * > 	cluster_queue;
	Cluster_node 				*cluster_node;

	cluster_queue.push( cur_cluster );

	/** unless the queue is empty */
	while ( !cluster_queue.empty() ) {

		/* pop the cluster node from cluster_queue */
		cluster_node 			= cluster_queue.front();
		cluster_queue.pop();

		/* print the initial cluster node */
		cout<< "level is "<< cluster_node->level << ",  letters: ";
		this->print_cluster( cluster_node );
			
		if ( cluster_node->left != NULL ) {
			cluster_queue.push( cluster_node->left );
		}

		if ( cluster_node->right != NULL ) {
			cluster_queue.push( cluster_node->right );
		}
	}


}
















