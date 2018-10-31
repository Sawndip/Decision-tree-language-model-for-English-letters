/* Author: Tianyi Chen, tchen59@jhu.edu */
#include "Bit_encode_decision_tree.h"

/* default constructor */
Tree_node_bit::Tree_node_bit( ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );

	this->left 			= NULL;
	this->right 		= NULL;

}


/* constructor given bit_decision_tree */
Tree_node_bit::Tree_node_bit( Bit_encode_dec_tree *bit_tree ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );

	this->left 			= NULL;
	this->right 		= NULL;
	this->tree 			= bit_tree;

}


/* compute total sample */
int Tree_node_bit::compute_num_sample( map< string, map< string, int > > four_gram_map, list< string > history  ) {

	int 				num_sample;
	string 				cur_history;
	map< string, int > 	iternal_map;
	map< string, map< string, int > >::iterator 	it_4_gram;
	map< string, int >::iterator 					it_internal;
	list< string >::iterator 						it_history;

	num_sample 	= 0;

	for ( it_history = history.begin(); it_history != history.end(); it_history++ ) {
		cur_history 		= *it_history;

		if ( four_gram_map.count( cur_history ) != 0 ) {
			iternal_map 	= four_gram_map.find( cur_history )->second;
			for ( it_internal = iternal_map.begin(); it_internal != iternal_map.end(); it_internal++ ) { 
				num_sample 	+= it_internal->second;
			}
		}
	}

	return num_sample;

}


/* calculate probs map */
map< string, double > Tree_node_bit::compute_probs_map( map< string, map< string, int > > four_gram_map, int num_sample, list< string > history ) {

	int 											counter;
	string 											code_l_4;
	string 											cur_history;
	list< char >::iterator 							it_letter;
	list< string >::iterator 						it_history;
	map< string, map< string, int > >::iterator 	it_4_gram;
	map< string, int >::iterator 					it_internal;
	map< string, int > 								tmp_map;
	map< string, double > 							output_prob_map;


	for ( it_letter = this->letters.begin(); it_letter != this->letters.end(); it_letter++ ) {

		counter 		= 0.0;
		code_l_4 		= this->tree->coding_book_letter.find( *it_letter )->second;
		
		for ( it_history = history.begin(); it_history != history.end(); it_history++ ) {

			cur_history = *it_history;
			if ( four_gram_map.count( cur_history ) != 0 ) {
				tmp_map = four_gram_map.find( cur_history )->second;

				if ( tmp_map.count( code_l_4 ) != 0 ) { 
					counter 	+= tmp_map.find( code_l_4 )->second;
				}

			}

		}

		output_prob_map.insert( make_pair( code_l_4, (double) counter / (double) num_sample ) );

	}

	return output_prob_map;

}



/* calculate dev_entropy */
double Tree_node_bit::compute_entropy( map< string, double > probs_map ) {

	double 			entropy;
	double 			cur_prob;
	map< string, double >::iterator 	it_prob;
	
	entropy 			= 0.0;

	for ( it_prob = probs_map.begin(); it_prob != probs_map.end(); it_prob++ ) {
		cur_prob 	= it_prob->second;
		if ( cur_prob > 0.0 ) {
			entropy 	+= - cur_prob * log2( cur_prob );
		}
	}	

	return entropy;

}


