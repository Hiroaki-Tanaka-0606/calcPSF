// calcPSF variables declaration
// description are in variables.hpp
#include <cstdio>

// constants
extern int buffer_length;
extern int Calculation_length;
extern int Log_file_length;
extern int Log_length;
extern char* Log_buffer;
extern int Solution_length;
extern int Potential_length;
extern int Potential_file_length;
extern double Data_read_error;
extern int HDF5_file_length;
extern int PS_state_length;
extern double au_ang;
extern double Eh;

// variables
/// blocks
extern bool Ct_block_appeared;
extern bool TF_block_appeared;
extern bool Rg_block_appeared;
extern bool At_block_appeared;
extern bool SC_block_appeared;
extern bool Oc_block_appeared;
extern bool PS_block_appeared;
/// Ct block
extern char* Calculation;
extern bool Calculation_set;
extern char* Log_file;
extern bool Log_file_set;
extern bool Console_log;
extern bool Console_log_set;
extern char* Output_file;
extern bool Output_file_set;
/// Rg block
extern int Radial_grid_count;
extern double* Radial_grid_intervals;
extern int* Radial_grid_points;
extern double* x_coordinates;
extern int x_count;
/// TF block
extern bool TF_test;
extern bool TF_test_set;
extern double Initial_diff_offset;
extern bool Initial_diff_offset_set;
extern double Initial_diff_delta;
extern bool Initial_diff_delta_set;
extern int Initial_diff_size;
extern bool Initial_diff_size_set;
extern double Initial_diff_min;
extern bool Initial_diff_min_set;
extern double Initial_diff_max;
extern bool Initial_diff_max_set;
extern double TF_threshold;
extern bool TF_threshold_set;
extern char* TF_solution;
extern double TF_solution_set;
extern double* TF_phi;
extern double* TF_phi_diff;
/// At block
extern int n_min;
extern bool n_min_set;
extern int n_max;
extern bool n_max_set;
extern int n_single;
extern int n_single_set;
extern int l_min;
extern bool l_min_set;
extern int l_max;
extern bool l_max_set;
extern int l_single;
extern int l_single_set;
extern int Z_min;
extern bool Z_min_set;
extern int Z_max;
extern bool Z_max_set;
extern int Z_single;
extern bool Z_single_set;
extern char* At_potential;
extern bool At_potential_set;
extern char* At_potential_file;
extern bool At_potential_file_set;
extern char* At_solution;
extern bool At_solution_set;
extern double Bisection_step;
extern double Bisection_step_set;
extern double At_radius_factor;
extern bool At_radius_factor_set;
extern double At_E_threshold;
extern bool At_E_threshold_set;
extern double At_initial_diff;
extern double At_initial_diff2;
extern double* At_p_x;
extern double* At_p_diff_x;
extern double* At_v_x;
extern double At_bisection_threshold;
extern int At_min_iteration;
extern int At_max_iteration;
/// SC block
extern double SC_mix_weight;
extern bool SC_mix_weight_set;
extern double SC_criterion_a;
extern bool SC_criterion_a_set;
extern double SC_criterion_b;
extern bool SC_criterion_b_set;
extern double* SC_sigma_x;
extern int SC_orbital_count;
extern int* SC_n_list;
extern int* SC_l_list;
extern double* SC_eigen_list;
extern double** SC_p_x;
/// Oc block
extern int Occupation_count;
extern int** At_occupation;
/// PS block
extern char* PS_input_file;
extern bool PS_input_file_set;
extern double PS_E_min;
extern bool PS_E_min_set;
extern double PS_E_max;
extern bool PS_E_max_set;
extern double PS_dE;
extern bool PS_dE_set;
extern double PS_E_pixel;
extern bool PS_E_pixel_set;
extern char* PS_initial_state;
extern bool PS_initial_state_set;
extern char* PS_final_state;
extern bool PS_final_state_set;
extern char* PS_polarization;
extern bool PS_polarization_set;
extern double PS_theta;
extern double PS_phi;
extern bool PS_theta_set;
extern bool PS_phi_set;
extern int PS_ext_up;
extern int PS_ext_ri;
extern int PS_ext_dn;
extern int PS_ext_le;
extern bool PS_ext_set;
extern char* PS_AO_file;
extern bool PS_AO_file_set;
extern double PS_sigma_max;

/// files
extern FILE* Log_file_obj;
extern FILE* Output_file_obj;

