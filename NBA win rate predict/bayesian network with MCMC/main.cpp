#include<iostream>
#include<random>
#include<cmath>


/******************************************************** 
opponent_power      A;      //1=powerful oppo	
home_or_away        B;	    //1=home
eFG                 C;      //1=bigger
TOV                 D;		//1=bigger
ORB                 E;		//1=bigger
FT                  F;		//1=bigger 
win                 G;      //1=win 
********************************************************/
const int A = 0;
const int B = 1;
const int C = 2;
const int D = 3;
const int E = 4;
const int F = 5;
const int G = 6;

int state[7] = { 1, 1, 1, 1, 1, 1, 1 };       //initiate state  ; 1=True
int sample_node[7] = { 0, 0, 0, 0, 0, 0, 0 }; //changable node or not ; 1=unchangable

int edge_list[12][2] = {                      //construct grapd edge 
	{ A, C }, { A, D }, { A, E }, { A, F },
	{ B, C }, { B, D }, { B, E }, { B, F },
	{ C, G }, { D, G }, { E, G }, { F, G },
};
double CPT_prob_A[1] = { 0.4828 };
double CPT_prob_B[1] = { 0.5 };
double CPT_prob_C[4] = { 0.4545, 0.7045, 0.5, 0.5789 };
double CPT_prob_D[4] = { 0.5, 0.4318, 0.6053, 0.6579 };
double CPT_prob_E[4] = { 0.5455, 0.7045, 0.6316, 0.6579 };
double CPT_prob_F[4] = { 0.4545, 0.4091, 0.3158, 0.4474 };
double CPT_prob_G[16] = { 0.4, 0.5, 0.3333, 0.9091, 0, 0, 0.1429, 0.1667, 1, 1, 1, 1, 0.7692, 1, 0.8889, 0.9333 };
double* CPT_list[7] = { CPT_prob_A, CPT_prob_B, CPT_prob_C, CPT_prob_D, CPT_prob_E, CPT_prob_F, CPT_prob_G };  //lits of index to CPT

using namespace std;

void gibbs_sampling(int);
void input();

int main()
{
	double count[7] = { 0, 0, 0, 0, 0, 0, 0 };	
	
	input();   // read in necessary input 
	cout << "\nstart..." << endl;
	
	for (int i = 0; i < 10000; i++){      //discard first 1000 sample 
		for (int y = 0; y < 7; y++){
			if (sample_node[y] == 0){    //sample changable node 
				gibbs_sampling(y);
			}
		}
	}
	for (int i = 0; i < 100000; i++){
		for (int y = 0; y < 7; y++){
			if (sample_node[y] == 0){    //sample changable node 
				gibbs_sampling(y);
			}
			if (state[y] == 1){
				count[y]++;
			}
		}
	}

	cout << "resault:" << endl;
	cout << "A:" << count[A] << "    prob:     " << count[A] / 1000 << "%" << endl;
	cout << "B:" << count[B] << "    prob:     " << count[B] / 1000 << "%" << endl;
	cout << "C:" << count[C] << "    prob:     " << count[C] / 1000 << "%" << endl;
	cout << "D:" << count[D] << "    prob:     " << count[D] / 1000 << "%" << endl;
	cout << "E:" << count[E] << "    prob:     " << count[E] / 1000 << "%" << endl;
	cout << "F:" << count[F] << "    prob:     " << count[F] / 1000 << "%" << endl;
	cout << "G:" << count[G] << "    prob:     " << count[G] / 1000 << "%" << endl;
	
	system("pause");
	return 0;

}