/* split current treenode into two leaves given questioned bit */
void Tree_node_bit::partition( int question_bit ) {

	char 			target_bit;
	string 			cur_history;
	Tree_node_bit 	*left_child 	= new Tree_node_bit();
	Tree_node_bit 	*right_child 	= new Tree_node_bit();

	list< string >::iterator 		it_history;

	/* split on dev */
	for ( it_history = this->dev_history.begin(); it_history != this->dev_history.end(); it_history++ ) {

		cur_history 	= *it_history;
		target_bit 		= cur_history[question_bit];

		if ( target_bit == '0' ) {
		 	left_child->dev_history.push_back( cur_history );
		} else if ( target_bit == '1' ) {
		 	right_child->dev_history.push_back( cur_history );
		}
	}

	/* split on cross-validation dataset */
	for ( it_history = this->cross_history.begin(); it_history != this->cross_history.end(); it_history++ ) {

		cur_history 	= *it_history;
		target_bit 		= cur_history[question_bit];
 
		if ( target_bit == '0' ) {
			left_child->cross_history.push_back( cur_history );
		} else if ( target_bit == '1' ) {
		 	right_child->cross_history.push_back( cur_history );
		}

	}


	/* set left child and right child */
	left_child->parent 				= this;
	right_child->parent 			= this;
	left_child->tree 				= this->tree;
	right_child->tree 				= this->tree;

	left_child->path 				= left_child->parent->path + "0";
	right_child->path 				= right_child->parent->path + "1";

	left_child->dev_num_sample 		= left_child->compute_num_sample( left_child->tree->dev_4_gram, left_child->dev_history );
	right_child->dev_num_sample 	= right_child->compute_num_sample( right_child->tree->dev_4_gram, right_child->dev_history );

	left_child->total_dev_num_sample	= left_child->tree->root->total_dev_num_sample;
	right_child->total_dev_num_sample 	= right_child->tree->root->total_dev_num_sample;

	left_child->cross_num_sample 	= left_child->compute_num_sample( left_child->tree->cross_4_gram, left_child->cross_history );
	right_child->cross_num_sample 	= right_child->compute_num_sample( right_child->tree->cross_4_gram, right_child->cross_history );

	left_child->total_cross_num_sample	= left_child->tree->root->total_cross_num_sample;
	right_child->total_cross_num_sample = right_child->tree->root->total_cross_num_sample;

	left_child->dev_probs_map 		= left_child->compute_probs_map( left_child->tree->dev_4_gram, left_child->dev_num_sample, left_child->dev_history );
	right_child->dev_probs_map 		= right_child->compute_probs_map( right_child->tree->dev_4_gram, right_child->dev_num_sample, right_child->dev_history );

	left_child->cross_probs_map 	= left_child->compute_probs_map( left_child->tree->cross_4_gram, left_child->cross_num_sample, left_child->cross_history );
	right_child->cross_probs_map 	= right_child->compute_probs_map( right_child->tree->cross_4_gram, right_child->cross_num_sample, right_child->cross_history );

	left_child->dev_entropy 		= left_child->compute_entropy( left_child->dev_probs_map );
	right_child->dev_entropy 		= right_child->compute_entropy( right_child->dev_probs_map );

	left_child->cross_entropy 		= left_child->compute_entropy( left_child->cross_probs_map );
	right_child->cross_entropy 		= right_child->compute_entropy( right_child->cross_probs_map );


	left_child->num_questions 		= 3;
	right_child->num_questions 		= 3;

	left_child->is_terminal 		= false;
	right_child->is_terminal 		= false;

	left_child->permitted_bits_idx 	= Malloc( int, left_child->num_questions );
	right_child->permitted_bits_idx = Malloc( int, right_child->num_questions );

	int i;
	for ( i = 0; i < left_child->num_questions; i++ ) {
		left_child->permitted_bits_idx[i] 	= this->permitted_bits_idx[i];
		right_child->permitted_bits_idx[i]	= this->permitted_bits_idx[i];
	}

	// cout<< "left dev entropy: " << left_child->dev_entropy << ",  left dev sample : "<<left_child->dev_num_sample << endl;
	// cout<< "right dev entropy: " <<right_child->dev_entropy<< ",  right dev sample: "<<right_child->dev_num_sample << endl;
	// cout<< "left cross entropy: " << left_child->cross_entropy << endl;
	// cout<< "right cross entropy: " <<right_child->cross_entropy << endl;

	this->tmp_left 					= left_child;
	this->tmp_right 				= right_child;

}


/* default constructor */
Bit_encode_dec_tree::Bit_encode_dec_tree ( ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );

}


/* constructor given Agglo_cluster_tree */
Bit_encode_dec_tree::Bit_encode_dec_tree ( Agglo_cluster_tree *allgo_tree ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );

	this->train_data 			= allgo_tree->train_data;
	this->test_data 			= allgo_tree->test_data;
	this->train_size 			= allgo_tree->train_size;
	this->test_size 			= allgo_tree->test_size;
	this->K 					= allgo_tree->K;

}


