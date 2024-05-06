#include "user_interface.h"

int User_interface_main_handler()
{
    int algorithm_option = -1;
    char input_file[1024];
    char output_file[1024];

    while (algorithm_option != 0)
    {
        printf("############################################\n");
        printf("#   Select an algorithm:                   #\n");
        printf("#   1. Rc4                                 #\n");
        printf("#   2. Aes                                 #\n");
        printf("#   3. Rsa                                 #\n");
        printf("#   0. Exit                                #\n");
        printf("############################################\n");
        printf("Option: ");
        scanf("%d", &algorithm_option);

        if (algorithm_option == 1)
        {
            int option;
            printf("Enter input file name: ");
            scanf("%1023s", input_file);
            printf("Enter output file name: ");
            scanf("%1023s", output_file);
            printf("Input file: %s, Output file: %s\n", input_file, output_file);
            printf("For encrypt press 1, for decrypt press 2: ");
            scanf("%d", &option);
            if (verify_files(input_file, output_file))
            {
                RC4_main_handler(option, input_file, output_file);
            }
        }
        else if (algorithm_option == 2)
        {

            int option;
            printf("Enter input file name: ");
            scanf("%1023s", input_file);
            printf("Enter output file name: ");
            scanf("%1023s", output_file);
            printf("Input file: %s, Output file: %s\n", input_file, output_file);
            printf("For encrypt press 1, for decrypt press 0: ");
            scanf("%d", &option);
            if (verify_files(input_file, output_file))
            {
                Aes_main_handler(option, input_file, output_file);
            }
        }
        else if (algorithm_option == 3)
        {

            int option;
            printf("Enter input file name: ");
            scanf("%1023s", input_file);
            printf("Enter output file name: ");
            scanf("%1023s", output_file);
            printf("Input file: %s, Output file: %s\n", input_file, output_file);
            printf("For encrypt press 1, for decrypt press 0: ");
            scanf("%d", &option);
            if (verify_files(input_file, output_file))
            {
                Rsa_Main_Handler(option, input_file, output_file);
            }
        }
        else if (algorithm_option == 0)
        {
            printf("Exiting...\n");
            break;
        }
        else
        {
            printf("Invalid option, please try again.\n");
        }
    }
    return 0;
}
int verify_files(const char *input_filename, const char *output_filename)
{
    FILE *file;

    file = fopen(input_filename, "r");
    if (file == NULL)
    {
        perror("Error opening input file");
        return FALSE;
    }
    fclose(file);

    file = fopen(output_filename, "w");
    if (file == NULL)
    {
        perror("Error opening or creating output file");
        return FALSE;
    }
    fclose(file);

    return TRUE;
}