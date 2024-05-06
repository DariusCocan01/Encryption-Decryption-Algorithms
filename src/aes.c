#include "aes.h"

static char *input = NULL;
static char *key = NULL;
static int key_size;
static int input_size;
static int output_size;
static unsigned char round_keys[11][16];

int Output_Handler_aes(unsigned char *arr, int length, const char *filename, int enc)
{
    FILE *file;
    file = fopen(filename, "wb");
    if (file == NULL)
    {
        return FALSE;
    }
    if (!enc)
    {
        int x = (int)arr[length - 1];
        length -= x;
    }
    fwrite(arr, sizeof(unsigned char), length, file);
    fclose(file);
    return TRUE;
}
void Initialize_output()
{
    if (input_size % 16 == 0)
    {
        output_size = input_size;
    }
    else
    {
        output_size = input_size + (16 - (input_size % 16));
    }
}
unsigned char gmul(unsigned char a, unsigned char b)
{
    unsigned char p = 0;
    unsigned char hi_bit_set;
    for (int i = 0; i < 8; i++)
    {
        if (b & 1)
            p ^= a;
        hi_bit_set = a & 0x80;
        a <<= 1;
        if (hi_bit_set)
            a ^= 0x1B;
        b >>= 1;
    }
    return p;
}
void Convert_to_byte(char *input, int length, unsigned char *byte_array)
{
    for (int i = 0; i < length; i++)
    {
        byte_array[i] = (unsigned char)input[i];
    }
}
void Verify(char *key, char *input)
{
    while (*key != '\0')
    {
        key_size++;
        key++;
    }
    while (*input != '\0')
    {
        input_size++;
        input++;
    }
}
void Initialize_round_keys(unsigned char *initial_key)
{
    for (int i = 0; i < 16; i++)
    {
        round_keys[0][i] = initial_key[i];
    }
}
void Copy_vectors(unsigned char *v1, unsigned char *v2, int length)
{
    for (int i = 0; i < length; i++)
    {
        v2[i] = v1[i];
    }
}
void Circular_byte_left_shift(unsigned char *v, int length)
{
    unsigned char aux = v[0];
    for (int i = 0; i < length; i++)
    {
        v[i] = v[i + 1];
    }
    v = v + 3;
    *v = aux;
}
void Byte_substitution(unsigned char *v, int length)
{
    for (int i = 0; i < length; i++)
    {
        v[i] = s_box[v[i]];
    }
}
void Add_round_constant(unsigned char *v, int round)
{
    v[0] = v[0] ^ round_constant[round - 1];
}
void XOR_vectors(unsigned char *destination, unsigned char *v1, unsigned char *v2, int length)
{
    for (int i = 0; i < length; i++)
    {
        destination[i] = v1[i] ^ v2[i];
    }
}
void Make_round_keys(unsigned char *current_round_key, unsigned char *next_round_key, int length, int round)
{
    Copy_vectors(current_round_key, next_round_key, length);
    Circular_byte_left_shift(next_round_key + 12, 4);
    Byte_substitution(next_round_key + 12, 4);
    Add_round_constant(next_round_key + 12, round);
    XOR_vectors(next_round_key, current_round_key, next_round_key + 12, 4);
    XOR_vectors(next_round_key + 4, next_round_key, current_round_key + 4, 4);
    XOR_vectors(next_round_key + 8, next_round_key + 4, current_round_key + 8, 4);
    XOR_vectors(next_round_key + 12, next_round_key + 8, current_round_key + 12, 4);
}
void Shift_row(unsigned char (*m)[4])
{
    int nr = 1;
    while (nr <= 3)
    {
        int aux = nr;
        while (aux)
        {
            Circular_byte_left_shift(m[nr], 4);
            aux--;
        }
        nr++;
    }
}
void Print_matrix(unsigned char matrix[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            printf("%02X ", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
void Print_message(unsigned char *mes, int length)
{
    for (int i = 0; i < length; i++)
    {
        printf("%02X ", mes[i]);
    }
    printf("\n");
}
void Construct_matrix(unsigned char *mes, unsigned char (*matrix)[4])
{
    int q = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            matrix[j][i] = mes[q++];
        }
    }
}
void Deconstruct_matrix(unsigned char *mes, unsigned char (*matrix)[4])
{
    int q = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            mes[q++] = matrix[j][i];
        }
    }
}
void XOR_matrix(unsigned char (*m1)[4], unsigned char (*m2)[4])
{
    for (int i = 0; i < 4; i++)
    {
        XOR_vectors(m1[i], m1[i], m2[i], 4);
    }
}
void Substitution_matrix(unsigned char (*m)[4])
{
    for (int i = 0; i < 4; i++)
    {
        Byte_substitution(m[i], 4);
    }
}
void Mix_columns(unsigned char (*m)[4])
{
    unsigned char aux[4][4];
    for (int c = 0; c < 4; c++)
    {
        for (int i = 0; i < 4; i++)
        {
            aux[i][c] = gmul(mix_columns_matrix[i][0], m[0][c]) ^
                        gmul(mix_columns_matrix[i][1], m[1][c]) ^
                        gmul(mix_columns_matrix[i][2], m[2][c]) ^
                        gmul(mix_columns_matrix[i][3], m[3][c]);
        }
    }
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = aux[i][j];
        }
    }
}
void Aes_algorithm(unsigned char *mes, unsigned char *current_key, int round)
{
    unsigned char state_matrix[4][4];
    unsigned char key_matrix[4][4];

    Construct_matrix(mes, state_matrix);
    Construct_matrix(current_key, key_matrix);

    if (round == 0)
    {
        XOR_matrix(state_matrix, key_matrix);
    }
    if (round > 0 && round < 10)
    {
        printf("Round: %d\n", round);
        Substitution_matrix(state_matrix);
        Shift_row(state_matrix);
        Mix_columns(state_matrix);
        XOR_matrix(state_matrix, key_matrix);
        Print_matrix(state_matrix);
    }
    if (round == 10)
    {
        printf("Round: %d\n", round);
        Substitution_matrix(state_matrix);
        Shift_row(state_matrix);
        XOR_matrix(state_matrix, key_matrix);
        Print_matrix(state_matrix);
    }
    Deconstruct_matrix(mes, state_matrix);
}
void InvShift_row(unsigned char (*m)[4])
{
    for (int row = 1; row < 4; row++)
    {
        for (int shift = 0; shift < row; shift++)
        {
            unsigned char temp = m[row][3];
            for (int col = 3; col > 0; col--)
            {
                m[row][col] = m[row][col - 1];
            }
            m[row][0] = temp;
        }
    }
}