/* encode all letter l \in \mathcal{L} */
void Bit_encode_dec_tree::encoding( Cluster_node * node ) {

	/* base case */
	if ( node == NULL ) {
		return ;
	}

	/* if the left leave of node is not null */
	if ( node->left != NULL ) {
		node->left->codeword 	= node->codeword + "0";
		this->encoding( node->left );
	}
	
	/* if the right leave of node is not null */
	if ( node->right != NULL ) {
		node->right->codeword 	= node->codeword + "1";
		this->encoding( node->right );
	}

	if ( node->letters.size() == 1 ) {

		int i;

		/* padding 0 until codework size is K */
		int left_k 	= K - node->codeword.size();

		for ( i = 0; i < left_k; i++ ) {
			node->codeword 	+= "0";
		}

		this->coding_book_letter.insert( make_pair( node->letters.front(), node->codeword ) );
		// cout<< node->letters.front()<< " : "<<node->codeword << endl;
	}


}

/* collect development, cross-validation, test data given development percentage */
void Bit_encode_dec_tree::collect_dev_cross_test( double dev_perc ) {

	int 	i;
	string 	history;
	string 	history_code;
	string 	next_letter;

	/* */
	this->dev_size 		= int( this->train_size * dev_perc );
	this->cross_size 	= this->train_size - this->dev_size;
	
	// cout<< this->train_size<< " " << this->dev_size<< " " << this->cross_size << " " << this->test_size <<endl;

	/* construct development set */
	for ( i = 0; i < this->dev_size; i++ ) {
		
		if ( i == this->dev_size - 3 ) {
			break;
		}

		/* encode into 3K bits */
		history 		= this->coding_book_letter.find( this->train_data[i] )->second  
						+ this->coding_book_letter.find( this->train_data[i+1] )->second
						+ this->coding_book_letter.find( this->train_data[i+2] )->second;
		next_letter 	= this->coding_book_letter.find( this->train_data[i+3] )->second;

		// history 		= string() + this->train_data[i] + this->train_data[i+1] + this->train_data[i+2];
		// next_letter 	= string() + this->train_data[i+3];

		// cout<< "history: "<< history<<", cur: "<< next_letter<<endl;
		// getchar();
		if ( this->dev_4_gram.count( history ) == 0 ) {
			map< string, int > 	cur_4_gram;
			cur_4_gram.insert( make_pair( next_letter, 1 ) );
			this->dev_4_gram.insert( make_pair( history, cur_4_gram ) );
		} else {
			map< string, int > 	old_4_gram 	= this->dev_4_gram.find( history )->second;
			if ( old_4_gram.count( next_letter ) == 0 ) {
				this->dev_4_gram.find( history )->second.insert( make_pair( next_letter, 1 ) );
			} else {
				this->dev_4_gram.find( history )->second.find( next_letter )->second	+= 1;
			}
		}

	}


	/* construct cross-validation set */
	for ( i = this->dev_size; i < this->train_size; i++ ) {

		if ( i == this->train_size - 3 ) {
			break;
		}

		/* encode into 3K bits */
		history 		= this->coding_book_letter.find( this->train_data[i] )->second  
						+ this->coding_book_letter.find( this->train_data[i+1] )->second
						+ this->coding_book_letter.find( this->train_data[i+2] )->second;
		next_letter 	= this->coding_book_letter.find( this->train_data[i+3] )->second;

		// history 		= string() + this->train_data[i] + this->train_data[i+1] + this->train_data[i+2];
		// next_letter 	= string() + this->train_data[i+3];

		if ( this->cross_4_gram.count( history ) == 0 ) {
			map< string, int > 	cur_4_gram;
			cur_4_gram.insert( make_pair( next_letter, 1 ) );
			this->cross_4_gram.insert( make_pair( history, cur_4_gram ) );
		} else {
			map< string, int > 	old_4_gram 	= this->cross_4_gram.find( history )->second;
			if ( old_4_gram.count( next_letter ) == 0 ) {
				this->cross_4_gram.find( history )->second.insert( make_pair( next_letter, 1 ) );
			} else {
				this->cross_4_gram.find( history )->second.find( next_letter )->second	+= 1;
			}
		}

	}


	/* construct test set */
	for ( i = 0; i < this->test_size; i++ ) {

		if ( i == this->test_size - 3 ) {
			break;
		}

		/* encode into 3K bits */
		history 		= this->coding_book_letter.find( this->test_data[i] )->second  
						+ this->coding_book_letter.find( this->test_data[i+1] )->second
						+ this->coding_book_letter.find( this->test_data[i+2] )->second;
		next_letter 	= this->coding_book_letter.find( this->test_data[i+3] )->second;

		if ( this->test_4_gram.count( history ) == 0 ) {
			map< string, int > 	cur_4_gram;
			cur_4_gram.insert( make_pair( next_letter, 1 ) );
			this->test_4_gram.insert( make_pair( history, cur_4_gram ) );
		} else {
			map< string, int > 	old_4_gram 	= this->test_4_gram.find( history )->second;
			if ( old_4_gram.count( next_letter ) == 0 ) {
				this->test_4_gram.find( history )->second.insert( make_pair( next_letter, 1 ) );
			} else {
				this->test_4_gram.find( history )->second.find( next_letter )->second	+= 1;
			}
		}

	}


}


