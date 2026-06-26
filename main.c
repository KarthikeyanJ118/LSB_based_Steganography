// NAME: KARTHIKEYAN J
// PROJECT: STEGANOGRAPHY 

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
OperationType check_operation_type(char *);

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("ERROR: Pass the sufficent number of arguments,Please pass\n");
        return 0;
    }

    // Encoding //
    if (strcmp(argv[1], "-e") == 0)
    {
        EncodeInfo encInfo;

        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            if (do_encoding(&encInfo) == e_success)
            {
                printf("Encoding  done successfully \n");
            }
            else
            {
                printf("Encoding failed \n");
            }
        }
        else
        {
            printf("Invalid encoding arguments\n");
        }
    }

    // Decoding //
    else if (strcmp(argv[1], "-d") == 0)
    {
        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            if (do_decoding(&decInfo) == e_success)
            {
                printf("Decoding done successfully \n");
            }
            else
            {
                printf("Decoding failed \n");
            }
        }
        else
        {
            printf("Invalid decoding arguments\n");
        }
    }

    else
    {
        printf("ERROR: Invalid option\n");
        printf("Use -e for encoding or -d for decoding\n");
    }
}
OperationType check_operation_type(char *symbol)
{
    if (strcmp(symbol, "-e") == 0)
    {
        return e_encode;
    }
    else if (strcmp(symbol, "-d") == 0)
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;
    }
}