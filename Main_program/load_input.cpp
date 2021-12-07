// calcPSF load input

#include <string>
#include <iostream>
#include <cstring>

#include "variables_ext.hpp"
#include "parse_inputs.hpp"

using namespace std;

void output_error(int line_number, char* description){
	printf("Error in line %d: %s\n", line_number, description);
}

int load_input(){
	// buffer variables
	string input_line_s;
	char* input_line_c=new char[buffer_length+1];
	int actual_line_length;
	char* keyword_buffer=new char[buffer_length+1];
	int sscanf_status;
	int parse_status;
	char* value_buffer=new char[buffer_length+1];

	// status variables
	int status=1;            // 1: OK, 0: error
	int line_number=0;       // number of the line being loaded
	bool in_the_block=false; // true when the line is in the block, false when not
	string* block_name;      // name of the block which the line is being in
	int Radial_grid_index=0; // current index of the radial grid configuration
	int Oc_orbital_index=0;  // current index of the occupation configuration

	while(getline(cin, input_line_s)){
		// cout << input_line_s << endl;

		//increase line number
		line_number++;
		
		// read one line from cin (stdin), copy to input_line_c
		actual_line_length=input_line_s.length();
		if(actual_line_length>buffer_length){
		  printf("Warning: line %d is longer than %d characters, the last part is ignored\n", line_number, buffer_length);
			actual_line_length=buffer_length;
		}
		input_line_s.copy(input_line_c, actual_line_length);
		input_line_c[actual_line_length]='\0';
		// cout << input_line_c << endl;

		// extract the first word
		sscanf_status=sscanf(input_line_c, "%s", keyword_buffer);
		/// sscanf_status<1 means nothing is in the line
		if(sscanf_status<1){
			printf("Line %d is a blank, ignoring\n", line_number);
			continue;
		}
		
		// distinguish the type of the line
		/// comment: 
		if(keyword_buffer[0]=='!' || keyword_buffer[0]== '#'){
			printf("Line %d is a comment, ignoring\n", line_number);
			continue;
		}
		/// start of the block
		if(keyword_buffer[0]=='&'){
			// check whether the line is already in a block or not
			if(in_the_block){
				output_error(line_number, (char*)"already in a block");
				status=0;
				goto FINALIZATION;
			}
			in_the_block=true;
			
			// extract the block name
			int keyword_buffer_length=strlen(keyword_buffer);
			block_name=new string(&keyword_buffer[1], &keyword_buffer[keyword_buffer_length]);

			// check whether the block name is valid and the block appears for the first time
			if(*block_name==string("Control")){
				// Ct: Control
				if(Ct_block_appeared){
					output_error(line_number, (char*)"block 'Control' already appeared"); status=0; goto FINALIZATION;
				}
				Ct_block_appeared=true;
			}else if(*block_name==string("Thomas-Fermi")){
				// TF: Thomas-Fermi
				if(TF_block_appeared){
					output_error(line_number, (char*)"block 'Thomas-Fermi' already appeared"); status=0; goto FINALIZATION;
				}
				TF_block_appeared=true;
			}else if(*block_name==string("Atomic-wfn")){
				// At: Atomic-wfn (wavefunction)
				if(At_block_appeared){
					output_error(line_number, (char*)"block 'Atomic-wfn' already appeared"); status=0; goto FINALIZATION;
				}
				At_block_appeared=true;
			}else if(*block_name==string("Radial-grid")){
				// Rg: Radial grid
				if(Rg_block_appeared){
					output_error(line_number, (char*)"block 'Radial-grid' already appeared"); status=0; goto FINALIZATION;
				}
				Rg_block_appeared=true;
				/// read the number of rows
				sscanf_status=sscanf(input_line_c, "%*s %d", &Radial_grid_count);
				if(sscanf_status<1 || Radial_grid_count<1){
					output_error(line_number, (char*)"invalid number of radial grids"); status=0; goto FINALIZATION;
				}
				Radial_grid_intervals=new double[Radial_grid_count];
				Radial_grid_points=new int[Radial_grid_count];
			}else if(*block_name==string("SCF-atom")){
				// SC: SCF-atom
				if(SC_block_appeared){
					output_error(line_number, (char*)"block 'SCF-atom' already appeared"); status=0; goto FINALIZATION;
				}
				SC_block_appeared=true;
			}else if(*block_name==string("Occupation")){
				// Oc: Occupation of an atom
				if(Oc_block_appeared){
					output_error(line_number, (char*)"block 'Occupation' already appeared"); status=0; goto FINALIZATION;
				}
				Oc_block_appeared=true;
				/// read the number of rows
				sscanf_status=sscanf(input_line_c, "%*s %d", &Occupation_count);
				if(sscanf_status<1 || Occupation_count<1){
					output_error(line_number, (char*)"invalid number of Occupation_count"); status=0; goto FINALIZATION;
				}
				int* At_occupation_alloc=new int[Occupation_count*Occupation_count];
				for(int i=0; i<Occupation_count*Occupation_count; i++){
					At_occupation_alloc[i]=0;
				}
			  At_occupation=new int*[Occupation_count];
				for(int i=0; i<Occupation_count; i++){
					At_occupation[i]=&At_occupation_alloc[i*Occupation_count];
				}
			}else if(*block_name==string("PSF")){
				// PS: Photoemission structure factor (PSF)
				if(PS_block_appeared){
					output_error(line_number, (char*)"block 'PSF' already appeared"); status=0; goto FINALIZATION;
				}
				PS_block_appeared=true;
			}else{
				// none of the above
				output_error(line_number, (char*)"invalid block name"); status=0; goto FINALIZATION;
			}
			printf("Block %s started at line %d\n", block_name->c_str(), line_number);
			continue;
		}
		/// end of the block
		if(keyword_buffer[0]=='/' && strlen(keyword_buffer)==1){
			printf("Block %s ended at line %d\n", block_name->c_str(), line_number);
			in_the_block=false;
			if(*block_name==string("Radial-grid") && Radial_grid_index!=Radial_grid_count){
				output_error(line_number, (char*)"inequal number of radial grids"); status=0; goto FINALIZATION;
			}
			if(*block_name==string("Occupation") && Oc_orbital_index!=Occupation_count){
				output_error(line_number, (char*)"inequal number of orbital indices"); status=0; goto FINALIZATION;
			}
			
			continue;
		}
		/// something invalid, out of the block
		if(!in_the_block){
			output_error(line_number, (char*)"something invalid appears out of the block");
			status=0;
			goto FINALIZATION;
		}
		
		/// special block
		if(*block_name==string("Radial-grid")){
			// Rg: (interval) (points)
			if(Radial_grid_index>=Radial_grid_count){
				output_error(line_number, (char*)"too much configurations in the Radial-grid block"); status=0; goto FINALIZATION;
			}
			sscanf_status=sscanf(input_line_c,"%lf %d", &Radial_grid_intervals[Radial_grid_index], &Radial_grid_points[Radial_grid_index]);
			if(sscanf_status<2 || Radial_grid_intervals[Radial_grid_index]<0 || Radial_grid_points[Radial_grid_index]<=0){
				output_error(line_number, (char*)"invalid value(s) of a radial grid"); status=0; goto FINALIZATION;
			}
			Radial_grid_index++;
			continue;
		}	  
		if(*block_name==string("Occupation")){
			// Oc: (s orbital) (p orbital) (d orbtital) ...
			if(Oc_orbital_index>=Occupation_count){
				output_error(line_number, (char*)"too much configurations in the Occupation block"); status=0; goto FINALIZATION;
			}
			int number_of_targets=Oc_orbital_index+1;
			char* sscanf_template=new char[buffer_length+1];
			int i, j;
			for(i=0; i<number_of_targets; i++){
				sscanf_template[0]='\0';
				for(j=0; j<=i-1; j++){
					sprintf(sscanf_template, "%s %%*d", sscanf_template);
				}
				sprintf(sscanf_template, "%s %%d", sscanf_template);
				sscanf_status=sscanf(input_line_c, sscanf_template, &At_occupation[Oc_orbital_index][i]);
			
				if(sscanf_status<1 || At_occupation[Oc_orbital_index][i]<0){
					output_error(line_number, (char*)"invalid value(s) of a occupation number"); status=0; goto FINALIZATION;
				}
			}
			Oc_orbital_index++;
			delete sscanf_template;
			continue;
		}

		/// usual block, "(keyword) (value)"
		/* template
			/// Variable: type
			if(strcmp(keyword_buffer, "Variable")==0){
				if(Variable_set){
					output_error(line_number, (char*)"keyword Variable already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_type(input_line_c, (char*)"Variable", (arguments,) value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Variable"); status=0; goto FINALIZATION;
				}
			  Variable_set=true; continue;
			}*/
			
		if(*block_name==string("Control")){
			// Ct block
			/// Calculation: char[]
			if(strcmp(keyword_buffer, "Calculation")==0){
				if(Calculation_set){
					output_error(line_number, (char*)"keyword Calculation already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Calculation", Calculation, Calculation_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Calculation"); status=0; goto FINALIZATION;
				}
				Calculation_set=true;
				continue;
			}
			/// Log_file: char[]
			if(strcmp(keyword_buffer, "Log_file")==0){
				if(Log_file_set){
					output_error(line_number, (char*)"keyword Log_file already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Log_file", Log_file, Log_file_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Log_file"); status=0; goto FINALIZATION;
				}
				Log_file_set=true;
				continue;
			}
			/// Console_log: bool
			if(strcmp(keyword_buffer, "Console_log")==0){
				if(Console_log_set){
					output_error(line_number, (char*)"keyword Console_log already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_bool(input_line_c, &Console_log, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Console_log"); status=0; goto FINALIZATION;
				}
				Console_log_set=true;
				continue;
			}
			/// Output_file: char[]
			if(strcmp(keyword_buffer, "Output_file")==0){
				if(Output_file_set){
					output_error(line_number, (char*)"keyword Output_file already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Output_file", Output_file, Log_file_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Output_file"); status=0; goto FINALIZATION;
				}
				Output_file_set=true;
				continue;
			}
		}else if(*block_name==string("Thomas-Fermi")){
			// TF block
			/// Calculation_test (TF_test): bool
			if(strcmp(keyword_buffer, "Calculation_test")==0){
				if(TF_test_set){
					output_error(line_number, (char*)"keyword Calculation_test already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_bool(input_line_c, &TF_test, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Calculation_test"); status=0; goto FINALIZATION;
				}
			  TF_test_set=true; continue;
			}
			/// Initial_diff_offset: double
			if(strcmp(keyword_buffer, "Initial_diff_offset")==0){
				if(Initial_diff_offset_set){
					output_error(line_number, (char*)"keyword Initial_diff_offset already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &Initial_diff_offset);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Initial_diff_offset"); status=0; goto FINALIZATION;
				}
			  Initial_diff_offset_set=true; continue;
			}
			/// Initial_diff_delta: double
			if(strcmp(keyword_buffer, "Initial_diff_delta")==0){
				if(Initial_diff_delta_set){
					output_error(line_number, (char*)"keyword Initial_diff_delta already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &Initial_diff_delta);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Initial_diff_delta"); status=0; goto FINALIZATION;
				}
			  Initial_diff_delta_set=true; continue;
			}
			/// Initial_diff_size: int
			if(strcmp(keyword_buffer, "Initial_diff_size")==0){
				if(Initial_diff_size_set){
					output_error(line_number, (char*)"keyword Initial_diff_size already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &Initial_diff_size);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Initial_diff_size"); status=0; goto FINALIZATION;
				}
			  Initial_diff_size_set=true; continue;
			}
			/// Initial_diff_min: double
			if(strcmp(keyword_buffer, "Initial_diff_min")==0){
				if(Initial_diff_min_set){
					output_error(line_number, (char*)"keyword Initial_diff_min already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &Initial_diff_min);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Initial_diff_min"); status=0; goto FINALIZATION;
				}
			  Initial_diff_min_set=true; continue;
			}
			/// Initial_diff_max: double
			if(strcmp(keyword_buffer, "Initial_diff_max")==0){
				if(Initial_diff_max_set){
					output_error(line_number, (char*)"keyword Initial_diff_max already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &Initial_diff_max);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Initial_diff_max"); status=0; goto FINALIZATION;
				}
			  Initial_diff_max_set=true; continue;
			}
			/// Threshold(TF_threshold): double
			if(strcmp(keyword_buffer, "Threshold")==0){
				if(TF_threshold_set){
					output_error(line_number, (char*)"keyword Threshold already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &TF_threshold);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Threshold"); status=0; goto FINALIZATION;
				}
			  TF_threshold_set=true; continue;
			}
			/// Solution: char[]
			if(strcmp(keyword_buffer, "Solution")==0){
				if(TF_solution_set){
					output_error(line_number, (char*)"keyword Solution already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Solution", TF_solution, Solution_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Solution"); status=0; goto FINALIZATION;
				}
				TF_solution_set=true;
				continue;
			}
		}else if(*block_name==string("Atomic-wfn")){
			// At block
			/// n_min: int
			if(strcmp(keyword_buffer, "n_min")==0){
				if(n_min_set){
					output_error(line_number, (char*)"keyword n_min already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &n_min);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of n_min"); status=0; goto FINALIZATION;
				}
			  n_min_set=true; continue;
			}
			/// n_max: int
			if(strcmp(keyword_buffer, "n_max")==0){
				if(n_max_set){
					output_error(line_number, (char*)"keyword n_max already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &n_max);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of n_max"); status=0; goto FINALIZATION;
				}
			  n_max_set=true; continue;
			}
			/// n (n_single): int
			if(strcmp(keyword_buffer, "n")==0){
				if(n_single_set){
					output_error(line_number, (char*)"keyword n already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &n_single);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of n"); status=0; goto FINALIZATION;
				}
			  n_single_set=true; continue;
			}
			/// l_min: int
			if(strcmp(keyword_buffer, "l_min")==0){
				if(l_min_set){
					output_error(line_number, (char*)"keyword l_min already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &l_min);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of l_min"); status=0; goto FINALIZATION;
				}
			  l_min_set=true; continue;
			}
			/// l_max: int
			if(strcmp(keyword_buffer, "l_max")==0){
				if(l_max_set){
					output_error(line_number, (char*)"keyword l_max already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &l_max);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of l_max"); status=0; goto FINALIZATION;
				}
			  l_max_set=true; continue;
			}
			/// l (l_single): int
			if(strcmp(keyword_buffer, "l")==0){
				if(l_single_set){
					output_error(line_number, (char*)"keyword l already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &l_single);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of l"); status=0; goto FINALIZATION;
				}
			  l_single_set=true; continue;
			}
			/// Z_min: int
			if(strcmp(keyword_buffer, "Z_min")==0){
				if(Z_min_set){
					output_error(line_number, (char*)"keyword Z_min already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &Z_min);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Z_min"); status=0; goto FINALIZATION;
				}
			  Z_min_set=true; continue;
			}
			/// Z_max: int
			if(strcmp(keyword_buffer, "Z_max")==0){
				if(Z_max_set){
					output_error(line_number, (char*)"keyword Z_max already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &Z_max);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Z_max"); status=0; goto FINALIZATION;
				}
			  Z_max_set=true; continue;
			}
			/// Z (Z_single): int
			if(strcmp(keyword_buffer, "Z")==0){
				if(Z_single_set){
					output_error(line_number, (char*)"keyword Z already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_int(input_line_c, &Z_single);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Z"); status=0; goto FINALIZATION;
				}
			  Z_single_set=true; continue;
			}
			/// Potential (At_potential): char[]
			if(strcmp(keyword_buffer, "Potential")==0){
				if(At_potential_set){
					output_error(line_number, (char*)"keyword Potential already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Potential", At_potential, Potential_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Potential"); status=0; goto FINALIZATION;
				}
				At_potential_set=true;
				continue;
			}
			/// Potential_file (At_potential_file): char[]
			if(strcmp(keyword_buffer, "Potential_file")==0){
				if(At_potential_file_set){
					output_error(line_number, (char*)"keyword Potential_file already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Potential_file", At_potential_file, Potential_file_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Potential_file"); status=0; goto FINALIZATION;
				}
				At_potential_file_set=true;
				continue;
			}
			
			/// Solution (At_solution): char[]
			if(strcmp(keyword_buffer, "Solution")==0){
				if(At_solution_set){
					output_error(line_number, (char*)"keyword Solution already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Solution", At_solution, Solution_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Solution"); status=0; goto FINALIZATION;
				}
				At_solution_set=true;
				continue;
			}
			/// Bisection_step: double
			if(strcmp(keyword_buffer, "Bisection_step")==0){
				if(Bisection_step_set){
					output_error(line_number, (char*)"keyword Bisection_step already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &Bisection_step);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Bisection_step"); status=0; goto FINALIZATION;
				}
			  Bisection_step_set=true; continue;
			}
			/// E_threshold (At_E_threshold): double
			if(strcmp(keyword_buffer, "E_threshold")==0){
				if(At_E_threshold_set){
					output_error(line_number, (char*)"keyword E_threshold already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &At_E_threshold);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of E_threshold"); status=0; goto FINALIZATION;
				}
			  At_E_threshold_set=true; continue;
			}
			/// Radius_factor (At_radius_factor): double
			if(strcmp(keyword_buffer, "Radius_factor")==0){
				if(At_radius_factor_set){
					output_error(line_number, (char*)"keyword Radius_factor already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &At_radius_factor);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Radius_factor"); status=0; goto FINALIZATION;
				}
			  At_radius_factor_set=true; continue;
			}
			
		}else if(*block_name==string("SCF-atom")){
			// SC block
			/// Mix_weight (SC_mix_weight): double
			if(strcmp(keyword_buffer, "Mix_weight")==0){
				if(SC_mix_weight_set){
					output_error(line_number, (char*)"keyword Mix_weight already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &SC_mix_weight);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Mix_weight"); status=0; goto FINALIZATION;
				}
			  SC_mix_weight_set=true; continue;
			}
			/// Criterion_a (SC_criterion_a): double
			if(strcmp(keyword_buffer, "Criterion_a")==0){
				if(SC_criterion_a_set){
					output_error(line_number, (char*)"keyword Criterion_a already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &SC_criterion_a);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of SC_criterion_a"); status=0; goto FINALIZATION;
				}
			  SC_criterion_a_set=true; continue;
			}
			/// Criterion_b (SC_criterion_b): double
			if(strcmp(keyword_buffer, "Criterion_b")==0){
				if(SC_criterion_b_set){
					output_error(line_number, (char*)"keyword Criterion_b already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &SC_criterion_b);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of SC_criterion_b"); status=0; goto FINALIZATION;
				}
			  SC_criterion_b_set=true; continue;
			}
		}else if(*block_name==string("PSF")){
			// PS block
			/// Input_file (PS_input_file): char[]
			if(strcmp(keyword_buffer, "Input_file")==0){
				if(PS_input_file_set){
					output_error(line_number, (char*)"keyword Input_file already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Input_file", PS_input_file, HDF5_file_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Input_file"); status=0; goto FINALIZATION;
				}
				PS_input_file_set=true; continue;
			}
			/// E_min (PS_E_min): double
			if(strcmp(keyword_buffer, "E_min")==0){
				if(PS_E_min_set){
					output_error(line_number, (char*)"keyword E_min already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &PS_E_min);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of E_min"); status=0; goto FINALIZATION;
				}
			  PS_E_min_set=true; continue;
			}
			/// E_max (PS_E_max): double
			if(strcmp(keyword_buffer, "E_max")==0){
				if(PS_E_max_set){
					output_error(line_number, (char*)"keyword E_max already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &PS_E_max);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of E_max"); status=0; goto FINALIZATION;
				}
			  PS_E_max_set=true; continue;
			}
			/// dE (PS_dE): double
			if(strcmp(keyword_buffer, "dE")==0){
				if(PS_dE_set){
					output_error(line_number, (char*)"keyword dE already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &PS_dE);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of dE"); status=0; goto FINALIZATION;
				}
			  PS_dE_set=true; continue;
			}
			/// E_pixel (PS_E_pixel): double
			if(strcmp(keyword_buffer, "E_pixel")==0){
				if(PS_E_pixel_set){
					output_error(line_number, (char*)"keyword E_pixel already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &PS_E_pixel);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of E_pixel"); status=0; goto FINALIZATION;
				}
			  PS_E_pixel_set=true; continue;
			}
			/// Initial_state (PS_initial_state): char[]
			if(strcmp(keyword_buffer, "Initial_state")==0){
				if(PS_initial_state_set){
					output_error(line_number, (char*)"keyword Initial_state already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Initial_state", PS_initial_state, PS_state_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Initial_state"); status=0; goto FINALIZATION;
				}
				PS_initial_state_set=true; continue;
			}
			/// Final_state (PS_final_state): char[]
			if(strcmp(keyword_buffer, "Final_state")==0){
				if(PS_final_state_set){
					output_error(line_number, (char*)"keyword Final_state already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Final_state", PS_final_state, PS_state_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Final_state"); status=0; goto FINALIZATION;
				}
				PS_final_state_set=true; continue;
			}
			/// Polarization (PS_polarization): char[]
			if(strcmp(keyword_buffer, "Polarization")==0){
				if(PS_polarization_set){
					output_error(line_number, (char*)"keyword Polarization already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Polarization", PS_polarization, PS_state_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Polarization"); status=0; goto FINALIZATION;
				}
				PS_polarization_set=true; continue;
			}
			/// Theta (PS_theta): double
			if(strcmp(keyword_buffer, "Theta")==0){
				if(PS_theta_set){
					output_error(line_number, (char*)"keyword Theta already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &PS_theta);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Theta"); status=0; goto FINALIZATION;
				}
			  PS_theta_set=true; continue;
			}
			/// Phi (PS_phi): double
			if(strcmp(keyword_buffer, "Phi")==0){
				if(PS_phi_set){
					output_error(line_number, (char*)"keyword Phi already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_double(input_line_c, &PS_phi);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Phi"); status=0; goto FINALIZATION;
				}
			  PS_phi_set=true; continue;
			}
			/// Atomic_orbitals_file (PS_AO_file): char[]
			if(strcmp(keyword_buffer, "Atomic_orbitals_file")==0){
				if(PS_AO_file_set){
					output_error(line_number, (char*)"keyword Atomic_orbitals_file already appeared"); status=0; goto FINALIZATION;
				}
				parse_status=parse_char(input_line_c, (char*)"Atomic_orbitals_file", PS_AO_file, HDF5_file_length, value_buffer);
				if(parse_status==0){
					output_error(line_number, (char*)"invalid value of Atomic_orbitals_file"); status=0; goto FINALIZATION;
				}
				PS_AO_file_set=true; continue;
			}
			/// Extend (PS_ext_(up|ri|dn|le)): int[4]
			/// if dimension==1, only right and left are used
			if(strcmp(keyword_buffer, "Extend")==0){
				if(PS_ext_set){
					output_error(line_number, (char*)"keyword Extend already appeared"); status=0; goto FINALIZATION;
				}
				sscanf_status=sscanf(input_line_c, "%*s %d %d %d %d", &PS_ext_up, &PS_ext_ri, &PS_ext_dn, &PS_ext_le);
				if(sscanf_status!=4){
					output_error(line_number, (char*)"invalid value of Extend"); status=0; goto FINALIZATION;
				}
				PS_ext_set=true; continue;
			}
			
		}else{
			// none of the above, which should not happen
			output_error(line_number, (char*)"unexpected block name error");
			status=0;
			goto FINALIZATION;
		}
		output_error(line_number, (char*)"invalid keyword");
		status=0;
		goto FINALIZATION;
	}

 FINALIZATION:
	delete input_line_c;
	delete keyword_buffer;
	return status;
}