/* tree grow */
void Bit_encode_dec_tree::grow_tree( ) {

	int 					i;
	int 					num_frontiers;
	int 					best_i;
	int 					best_bit;

	/* greateast reduction in entropy */
	double 					greatest_dev_reduct;
	double 					current_reduction;
	double 					candidate_reduction;

	Tree_node_bit 			*best_left_node;
	Tree_node_bit 			*best_right_node;
	Tree_node_bit 			*cur_tree_node;
	list< Tree_node_bit * > new_node_list;
	list< Tree_node_bit * >::iterator 	it_tree_node;

	// cout<< "root entropy:"<< this->root->dev_entropy << endl;
	// cout<< "dev sample: "<< this->root->dev_num_sample << endl;
	// cout<< "total_dev_num_sample: "<< this->root->total_dev_num_sample << endl;
	// cout<< "root cross entropy:"<< this->root->cross_entropy << endl;
	// getchar();

	/* construct root at first */
	this->set_root();

	/* add root into frontier_node_list at first */
	this->frontier_node_list.push_back( this->root );

	/* main loop for growing tree */
	while( true ) {

		/* until all frontier nodes become ternimal */
		if ( this->frontier_node_list.size() == 0 ) {
			fprintf(stderr, "\n" );
			cout<< "No frontier node..."<< endl;
			cout<< "Bit tree growing complete..."<<endl;
			break;
		}


		/* */
		for ( it_tree_node = this->frontier_node_list.begin(); it_tree_node != this->frontier_node_list.end(); it_tree_node++ ) {
			
			/* display . for process */
			fprintf(stderr, "." );

			greatest_dev_reduct	 	= MINIMUM_VALUE;

			cur_tree_node 			= *it_tree_node;

			/* try 3 questions to select the best one */
			for ( i = 0; i < cur_tree_node->num_questions; i++ ) {

				
				/* check here later */
				if ( cur_tree_node->permitted_bits_idx[i] >= i * this->K + this->K ) {

					continue;
				}

				/* partition current tree node */
				cur_tree_node->partition( cur_tree_node->permitted_bits_idx[i] );

				current_reduction 	= (double) cur_tree_node->dev_num_sample / (double) cur_tree_node->total_dev_num_sample * cur_tree_node->dev_entropy - 
										( (double) cur_tree_node->tmp_left->dev_num_sample / (double) cur_tree_node->tmp_left->total_dev_num_sample * cur_tree_node->tmp_left->dev_entropy
										+ (double) cur_tree_node->tmp_right->dev_num_sample / (double) cur_tree_node->tmp_right->total_dev_num_sample * cur_tree_node->tmp_right->dev_entropy );

				if ( current_reduction > greatest_dev_reduct ) {
					greatest_dev_reduct = current_reduction;
					best_left_node 		= cur_tree_node->tmp_left;
					best_right_node 	= cur_tree_node->tmp_right;
					best_i 				= i;
					cur_tree_node->questioned_bit 	= cur_tree_node->permitted_bits_idx[i];
				}

			}

			/* set candidate left and right node for current tree node for permananent split */
			cur_tree_node->candidate_left 	= best_left_node;
			cur_tree_node->candidate_right	= best_right_node;

			/* check candidate left and right on cross-validation tree */
			candidate_reduction 		= (double) cur_tree_node->cross_num_sample / (double) cur_tree_node->total_cross_num_sample * cur_tree_node->cross_entropy -
										( (double) cur_tree_node->candidate_left->cross_num_sample / (double) cur_tree_node->candidate_left->total_cross_num_sample * cur_tree_node->candidate_left->cross_entropy
										+ (double) cur_tree_node->candidate_right->cross_num_sample / (double) cur_tree_node->candidate_right->total_cross_num_sample * cur_tree_node->candidate_right->cross_entropy );
			// cout<< "candidate reduction: " << candidate_reduction << endl;
			/* If reduction is greater than 0.005 bits, make the split permanent. Let the frontier grow. */
			if ( candidate_reduction >= 0.005 ) {
				
				cur_tree_node->left 	= cur_tree_node->candidate_left;
				cur_tree_node->right 	= cur_tree_node->candidate_right;
				new_node_list.push_back( cur_tree_node->left );
				new_node_list.push_back( cur_tree_node->right );
				cur_tree_node->is_terminal 	= true;
				
				/* has issue here */
				cur_tree_node->left->permitted_bits_idx[best_i]++;
				cur_tree_node->right->permitted_bits_idx[best_i]++;


				// cout<< "new_node_list add new two nodes" << endl;
			} 
			/* otherwise, denote this node as terminal */
			else {
				cur_tree_node->is_terminal 	= true;
			}

			// cout<< "cur_questioned_bit:"<< cur_tree_node->questioned_bit<<endl;
			// getchar();

		} 

		/* update frontier node list */
		/* at first, remove all node in list that is terminated. */
		num_frontiers 			= this->frontier_node_list.size();
		for ( i = 0, it_tree_node = this->frontier_node_list.begin(); i < num_frontiers; i++ ) {
			cur_tree_node 		= *it_tree_node;
			if ( cur_tree_node->is_terminal == true ) {
				it_tree_node 	= this->frontier_node_list.erase( it_tree_node );
			}
		}

		// cout<< "frontier_node_list size: "<< this->frontier_node_list.size()<<endl;
		/* secondly, add new tree nodes */
		for ( it_tree_node = new_node_list.begin(); it_tree_node != new_node_list.end(); it_tree_node++ ) {
			this->frontier_node_list.push_back( *it_tree_node );
		}
		new_node_list.clear();
		// cout<< "frontier_node_list size: "<< this->frontier_node_list.size()<<endl;
		// cout<< "new_node_list size: "<< new_node_list.size()<<endl;
		// getchar();
	
	} /* end of while( true ) */

}