void InvSubstitution_matrix(unsigned char (*m)[4])
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            m[row][col] = inv_s_box[m[row][col]];
        }
    }
}

void InvMix_columns(unsigned char (*m)[4])
{
    unsigned char temp[4][4];
    for (int col = 0; col < 4; col++)
    {
        temp[0][col] = gmul(0x0E, m[0][col]) ^ gmul(0x0B, m[1][col]) ^ gmul(0x0D, m[2][col]) ^ gmul(0x09, m[3][col]);
        temp[1][col] = gmul(0x09, m[0][col]) ^ gmul(0x0E, m[1][col]) ^ gmul(0x0B, m[2][col]) ^ gmul(0x0D, m[3][col]);
        temp[2][col] = gmul(0x0D, m[0][col]) ^ gmul(0x09, m[1][col]) ^ gmul(0x0E, m[2][col]) ^ gmul(0x0B, m[3][col]);
        temp[3][col] = gmul(0x0B, m[0][col]) ^ gmul(0x0D, m[1][col]) ^ gmul(0x09, m[2][col]) ^ gmul(0x0E, m[3][col]);
    }

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            m[row][col] = temp[row][col];
        }
    }
}

void Aes_decrypt(unsigned char *mes, unsigned char *current_key, int round)
{
    unsigned char state_matrix[4][4];
    unsigned char key_matrix[4][4];

    Construct_matrix(mes, state_matrix);
    Construct_matrix(current_key, key_matrix);

    if (round == 0)
    {
        XOR_matrix(state_matrix, key_matrix);
    }
    if (round > 0 && round < 10)
    {
        // printf("Round: %d\n", round);
        XOR_matrix(state_matrix, key_matrix);
        InvMix_columns(state_matrix);
        InvShift_row(state_matrix);
        InvSubstitution_matrix(state_matrix);
        // Print_matrix(state_matrix);
    }
    if (round == 10)
    {

        // printf("Round: %d\n", round);
        XOR_matrix(state_matrix, key_matrix);

        InvSubstitution_matrix(state_matrix);
        InvShift_row(state_matrix);
        // Print_matrix(state_matrix);
    }
    Deconstruct_matrix(mes, state_matrix);
}
void Aes_main_handler(int option, const char *input_file, const char *output_file)
{
    input_size = Input_handler(&input, input_file);
    key_size = Input_handler(&key, "files/aesKey.txt");

    Initialize_output();
    unsigned char *message = malloc(output_size * sizeof(unsigned char));
    unsigned char *output_message = malloc(output_size * sizeof(unsigned char));
    unsigned char *first_key = malloc(key_size * sizeof(unsigned char));
    Convert_to_byte(input, input_size, message);
    Convert_to_byte(key, key_size, first_key);

    for (int i = input_size; i < output_size; i++)
    {
        message[i] = 0xFF;
        if (i == output_size - 1)
        {
            message[i] = (unsigned char)(output_size - input_size);
        }
    }

    Initialize_round_keys(first_key);

    for (int i = 0; i < 10; i++)
    {
        Make_round_keys(round_keys[i], round_keys[i + 1], 16, i + 1);
    }

    int nr = 1;

    while (nr <= output_size / 16)
    {
        unsigned char *aux = malloc(16 * sizeof(unsigned char));
        int c = 0;
        for (int k = 16 * (nr - 1); k < 16 * nr; k++)
        {
            aux[c] = message[k];
            c++;
        }
        if (option)
        {
            for (int i = 0; i < 11; i++)
            {
                Aes_algorithm(aux, round_keys[i], i);
            }
        }
        else
        {
            for (int i = 10; i >= 0; i--)
            {
                Aes_decrypt(aux, round_keys[i], i);
            }
        }

        c = 0;
        for (int k = 16 * (nr - 1); k < 16 * nr; k++)
        {
            output_message[k] = aux[c];
            c++;
        }

        nr++;
        free(aux);
    }

    if (option)
    {
        Output_Handler_aes(output_message, output_size, output_file, 1);
    }
    else
    {
        Output_Handler_aes(output_message, input_size, output_file, 0);
    }
    printf("%d \n", output_size);
    free(message);
    free(output_message);
    free(first_key);
}