#ifndef USR
#define USR
// includes
#include "rc4.h"
#include "aes.h"
#include "rsa.h"
// functions
int User_interface_main_handler();
int verify_files(const char *input_filename, const char *output_filename);
#endif