/* construct root */
void Bit_encode_dec_tree::set_root( ) { 

	this->root 					= new Tree_node_bit();
	this->root->tree 			= this;
	this->root->left 			= NULL;
	this->root->right 			= NULL;
	this->root->is_terminal 	= false;

	map< string, map< string, int > >::iterator 	it_dev_4_gram;
	for ( it_dev_4_gram = this->dev_4_gram.begin(); it_dev_4_gram != this->dev_4_gram.end(); it_dev_4_gram++ ) {
		this->root->dev_history.push_back( it_dev_4_gram->first );
	}

	map< string, map< string, int > >::iterator 	it_cross_4_gram;
	for ( it_cross_4_gram = this->cross_4_gram.begin(); it_cross_4_gram != this->cross_4_gram.end(); it_cross_4_gram++ ) {
		this->root->cross_history.push_back( it_cross_4_gram->first );
	}


	this->root->total_dev_num_sample 		= this->root->compute_num_sample( this->dev_4_gram, this->root->dev_history );
	this->root->total_cross_num_sample		= this->root->compute_num_sample( this->cross_4_gram, this->root->cross_history );

	this->root->dev_num_sample 				= this->root->total_dev_num_sample;
	this->root->cross_num_sample 			= this->root->total_cross_num_sample;

	this->root->dev_probs_map 	= this->root->compute_probs_map( this->dev_4_gram, this->root->dev_num_sample, this->root->dev_history );
	this->root->cross_probs_map = this->root->compute_probs_map( this->cross_4_gram, this->root->cross_num_sample, this->root->cross_history );

	this->root->dev_entropy 	= this->root->compute_entropy( this->root->dev_probs_map );
	this->root->cross_entropy 	= this->root->compute_entropy( this->root->cross_probs_map );

	this->root->num_questions 	= 3;

	this->root->permitted_bits_idx 			= Malloc( int, this->root->num_questions );
	this->root->permitted_bits_idx[0]		= 0;
	this->root->permitted_bits_idx[1]		= this->root->permitted_bits_idx[0] + this->K;
	this->root->permitted_bits_idx[2]		= this->root->permitted_bits_idx[1] + this->K;

	this->root->path 			= "";
	this->root->parent 			= NULL;

}

