#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

// Read and validate arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check destination image argument
    if (argv[2] == NULL)
    {
        printf("ERROR: Destination image is not provided\n");
        return e_failure;
    }
    // Check for .bmp file
    if (strstr(argv[2], ".bmp") != NULL)
    {
        decInfo->dest_image_fname = argv[2];
    }
    else
    {
        printf("ERROR: Destination image file should be .bmp\n");
        return e_failure;
    }
    // Store output file name
    if (argv[3] != NULL)
    {
        decInfo->output_fname = argv[3];
    }
    else
    {
        decInfo->output_fname = "decoded_output";
    }
    printf("Read and validate decode arguments successfully\n");
    return e_success;
}

// Opening files for decoding
Status open_files_decode(DecodeInfo *decInfo)
{
    // open destination image file in read mode
    decInfo->fptr_dest_image = fopen(decInfo->dest_image_fname, "rb");

    // Error checking
    if (decInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        printf("Error: Unable to open the destnation image file\n");
        return e_failure;
    }
    printf("Opened files for decoding successfully\n");
    return e_success;
}

// Decode magic string
Status decode_magic_string(DecodeInfo *decInfo)
{
    char imageBuffer[8];
    char ch;
    for (int i = 0; i < strlen(MAGIC_STRING); i++)
    {
        if (fread(imageBuffer, 1, 8, decInfo->fptr_dest_image) != 8)
        {
            printf("ERROE: unexpected end of file\n");
            return e_failure;
        }
        ch = decode_lsb_to_byte(imageBuffer);
        if (ch != MAGIC_STRING[i])
        {
            printf("ERROR: Magic string is mismatched\n");
            return e_failure;
        }
    }
    printf("Decoded magic string successfully\n");
    return e_success;
}

// Decode secret file extension size
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size)
{
    char imageBuffer[32];
    if (fread(imageBuffer, 1, 32, decInfo->fptr_dest_image) != 32)
    {
        printf("ERROE: unexpected end of file\n");
        return e_failure;
    }
    *extn_size = decode_lsb_to_size(imageBuffer);
    if (*extn_size > 4)
    {
        printf("ERROR: invalid extension size\n");
        return e_failure;
    }
    printf("Decoded extension size successfully\n");
    return e_success;
}

// Decode secret file extension
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size)
{
    char imageBuffer[8];
    for (int i = 0; i < extn_size; i++)
    {
        if (fread(imageBuffer, 1, 8, decInfo->fptr_dest_image) != 8)
        {
            printf("ERROE: unexpected end of file\n");
            return e_failure;
        }
        decInfo->extn_secret_file[i] = decode_lsb_to_byte(imageBuffer);
    }
    decInfo->extn_secret_file[extn_size] = '\0';
    printf("Decoded file extension successfully\n");
    return e_success;
}

// Decode secret file size
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char imageBuffer[32];
    if (fread(imageBuffer, 1, 32, decInfo->fptr_dest_image) != 32)
    {
        printf("ERROE: unexpected end of file\n");
        return e_failure;
    }
    decInfo->size_secret_file = decode_lsb_to_size(imageBuffer);
    printf("Decoded secret file size successfully\n");
    return e_success;
}

// Decode secret file data
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char imageBuffer[8];
    char ch;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        if (fread(imageBuffer, 1, 8, decInfo->fptr_dest_image) != 8)
        {
            printf("ERROE: unexpected end of file\n");
            return e_failure;
        }
        ch = decode_lsb_to_byte(imageBuffer);
        fwrite(&ch, 1, 1, decInfo->fptr_output_file);
    }
    printf("Decoded secret file successfully\n");
    return e_success;
}

// convert LSB bits to character
char decode_lsb_to_byte(char *imageBuffer)
{
    char info = 0;
    // Extract 1 LSB from each byte
    for (int i = 0; i < 8; i++)
    {
        info = info << 1;
        info = info | (imageBuffer[i] & 1);
    }
    return info;
}

// Convert LSB bits to size
long decode_lsb_to_size(char *imageBuffer)
{
    long size = 0;
    // Extract 32 LSB bits
    for (int i = 0; i < 32; i++)
    {
        size = size << 1;
        size = size | (imageBuffer[i] & 1);
    }
    return size;
}

// Do complete decoding
Status do_decoding(DecodeInfo *decInfo)
{
    int extn_size;
    printf("Entered into decoding function\n");
    // Open destination image
    if (open_files_decode(decInfo) != e_success)
    {
        return e_failure;
    }
    // skip BMP header
    fseek(decInfo->fptr_dest_image, 54, SEEK_SET);

    // Decode magic string
    if (decode_magic_string(decInfo) != e_success)
    {
        return e_failure;
    }

    // Decode secret file extension size
    if (decode_secret_file_extn_size(decInfo, &extn_size) != e_success)
    {
        return e_failure;
    }

    // Decode secret file extension
    decode_secret_file_extn(decInfo, extn_size);

    // Output file name
    char output_file[100];
    strcpy(output_file, decInfo->output_fname);

    // Remove user given extension (if any)
    char *dot = strchr(output_file, '.');
    if (dot != NULL)
    {
        *dot = '\0';
    }

    // Add decoded extension
    strcat(output_file, decInfo->extn_secret_file);

    // open output file
    decInfo->fptr_output_file = fopen(output_file, "wb");
    if (decInfo->fptr_output_file == NULL)
    {
        return e_failure;
    }

    // Decode secret file size
    decode_secret_file_size(decInfo);

    // Decode secret file data
    decode_secret_file_data(decInfo);

    // close all
    fclose(decInfo->fptr_dest_image);
    fclose(decInfo->fptr_output_file);
    return e_success;
}