#ifndef RC4
#define RC4

// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// defines
#define TRUE 1
#define FALSE 0

// functions
void RC4_main_handler(int option, const char *input_file, const char *output_file);
int Initialize_key();
int Initialize_default_state(int *arr);
int Input_handler(char **input, const char *filename);
int Output_Handler(int arr[], int length, const char *filename);
void Encrypt(int *key, char *mes, int length, const char *output_file);
void Decrypt(int *key, char *mes, int length, const char *output_file);
void RC4_generate_stream(int length, int *key);
void Reset_State();
#endif