/* compute the perplexity of the test data */
double Bit_encode_dec_tree::compute_perplex( ) {

	int 										i;
	double 										cur_prob;
	double 										log2_prob_sum;
	double 										perplexity;
	char 										target_bit;
	string 										cur_history;
	string 										cur_next_letter;
	map< string, int > 							cur_gram;
	map< string, double > 						cur_distribution;
	Tree_node_bit 								*cur_tree_node;
	map< string, map< string, int > >::iterator it_test;
	map< string, int >::iterator 				it_gram;
	map< string, double >::iterator 			it_prob;

	log2_prob_sum 	= 0.0;
	
	for ( it_test = this->test_4_gram.begin(); it_test != this->test_4_gram.end(); it_test++ ) {
		
		cur_history 	= it_test->first;
		cur_gram 		= it_test->second;

		// cout<< cur_history<< endl;
		
		cur_tree_node 	= this->root;
		
		// cout<< "question_bit: "<< cur_tree_node->questioned_bit<<endl;
		// cout<< "cur_history: "<<cur_history<<endl;

		/* find corresponding leaf */
		while( true ) {

			// cout<< "line 577: cur_tree_node, questioned_bit: "<< cur_tree_node->questioned_bit << endl;

			/* if there is no further child to search, stop */
			if ( cur_tree_node->left == NULL ) {
				break;
			}

			target_bit 	= cur_history[ cur_tree_node->questioned_bit ];

			if ( target_bit == '0' ) {
				cur_tree_node 	= cur_tree_node->left;
			} else if ( target_bit == '1' ) {
				cur_tree_node 	= cur_tree_node->right;
			}
				
		}

		cur_distribution 	= cur_tree_node->dev_probs_map;

		for ( it_gram = cur_gram.begin(); it_gram != cur_gram.end(); it_gram++ ) {

			cur_next_letter 	= it_gram->first;

			cur_prob 			= cur_distribution.find( cur_next_letter )->second;

			for ( i = 0; i < it_gram->second; i++ ) {
				if ( cur_prob > 0.0 ) {b
					log2_prob_sum 		+= log2( cur_prob );
				}
			}

		}
		

	}

	perplexity 	= pow( 2.0, -1.0 / (double) ( this->test_size - 3 ) * log2_prob_sum );

	return perplexity;
}

/* print tools: print 4-gram map */
void Bit_encode_dec_tree::print_4_gram( map< string, int > four_gram ) {

	map< string, int >::iterator 	it_4_gram;

	for ( it_4_gram = four_gram.begin(); it_4_gram != four_gram.end(); it_4_gram++ ) {

		cout<< it_4_gram->first << " , " << it_4_gram->second << endl;

	}

}


/* print tool: print 4-grams map */
void Bit_encode_dec_tree::print_4_grams( map< string, map< string, int > > four_grams ) {

	map< string, map< string, int > >::iterator 	it_4_gram;

	for ( it_4_gram = four_grams.begin(); it_4_gram != four_grams.end(); it_4_gram++ ) {

		cout << "================================" << endl;
		cout << "Current history: " << it_4_gram->first << endl;
		cout << "--------------------------------" << endl;

		this->print_4_gram( it_4_gram->second );

	}
}






/* print tool: print bit encode tree by BFS from root node */
void Bit_encode_dec_tree::print_bit_tree_bfs( Tree_node_bit *cur_node ) {

	/* */
	queue< Tree_node_bit * > 	tree_node_queue;
	Tree_node_bit 				*tree_node;

	tree_node_queue.push( cur_node );

	/** unless the queue is empty */
	while ( !tree_node_queue.empty() ) {

		/* pop the tree node from tree_node_queue */
		tree_node 			= tree_node_queue.front();
		tree_node_queue.pop();

		/* print the initial cluster node */
		cout<< "path: " << tree_node->path << ", questioned bit "<< tree_node->questioned_bit << endl;
			
		if ( tree_node->left != NULL ) {
			tree_node_queue.push( tree_node->left );
		}

		if ( tree_node->right != NULL ) {
			tree_node_queue.push( tree_node->right );
		}
	}

}

















