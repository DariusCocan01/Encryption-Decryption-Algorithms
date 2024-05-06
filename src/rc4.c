#include "rc4.h"

int initial_state[256];
int initial_key[256];
int permutation_vector[256];
char *input = NULL;
int input_size = 0;

int Initialize_default_state(int *arr)
{
    int i = 0;
    while (i < 256)
    {
        *arr = i;
        arr++;
        i++;
    }
    return TRUE;
}
int Permutation()
{
    int j = 0;
    for (int i = 0; i < 256; i++)
    {
        j = (j + initial_state[i] + initial_key[i]) % 256;
        int x = initial_state[i];
        initial_state[i] = initial_state[j];
        initial_state[j] = x;
    }
    return TRUE;
}
int Initialize_key()
{
    FILE *file;
    char buffer[257];
    size_t num_read;

    file = fopen("files/rc4Key.txt", "r");
    if (file == NULL)
    {
        perror("Error opening rc4Key file\n");
        return FALSE;
    }
    num_read = fread(buffer, sizeof(char), 256, file);

    buffer[num_read] = '\0';
    fclose(file);

    int j = 0;
    for (int i = 0; i < 256; i++)
    {
        if (j == num_read)
        {
            j = 0;
        }
        int ascii_value = (int)buffer[j];
        initial_key[i] = ascii_value;
        j++;
    }

    return TRUE;
}
int Input_handler(char **input, const char *filename)
{
    FILE *file;
    size_t num_read;
    char *buffer;
    long file_size;
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error reading input");
        return FALSE;
    }
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Memory error\n");
        fclose(file);
        return FALSE;
    }

    num_read = fread(buffer, sizeof(char), file_size, file);

    buffer[num_read] = '\0';

    *input = buffer;
    fclose(file);

    return num_read;
}
void RC4_generate_stream(int length, int *key)
{
    int i = 0, j = 0, t;
    for (int k = 0; k < length; k++)
    {
        i = (i + 1) % 256;
        j = (j + initial_state[i]) % 256;

        t = initial_state[i];
        initial_state[i] = initial_state[j];
        initial_state[j] = t;

        int r = (initial_state[i] + initial_state[j]) % 256;
        key[k] = initial_state[r];
    }
}
int Output_Handler(int arr[], int length, const char *filename)
{
    FILE *file;
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return FALSE;
    }
    for (int i = 0; i < length; i++)
    {
        fputc(arr[i], file);
    }
    fclose(file);
    return TRUE;
}
void Encrypt(int *key, char *mes, int length, const char *output_file)
{
    int output_data[length];
    for (int i = 0; i < length; i++)
    {
        output_data[i] = key[i] ^ (int)mes[i];
    }
    Output_Handler(output_data, length, output_file);
}
void Decrypt(int *key, char *mes, int length, const char *output_file)
{
    int output_data[length];
    for (int i = 0; i < length; i++)
    {
        output_data[i] = key[i] ^ (int)mes[i];
    }
    Output_Handler(output_data, length, output_file);
}
void Reset_State()
{
    for (int i = 0; i < 256; i++)
    {
        initial_state[i] = i;
    }

    memset(initial_key, 0, sizeof(initial_key));
    memset(permutation_vector, 0, sizeof(permutation_vector));

    if (input != NULL)
    {
        free(input);
        input = NULL;
    }
    input_size = 0;
}

void RC4_main_handler(int option, const char *input_file, const char *output_file)
{
    Reset_State();
    Initialize_default_state(initial_state);
    Initialize_key();
    Permutation();
    input_size = Input_handler(&input, input_file);
    if (input_size == FALSE)
    {
        fprintf(stderr, "Failed to read input file.\n");
        return;
    }

    int key[input_size];
    RC4_generate_stream(input_size, key);

    if (option == 1)
    {
        Encrypt(key, input, input_size, output_file);
    }
    else
    {
        Decrypt(key, input, input_size, output_file);
    }

    free(input);
    input = NULL;
}