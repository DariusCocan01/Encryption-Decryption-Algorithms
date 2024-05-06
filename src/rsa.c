#include "rsa.h"

struct Key
{
    int x;
    int y;
} public_key, private_key;

static char *input = NULL;
static int input_size;

void WriteToFile(const char *output_file, long long *output_message, int size, int option)
{
    FILE *file = fopen(output_file, option ? "w" : "wb");
    if (!file)
    {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    if (option)
    {
        for (int i = 0; i < size; i++)
        {
            fprintf(file, "%lld ", output_message[i]);
        }
        fprintf(file, "\n");
    }
    else
    {
        for (int i = 0; i < size; i++)
        {
            while (output_message[i] > 0)
            {
                fprintf(file, "%c", (unsigned char)(output_message[i] & 0xFF));
                output_message[i] >>= 8;
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

int Coprime(int x, int y)
{
    while (y != 0)
    {
        int temp = y;
        y = x % y;
        x = temp;
    }
    return x == 1;
}

int D_value(int e, int phi)
{

    int a = phi;
    int b = e;

    int x0 = 0;
    int x1 = 1;
    while (b != 0)
    {
        int quotient = a / b;
        int temp = a % b;
        a = b;
        b = temp;
        int x2 = x0 - quotient * x1;
        x0 = x1;
        x1 = x2;
    }

    if (x0 < 0)
    {
        x0 += phi;
    }

    return x0;
}

long long Exp(long long base, long long exp, long long mod)
{
    long long result = 1;
    base = base % mod;
    while (exp > 0)
    {
        if (exp % 2 == 1)
            result = (result * base) % mod;
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

static int Input_handler_rsa(char **input, const char *input_file)
{
    FILE *file = fopen(input_file, "rb");
    if (!file)
    {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);
    rewind(file);

    *input = malloc(size);
    fread(*input, 1, size, file);
    fclose(file);

    return size;
}

void Convert_to_long_long(unsigned char *message, int input_size, long long *output_message, int chunk_size)
{
    int index = 0;
    for (int i = 0; i < input_size; i += chunk_size)
    {
        long long combined = 0;
        for (int j = 0; j < chunk_size && i + j < input_size; j++)
        {
            combined = (combined << 8) | message[i + j];
        }
        output_message[index++] = combined;
    }
}

void Convert_back_to_chars(long long *input_message, int num_chunks, unsigned char *output_message, int chunk_size)
{
    int index = 0;
    for (int i = 0; i < num_chunks; i++)
    {
        long long combined = input_message[i];
        for (int j = chunk_size - 1; j >= 0 && index < num_chunks * chunk_size; j--)
        {
            output_message[index++] = (combined >> (8 * j)) & 0xFF;
        }
    }
}
void Decryption(const char *input_file, const char *of, long long exp, long long mod)
{
    FILE *file, *output_file;
    long long num;

    file = fopen(input_file, "r");
    if (file == NULL)
    {
        printf("Unable to open input file\n");
    }
    output_file = fopen(of, "w");
    if (output_file == NULL)
    {
        printf("Unable to open output file\n");
        fclose(file);
    }

    while (fscanf(file, "%lld", &num) != EOF)
    {
        int x = Exp(num, exp, mod);
        fputc(x, output_file);
    }

    fclose(file);
    fclose(output_file);
}
void Rsa_Main_Handler(int option, const char *input_file, const char *output_file)
{
    input_size = Input_handler_rsa(&input, input_file);
    unsigned char *message = malloc(input_size * sizeof(unsigned char));
    memcpy(message, input, input_size);

    int p = 61;
    int q = 53;

    int n = p * q;
    int phi = (p - 1) * (q - 1);
    int e = 17;

    if (!Coprime(e, phi))
    {
        e = 65537;
    }

    int d = D_value(e, phi);

    public_key.x = e;
    public_key.y = n;
    private_key.x = d;
    private_key.y = n;

    int chunk_size = 1;
    while ((1LL << (8 * chunk_size)) <= n && chunk_size <= sizeof(long long))
    {
        chunk_size++;
    }
    chunk_size--;

    int num_chunks = (input_size + chunk_size - 1) / chunk_size;
    long long *output_message = malloc(num_chunks * sizeof(long long));

    if (option)
    {
        Convert_to_long_long(message, input_size, output_message, chunk_size);
        for (int i = 0; i < num_chunks; i++)
        {
            output_message[i] = Exp(output_message[i], public_key.x, public_key.y);
        }
        WriteToFile(output_file, output_message, num_chunks, 1);
    }
    else
    {

        Decryption(input_file, output_file, private_key.x, private_key.y);
    }

    free(message);
    free(output_message);
}
