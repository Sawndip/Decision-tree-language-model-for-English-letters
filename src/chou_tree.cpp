/* Author: Tianyi Chen, tchen59@jhu.edu */
#include "chou_tree.h"

/* default constructor */
Tree_node_chou::Tree_node_chou( ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );

	this->left 			= NULL;
	this->right 		= NULL;

}

/* compute total sample */
int Tree_node_chou::compute_num_sample( map< string, map< string, int > > four_gram_map, list< string > history  ) {

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
map< string, double > Tree_node_chou::compute_probs_map( map< string, map< string, int > > four_gram_map, int num_sample, list< string > history ) {

	int 											counter;
	string 											l_4;
	string 											cur_history;
	list< char >::iterator 							it_letter;
	list< string >::iterator 						it_history;
	map< string, map< string, int > >::iterator 	it_4_gram;
	map< string, int >::iterator 					it_internal;
	map< string, int > 								tmp_map;
	map< string, double > 							output_prob_map;


	for ( it_letter = this->letters.begin(); it_letter != this->letters.end(); it_letter++ ) {

		counter 		= 0.0;
		l_4 			= *it_letter;
		
		for ( it_history = history.begin(); it_history != history.end(); it_history++ ) {

			cur_history = *it_history;
			if ( four_gram_map.count( cur_history ) != 0 ) {
				tmp_map = four_gram_map.find( cur_history )->second;

				if ( tmp_map.count( l_4 ) != 0 ) { 
					counter 	+= tmp_map.find( l_4 )->second;
				}

			}

		}

		output_prob_map.insert( make_pair( l_4, (double) counter / (double) num_sample ) );

	}

	return output_prob_map;
}


/* compute Gini index given probability map */
double 	Tree_node_chou::compute_gini_index( map< string, double > probs_map ) {

	double gini_index;
	double prob_sq_sum;
	
	map< string, double >::iterator 	it_prob;

	gini_index 	= 0.0;
	prob_sq_sum	= 0.0;

	for ( it_prob = probs_map.begin(); it_prob != probs_map.end(); it_prob++ ) {
		prob_sq_sum 	+= ( it_prob->second ) * ( it_prob->second );
	}

	gini_index 	= 1.0 - prob_sq_sum;

	return 	gini_index;

}


/* find the best partition */
void  Tree_node_chou::partition_best( int question_i ) {

	string 							cur_history;
	char 							cur_beta;
	char 							cur_letter;
	char 							cur_word;
	double 							random_number;
	double 							cur_prob_A;
	double 							cur_prob_A_C;
	double 							p_w_beta;
	double 							A_sum_w;
	double 							A_C_sum_w;
	double 							cur_p_w_A;
	double 							cur_p_w_A_C;
	int 							iter;
	int 							max_iter;
	int 							status;
	int 							sum_f_beta_A;
	int 							sum_f_beta_A_C;
	int 							sum_f_w_beta_A;
	int 							sum_f_w_beta_A_C;
	int 							tmp_count;

	map< char, set< string > > 		equiv_map; 
	map< char, double > 			p_w_A;
	map< char, double > 			p_w_A_C;

	set< string > 					cur_set;
	set< char > 					A_old;
	set< char > 					A_C_old;
	set< char > 					A;
	set< char > 					A_C;
	set< char > 					A_new;
	set< char > 					A_C_new;
	set< string > 					A_history_set;
	set< string > 					A_C_history_set;
	set< string > 					beta_w_set;
	list< string >::iterator 		it_history;
	list< char >::iterator 			it_letter;
	set< string >::iterator 		it_history_A_set;
	set< char >::iterator 			it_char_set;
	map< char, set< string > >::iterator 	it_equiv;
	map< string, int > 				tmp_gram;
	map< string, int >::iterator 	it_gram;


	Tree_node_chou 					*left_child 	= new Tree_node_chou();
	Tree_node_chou 					*right_child 	= new Tree_node_chou();

	/* create equivalent map */
	for ( it_history = this->dev_history.begin(); it_history != this->dev_history.end(); it_history++ ) {
		cur_history 	= *it_history;
		cur_beta 		= cur_history[ question_i ];
		// cout << cur_history << " " << cur_beta << endl ; 
		if ( equiv_map.count( cur_beta ) == 0 ) {
			set< string > 			tmp_set;
			tmp_set.insert( cur_history );
			equiv_map.insert( make_pair( cur_beta, tmp_set ) );
		} else if ( equiv_map.count( cur_beta ) != 0 ) {
			equiv_map.find( cur_beta )->second.insert( cur_history );
		}
	}



	/* initialize A_old, A_C_old, A, A_C */
	for ( it_equiv = equiv_map.begin(); it_equiv != equiv_map.end(); it_equiv++ ) {
		// cout<< it_equiv->first << endl;
		random_number 	= ( (double) rand() / (RAND_MAX) ); 
		// cout<< random_number << endl;

		if ( random_number > 0.5 ) {
			A.insert( it_equiv->first );
			A_history_set.insert( it_equiv->second.begin(), it_equiv->second.end() );
		} else if ( random_number <= 0.5 ) {
			A_C.insert( it_equiv->first );
			A_C_history_set.insert( it_equiv->second.begin(), it_equiv->second.end() );
		}
	}


	status 			= 0;
	max_iter	 	= 1E6;
	iter 			= 0;

	/* main loop of Chou's algorithm */
	while( true ) {

		/* termination condition 1 */
		if ( A == A_old && A_C == A_C_old ) {
			status	= 0;
			// cout<< "termination 1"<<endl;
			break;
		}

		/* termination condition 2 */
		if ( iter >= max_iter ) {
			status 	= 1;
			break;
		}

		/* set A_old and A_C_old */
		A_old.clear();
		A_C_old.clear();
		A_old.insert( A.begin(), A.end() );
		A_C_old.insert( A_C.begin(), A_C.end() );

		/* Step 3: compute p(w|A) and p(w|A_C) */
		sum_f_beta_A 	= this->tree->collect_count_sum( A_history_set, this->tree->dev_4_gram );
		
		sum_f_beta_A_C 	= this->tree->collect_count_sum( A_C_history_set, this->tree->dev_4_gram );

		
		for ( it_equiv = equiv_map.begin(); it_equiv != equiv_map.end(); it_equiv++ ) {
			
			cur_word 	= it_equiv->first;

			/* calculate sum_f_w_beta_A */
			sum_f_w_beta_A 		= this->tree->collect_word_sum( A_history_set, this->tree->dev_4_gram, string() + cur_word );

			/* calculate sum_f_w_beta_A_C */
			sum_f_w_beta_A_C 	= this->tree->collect_word_sum( A_C_history_set, this->tree->dev_4_gram, string() + cur_word );

			cur_prob_A 		= (double) sum_f_w_beta_A / (double) sum_f_beta_A ;

			cur_prob_A_C 	= (double) sum_f_w_beta_A_C / (double) sum_f_beta_A_C;
			
			p_w_A.insert( make_pair( cur_word, cur_prob_A ) );
			
			p_w_A_C.insert( make_pair( cur_word, cur_prob_A_C ) );

		}

		/* Step 4: for each beta, put them into new partition */
		A_new.clear();
		A_C_new.clear();

		for ( it_equiv = equiv_map.begin(); it_equiv != equiv_map.end(); it_equiv++ ) { 
			
			cur_beta 		= it_equiv->first;
			
			beta_w_set 		= it_equiv->second;

			A_sum_w 		= 0.0;
			A_C_sum_w 		= 0.0;

			/* for each w in letters */
			for ( it_letter = this->letters.begin(); it_letter != this->letters.end(); it_letter++ ) {
				cur_word 		= *it_letter;

				/* compute p( w | beta ) */
				p_w_beta 		= 0.0;
				int tmp_deno 	= this->tree->collect_count_sum( beta_w_set, this->tree->dev_4_gram );
				int tmp_nume 	= this->tree->collect_word_sum( beta_w_set, this->tree->dev_4_gram, string() + cur_word );

				p_w_beta 			= (double) tmp_nume / (double) tmp_deno;	

				cur_p_w_A 		= p_w_A.find(cur_word)->second;
				cur_p_w_A_C 	= p_w_A_C.find(cur_word)->second;
				A_sum_w 		+= ( p_w_beta - cur_p_w_A ) * ( p_w_beta - cur_p_w_A );
				A_C_sum_w 		+= ( p_w_beta - cur_p_w_A_C ) * ( p_w_beta - cur_p_w_A_C );

			}

			/* re assign beta into new partitions */
			if ( A_sum_w <= A_C_sum_w ) {
				A_new.insert( cur_beta );
			} else {
				A_C_new.insert( cur_beta );
			}

		}

		/* update current A and A_C */
		/* set A_old and A_C_old */
		A.clear();
		A_C.clear();
		for ( it_char_set = A_new.begin(); it_char_set != A_new.end(); it_char_set++ ) {
			A.insert( *it_char_set );
			set< string > 	tmp_hist_set 	= equiv_map.find( *it_char_set )->second;
			A_history_set.insert( tmp_hist_set.begin(), tmp_hist_set.end() );
		}

		for ( it_char_set = A_C_new.begin(); it_char_set != A_C_new.end(); it_char_set++ ) {
			A_C.insert( *it_char_set );
			set< string > 	tmp_hist_set 	= equiv_map.find( *it_char_set )->second;
			A_C_history_set.insert( tmp_hist_set.begin(), tmp_hist_set.end() );
		}



		iter++;
		p_w_A.clear();
		p_w_A_C.clear();

		// cout<< "---------------"<<endl;

	}


	left_child->parent 			= this;
	left_child->tree 			= this->tree;
	left_child->left 			= NULL;
	left_child->right 			= NULL;
	left_child->is_terminal 	= false;
	left_child->path 			= left_child->parent->path + "0";
	left_child->A_set 			= A;

	right_child->parent 		= this;
	right_child->tree 			= this->tree;
	right_child->left 			= NULL;
	right_child->right 			= NULL;
	right_child->is_terminal 	= false;
	right_child->path 			= right_child->parent->path + "1";
	right_child->A_set 			= A_C;

	/* construct left_child, and right child dev_history */
	for ( it_history = this->dev_history.begin(); it_history != this->dev_history.end(); it_history++ ) { 

		cur_history 			= *it_history;
		if ( A.count( cur_history[question_i] ) ) {
			left_child->dev_history.push_back( cur_history );
		} else if( A_C.count( cur_history[question_i] ) ) {
			right_child->dev_history.push_back( cur_history );
		}

	}	

	// this->print_set( A );
	// this->print_set( A_C );
	// cout<< this->dev_history.size() << " " << left_child->dev_history.size() << " " << right_child->dev_history.size() << endl;

	/* construct left_child, and right child cross_history */
	for ( it_history = this->cross_history.begin(); it_history != this->cross_history.end(); it_history++ ) { 

		cur_history 			= *it_history;
		if ( A.count( cur_history[question_i] ) ) {
			left_child->cross_history.push_back( cur_history );
		} else if( A_C.count( cur_history[question_i] ) ) {
			right_child->cross_history.push_back( cur_history );
		}

	}	
	// cout << left_child->path << " "<< right_child->path<<endl;
	// cout<< this->cross_history.size() << " " << left_child->cross_history.size() << " " << right_child->cross_history.size() << endl;

	if ( left_child->dev_history.size() == 0 || right_child->dev_history.size() == 0 ) {
		left_child->dev_gini_index 			= INF;
		left_child->cross_gini_index 		= INF;
		right_child->dev_gini_index 		= INF;
		right_child->cross_gini_index 		= INF;
		left_child->total_dev_num_sample 	= 1;
		left_child->total_cross_num_sample 	= 1;
		right_child->total_dev_num_sample 	= 1;
		right_child->total_cross_num_sample = 1;
		left_child->dev_num_sample 			= 1;
		left_child->cross_num_sample 		= 1;
		right_child->dev_num_sample 		= 1;
		right_child->cross_num_sample 		= 1;
		left_child->num_questions 			= 3;
		right_child->num_questions 			= 3;
		this->tmp_left 						= left_child;
		this->tmp_right 					= right_child;
		return ;
	}
	left_child->dev_num_sample 		= left_child->compute_num_sample( left_child->tree->dev_4_gram, left_child->dev_history );
	left_child->cross_num_sample 	= left_child->compute_num_sample( left_child->tree->cross_4_gram, left_child->cross_history );

	right_child->dev_num_sample 	= right_child->compute_num_sample( left_child->tree->dev_4_gram, right_child->dev_history );
	right_child->cross_num_sample 	= right_child->compute_num_sample( left_child->tree->cross_4_gram, right_child->cross_history );

	left_child->total_dev_num_sample 	= left_child->tree->root->total_dev_num_sample;
	right_child->total_dev_num_sample 	= right_child->tree->root->total_dev_num_sample;
	left_child->total_cross_num_sample 	= left_child->tree->root->total_cross_num_sample;
	right_child->total_cross_num_sample = right_child->tree->root->total_cross_num_sample;

	left_child->dev_probs_map 		= left_child->compute_probs_map( left_child->tree->dev_4_gram, left_child->dev_num_sample, left_child->dev_history );
	right_child->dev_probs_map 		= right_child->compute_probs_map( right_child->tree->dev_4_gram, right_child->dev_num_sample, right_child->dev_history );

	left_child->cross_probs_map 	= left_child->compute_probs_map( left_child->tree->cross_4_gram, left_child->cross_num_sample, left_child->cross_history );
	right_child->cross_probs_map 	= right_child->compute_probs_map( right_child->tree->cross_4_gram, right_child->cross_num_sample, right_child->cross_history );

	
	left_child->dev_gini_index 		= left_child->compute_gini_index( left_child->dev_probs_map );
	right_child->dev_gini_index		= right_child->compute_gini_index( right_child->dev_probs_map );

	left_child->cross_gini_index 	= left_child->compute_gini_index( left_child->cross_probs_map );
	right_child->cross_gini_index 	= right_child->compute_gini_index( right_child->cross_probs_map ); 

	left_child->num_questions 		= 3;
	right_child->num_questions 		= 3;

	// cout<< left_child->dev_gini_index << " " << right_child->dev_gini_index	<< endl;
	// cout<< left_child->cross_gini_index << " "  << right_child->cross_gini_index << endl;

	this->tmp_left 					= left_child;
	this->tmp_right 				= right_child;


}


/* print tool */
void Tree_node_chou::print_set( set< char > char_set ){

	set< char >::iterator 	it_set;
	for ( it_set = char_set.begin(); it_set != char_set.end(); it_set++ ) {
		cout<< *it_set << " ";
	}
	cout<<endl;
}

/* print tool */
void Tree_node_chou::print_set_history( set< string > string_set ){

	set< string >::iterator 	it_set;
	for ( it_set = string_set.begin(); it_set != string_set.end(); it_set++ ) {
		cout<< *it_set << "  ";
	}
	cout<<endl;
}

/* Default constructor */
Chou_tree::Chou_tree( ) {

	/* initialize all possible letters */
	int 	i;
	for ( i = 0; i < 26; i++ ) {
		this->letters.push_back( char( 'a' + i ) );
	}
	this->letters.push_back( ' ' );


}

Chou_tree::Chou_tree( Agglo_cluster_tree *allgo_tree ) {

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

/* collect development, cross-validation, test data given development percentage */
void Chou_tree::collect_dev_cross_test( double dev_perc ) {

	int 	i;
	string 	history;
	string 	history_code;
	string 	next_letter;

	/* */
	this->dev_size 		= int( this->train_size * dev_perc );
	this->cross_size 	= this->train_size - this->dev_size;
	

	/* construct development set */
	for ( i = 0; i < this->dev_size; i++ ) {
		
		if ( i == this->dev_size - 3 ) {
			break;
		}

		history 		= string() + this->train_data[i] + this->train_data[i+1] + this->train_data[i+2];
		next_letter 	= string() + this->train_data[i+3];

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


		history 		= string() + this->train_data[i] + this->train_data[i+1] + this->train_data[i+2];
		next_letter 	= string() + this->train_data[i+3];

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

		history 		= string() + this->test_data[i] + this->test_data[i+1] + this->test_data[i+2];
		next_letter 	= string() + this->test_data[i+3];

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

/* construct tree */
void Chou_tree::grow_tree( ) {

	/* set random seed */
	srand((unsigned)time(NULL));

	int 								i;
	int 								best_i;
	int 								num_frontiers;
	Tree_node_chou 						*cur_tree_node;
	Tree_node_chou 						*best_left_node;
	Tree_node_chou 						*best_right_node;
	list< Tree_node_chou * > 			new_node_list;
	list< Tree_node_chou * >::iterator 	it_tree_node;
	
	/* greateast reduction in entropy */
	double 								greatest_dev_reduct;
	double 								current_reduction;
	double 								candidate_reduction;


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

			greatest_dev_reduct 	= MINIMUM_VALUE;
			cur_tree_node			= *it_tree_node;
			best_i 					= -1;

			/* need to check the order correctly */
			for ( i = 0; i < cur_tree_node->num_questions; i++ ) {

				cur_tree_node->partition_best( i );

				// cout<< cur_tree_node->dev_gini_index << " " << cur_tree_node->tmp_left->dev_gini_index << " " << cur_tree_node->tmp_right->dev_gini_index << endl;

				current_reduction	= (double) cur_tree_node->dev_num_sample / (double) cur_tree_node->total_dev_num_sample * cur_tree_node->dev_gini_index - 
										( (double) cur_tree_node->tmp_left->dev_num_sample / (double) cur_tree_node->tmp_left->total_dev_num_sample * cur_tree_node->tmp_left->dev_gini_index
										+ (double) cur_tree_node->tmp_right->dev_num_sample / (double) cur_tree_node->tmp_right->total_dev_num_sample * cur_tree_node->tmp_right->dev_gini_index );

				if ( current_reduction > greatest_dev_reduct ) {
					greatest_dev_reduct 	= current_reduction;
					best_right_node 		= cur_tree_node->tmp_right;
					best_left_node 			= cur_tree_node->tmp_left;
					best_i 					= i;
					cur_tree_node->questioned_bit 	= best_i;
				}
			}

			/* set candidate left and right node for current tree node for permananent split */
			cur_tree_node->candidate_left 	= best_left_node;
			cur_tree_node->candidate_right 	= best_left_node;

			/* check candidate left and right on cross-validation tree */
			candidate_reduction 		= (double) cur_tree_node->cross_num_sample / (double) cur_tree_node->total_cross_num_sample * cur_tree_node->cross_gini_index -
										( (double) cur_tree_node->candidate_left->cross_num_sample / (double) cur_tree_node->candidate_left->total_cross_num_sample * cur_tree_node->candidate_left->cross_gini_index
										+ (double) cur_tree_node->candidate_right->cross_num_sample / (double) cur_tree_node->candidate_right->total_cross_num_sample * cur_tree_node->candidate_right->cross_gini_index );

			// cout<< "cross reduction: "<< candidate_reduction << endl;

			if ( candidate_reduction >= 0.005 && cur_tree_node->candidate_left->dev_num_sample != cur_tree_node->dev_num_sample && cur_tree_node->candidate_right->dev_num_sample != cur_tree_node->dev_num_sample ) {
				cur_tree_node->left 		= cur_tree_node->candidate_left;
				cur_tree_node->right 		= cur_tree_node->candidate_right;
				new_node_list.push_back( cur_tree_node->left );
				new_node_list.push_back( cur_tree_node->right );
				cur_tree_node->is_terminal 	= true;
			} else {
				cur_tree_node->is_terminal 	= true;
			} 

		}	

		// cout<< "frontier_node_list size: "<< this->frontier_node_list.size() << endl;

		/* update frontier node list */
		/* at first, remove all node in list that is terminated. */
		num_frontiers 			= this->frontier_node_list.size();
		for ( i = 0, it_tree_node = frontier_node_list.begin(); i < num_frontiers; i++ ) {
			cur_tree_node 		= *it_tree_node;
			if ( cur_tree_node->is_terminal == true ) {
				it_tree_node 	= this->frontier_node_list.erase( it_tree_node );
			}
		}

		// cout<< "frontier_node_list size: "<< this->frontier_node_list.size() << endl;

		/* secondly, add new tree nodes */
		for ( it_tree_node = new_node_list.begin(); it_tree_node != new_node_list.end(); it_tree_node++ ) {
			this->frontier_node_list.push_back( *it_tree_node );
		}

		// cout<< "frontier_node_list size: "<<this->frontier_node_list.size() << endl;
		new_node_list.clear();		
		
	} /* end of while( true ) */
}

/* construct root */
void Chou_tree::set_root( ) {

	this->root 					= new Tree_node_chou();
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

	this->root->dev_probs_map 				= this->root->compute_probs_map( this->dev_4_gram, this->root->dev_num_sample, this->root->dev_history );
	this->root->cross_probs_map 			= this->root->compute_probs_map( this->cross_4_gram, this->root->cross_num_sample, this->root->cross_history );

	this->root->dev_gini_index 				= this->root->compute_gini_index( this->root->dev_probs_map );
	this->root->cross_gini_index			= this->root->compute_gini_index( this->root->cross_probs_map );

	this->root->num_questions 				= 3;

	this->root->path 						= "";

}

/* collect total sample given history set */
int Chou_tree::collect_count_sum( set< string > history_set, map< string, map< string, int > > gram ) {

	int 						result;
	string 						cur_history;
	map< string, int > 			cur_gram;
	set< string >::iterator 	it_history;
	map< string, int >::iterator 	it_gram;


	result 						= 0;

	for ( it_history = history_set.begin(); it_history != history_set.end(); it_history++ ) {
		cur_history 			= *it_history;
		if ( gram.count(cur_history) != 0 ) {
			cur_gram 			= gram.find( cur_history )->second;
			for ( it_gram = cur_gram.begin(); it_gram != cur_gram.end(); it_gram++ ) {
				result 	+= it_gram->second;
			}
		}
	}


	return result;
}


int Chou_tree::collect_word_sum( set< string > history_set, map< string, map< string, int > > gram, string word ) {

	int 						result;
	string 						cur_history;
	map< string, int > 			cur_gram;
	set< string >::iterator 	it_history;
	map< string, int >::iterator 	it_gram;

	result 						= 0;

	for ( it_history = history_set.begin(); it_history != history_set.end(); it_history++ ) {
		cur_history 			= *it_history;
		if ( gram.count(cur_history) != 0 ) {
			cur_gram 			= gram.find( cur_history )->second;
			if ( cur_gram.count( word ) != 0 ) {
				result 			+= cur_gram.find(word)->second;
			}
		}
	}

	return result;
}


/* compute the perplexity of the test data */
double Chou_tree::compute_perplex( ) {

	int 										i;
	double 										cur_prob;
	double 										log2_prob_sum;
	double 										perplexity;
	double 										random_number;
	char 										target_bit;
	string 										cur_history;
	string 										cur_next_letter;
	map< string, int > 							cur_gram;
	map< string, double > 						cur_distribution;
	Tree_node_chou 								*cur_tree_node;
	map< string, map< string, int > >::iterator it_test;
	map< string, int >::iterator 				it_gram;
	map< string, double >::iterator 			it_prob;

	log2_prob_sum 	= 0.0;

	for ( it_test = this->test_4_gram.begin(); it_test != this->test_4_gram.end(); it_test++ ) {
		
		cur_history 	= it_test->first;
		cur_gram 		= it_test->second;

		cur_tree_node 	= this->root;

		/* find corresponding leaf */
		while( true ) {
			
			/* if there is no further child to search, stop */
			if ( cur_tree_node->left == NULL || cur_tree_node->right == NULL ) {
				break;
			}

			target_bit 	= cur_history[ cur_tree_node->questioned_bit ];
			// cout<< "target_bit: "<< target_bit << " " << cur_tree_node->questioned_bit << endl;

			if ( cur_tree_node->left->A_set.count(target_bit) != 0 ) {
				cur_tree_node 	= cur_tree_node->left;
			} else if ( cur_tree_node->right->A_set.count(target_bit) != 0 ) {
				cur_tree_node 	= cur_tree_node->right;
			} else {
				random_number 	= ( (double) rand() / (RAND_MAX) ); 

				if ( random_number > 0.5 ) {
					cur_tree_node 	= cur_tree_node->left;
				} else {
					cur_tree_node 	= cur_tree_node->right;
				}
			}
		}

		cur_distribution 	= cur_tree_node->dev_probs_map;

		for ( it_gram = cur_gram.begin(); it_gram != cur_gram.end(); it_gram++ ) {

			cur_next_letter 	= it_gram->first;

			cur_prob 			= cur_distribution.find( cur_next_letter )->second;

			for ( i = 0; i < it_gram->second; i++ ) {
				if ( cur_prob > 0.0 ) {
					log2_prob_sum 		+= log2( cur_prob );
				}
			}

		}
	}

	perplexity 	= pow( 2.0, -1.0 / (double) ( this->test_size - 3 ) * log2_prob_sum );

	return perplexity;


}