void input()
{
	char node_name = 'a';
	int node_state;

	cout << "opponent_power    A; //1=powerful oppo" << endl;
	cout << "home_or_away      B; //1=home" << endl;
	cout << "eFG               C; //1=bigger" << endl;
	cout << "TOV               D; //1=bigger" << endl;
	cout << "ORB               E; //1=bigger" << endl;
	cout << "FT                F; //1=bigger " << endl;
	cout << "win               G; //1=win " << endl;

	while (1){
		cout << "input the observe node and state(EX:a) & q for quit" << endl;
		cin >> node_name;
		if (node_name == 'q'){
			cout << "quit" << endl;
			break;
		}
		cout << "input the observe node state(1/0)" << endl;
		cin >> node_state;
		state[int(node_name) - 97] = node_state;
		sample_node[int(node_name) - 97] = 1;
	}

	cout << "node:     A B C D E F G" << endl;
	cout << "state:    ";
	for (int i = 0; i < 7; i++){
		cout << state[i] << ' ';
	}
	cout << endl;
	cout << "changable:";
	for (int i = 0; i < 7; i++){
		cout << sample_node[i] << ' ';
	}
	cout << "(0=changable)";

	return ;
}
void gibbs_sampling(int node)
{
	int parent[10] = {};
	int child[10] = {};
	int child_parent[10] = {};
	int parent_num = 0;
	int child_num = 0;
	int child_parent_num = 0;

	int index = 0;
	int p = 0;
	int store_node_state = 0;
	int store_parent_num = 0;
	int store_child_num = 0;

	double probability_0 = 1;
	double probability_1 = 1;
	double probability = 0;         //probability = probability_(0/1) / ( probability_1+probability_0)
	random_device rd;
	default_random_engine generator(rd());

	store_node_state = state[node];                //store the node state so we can change it ,more convenience 

	for (int i = 0; i < 12; i++){
		if (node == edge_list[i][1]){              //find out the parent of this node
			parent[parent_num] = edge_list[i][0];  //store the parent and number of parent
			parent_num++;
		}
		if (node == edge_list[i][0]){              //find out the child of this node
			child[child_num] = edge_list[i][1];    //store the child and number of parent
			child_num++;
		}
	}

	store_parent_num = parent_num;
	store_child_num = child_num;

	//count P(node=1|....)
	parent_num--;                                                      //because the index is start from 0
	state[node] = 1;
	while (parent_num + 1 > 0){                                        //count CPT index for this node 
		index += state[parent[parent_num]] * pow(2, (p));
		p++;
		parent_num--;
	}
	probability_1 *= CPT_list[node][index];                            //count P(node=1|parent(node))

	child_num--;
	index = 0;                                                         //reset index
	while (child_num + 1 > 0){
		child_parent_num = 0;
		for (int i = 0; i < 12; i++){                                  //find this child's parents 
			if (child[child_num] == edge_list[i][1]){
				child_parent[child_parent_num] = edge_list[i][0];
				child_parent_num++;
			}
		}
		child_parent_num--;                                                //almost same as above
		p = 0;
		while (child_parent_num + 1 > 0){                                        //count CPT index for this node 
			index += state[child_parent[child_parent_num]] * pow(2, (p));
			p++;
			child_parent_num--;
		}
		if (state[child[child_num]] == 1){
			probability_1 *= CPT_list[child[child_num]][index];                            //count P(child(node)=1|parent[child(node)])
		}
		else {
			probability_1 *= (1 - CPT_list[child[child_num]][index]);                     //count P(child(node)=0|parent[child(node)])
		}
		child_num--;
		index = 0;													   //reseet index
	}

	parent_num = store_parent_num;
	child_num = store_child_num;

	//count P(node=0|....)
	parent_num--;                                                      //because the index is start from 0
	state[node] = 0;
	p = 0;
	while (parent_num + 1 > 0){                                        //count CPT index for this node 
		index += state[parent[parent_num]] * pow(2, (p));
		p++;
		parent_num--;
	}
	probability_0 *= (1 - CPT_list[node][index]);                            //count P(node=0|parent(node))

	child_num--;
	index = 0;
	while (child_num + 1 > 0){
		child_parent_num = 0;
		for (int i = 0; i < 12; i++){                                  //find this child's parents 
			if (child[child_num] == edge_list[i][1]){
				child_parent[child_parent_num] = edge_list[i][0];
				child_parent_num++;
			}
		}
		child_parent_num--;                                                //almost same as above
		p = 0;
		while (child_parent_num + 1 > 0){                                        //count CPT index for this node 
			index += state[child_parent[child_parent_num]] * pow(2, (p));
			p++;
			child_parent_num--;
		}
		if (state[child[child_num]] == 1){
			probability_0 *= CPT_list[child[child_num]][index];                            //count P(child(node)=1|parent[child(node)])
		}
		else {
			probability_0 *= (1 - CPT_list[child[child_num]][index]);                     //count P(child(node)=0|parent[child(node)])
		}
		child_num--;
		index = 0;
	}

	probability = probability_1 / (probability_1 + probability_0);

	bernoulli_distribution distribution(probability);
	state[node] = distribution(generator);
	
	return;
}

