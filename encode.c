#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

// Get the image size
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

// Get the secret file size
uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    uint size = 0;
    // Move file pointer to end //
    fseek(fptr, 0, SEEK_END);
    // Get current position (file size) //
    size = ftell(fptr);
    // Reset file pointer to beginning //
    fseek(fptr, 0, SEEK_SET);
    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

// Read and validate encode arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // check the argv[2] is having .bmp or not
    if (strstr(argv[2], ".bmp") != NULL)
    {
        // True: store into the structure member
        encInfo->src_image_fname = argv[2];
    }
    else
    {
        // False: print error message and return the e_failure
        printf("ERROR: Source image file should be .bmp\n");
        return e_failure;
    }

    // check the argv[3] is having .txt or not
    if (strstr(argv[3], ".txt") || strstr(argv[3], ".c") ||strstr(argv[3], ".h") || strstr(argv[3], ".sh") != NULL)
    {
        // True: store  into the structure member
        encInfo->secret_fname = argv[3];
    }
    else
    {
        // False: print error message and return the e_failure
        printf("ERROR: Secret image file should be .txt\n");
        return e_failure;
    }

    // check the argv[4] is having .bmp or not
    if (argv[4] != NULL)
    {
        if (strstr(argv[4], ".bmp") != NULL)
        {
            // True: store  into the structure member
            encInfo->dest_image_fname = argv[4];
        }
        else
        {
            // False: print error message and return the e_failure
            printf("ERROR: Destination image file should be .bmp\n");
            return e_failure;
        }
    }
    // if the argv[4] has not passed,set the default as .bmp
    else
    {
        encInfo->dest_image_fname = "dest.bmp";
    }
    // if all arguments are valid return e_success
    printf("Read and validate encode arugments function is successfull\n");
    return e_success;
}

// Open files for encoding
Status open_files_encode(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // dest Image file
    encInfo->fptr_dest_image = fopen(encInfo->dest_image_fname, "wb");
    // Do Error handling
    if (encInfo->fptr_dest_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->dest_image_fname);

        return e_failure;
    }

    // No failure return e_success
    printf("Open files sucessfully\n");
    return e_success;
}

// Check image capacity
Status check_capacity(EncodeInfo *encInfo)
{
    // checking image capacity by calling the get_image_size_for_bmp function
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    // storing the file size into the size_secret_file
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);

    // checking the image capacity
    if (encInfo->image_capacity > (16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8)))
    {
        // True: return e_success
        printf("Check capacity is successfull\n");
        return e_success;
    }
    else
    {
        // False: return e_failure
        return e_failure;
    }
}

Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    char imageBuffer[54];
    // rewind the source file pointer
    rewind(fptr_src_image);

    // read 54 bytes from source image //
    if (fread(imageBuffer, 1, 54, fptr_src_image) != 54)
    {
        return e_failure;
    }
    // write 54 bytes to destination image //
    if (fwrite(imageBuffer, 1, 54, fptr_dest_image) != 54)
    {
        return e_failure;
    }
    // check file pointer offsets //
    if (ftell(fptr_src_image) == ftell(fptr_dest_image))
    {
        printf("Copy bmp header is successfull\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Encode magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    int i = 0;
    for (i = 0; magic_string[i] != '\0'; i++)
    {
        // read 8 bytes and store to source image
        if (fread(imageBuffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }

        //  encode_byte_to_lsb() function call
        encode_byte_to_lsb(magic_string[i], imageBuffer);

        // write 8 bytes and store to destination image
        if (fwrite(imageBuffer, 1, 8, encInfo->fptr_dest_image) != 8)
        {
            return e_failure;
        }
    }

    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Encoded magic string successfully\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Encode secret file extension size
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    // Encode each extension character
    if (fread(imageBuffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }
    encode_size_to_lsb(size, imageBuffer);
    if (fwrite(imageBuffer, 1, 32, encInfo->fptr_dest_image) != 32)
    {
        return e_failure;
    }
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Encoded secret file extension size successfully\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Encode secret file extension
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char imageBuffer[8];
    int i = 0;
    for (i = 0; file_extn[i] != '\0'; i++)
    {
        if (fread(imageBuffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }
        encode_byte_to_lsb(file_extn[i], imageBuffer);
        if (fwrite(imageBuffer, 1, 8, encInfo->fptr_dest_image) != 8)
        {
            return e_failure;
        }
    }
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Encoded secret file extension succesfully\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char imageBuffer[32];
    if (fread(imageBuffer, 1, 32, encInfo->fptr_src_image) != 32)
    {
        return e_failure;
    }
    encode_size_to_lsb(file_size, imageBuffer);
    if (fwrite(imageBuffer, 1, 32, encInfo->fptr_dest_image) != 32)
    {
        return e_failure;
    }
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_dest_image))
    {
        printf("Encoded secret file size successfully\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{

    char secret_file_data;
    char imageBuffer[8];

    // Rewind secret file pointer //
    rewind(encInfo->fptr_secret);

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        // Read one byte from secret file //
        if (fread(&secret_file_data, 1, 1, encInfo->fptr_secret) != 1)
        {
            return e_failure;
        }
        // Read 8 bytes from source image //
        if (fread(imageBuffer, 1, 8, encInfo->fptr_src_image) != 8)
        {
            return e_failure;
        }
        // Encode secret byte into image buffer //
        encode_byte_to_lsb(secret_file_data, imageBuffer);
        // Write modified buffer to destination image //
        if (fwrite(imageBuffer, 1, 8, encInfo->fptr_dest_image) != 8)
        {
            return e_failure;
        }
    }
    if (ftell(encInfo->fptr_secret) == encInfo->size_secret_file) // Check file offsets //
    {
        printf("Encoded secret file data successfully\n");
        return e_success;
    }

    return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char buffer;
    // Copy remaining data byte by byte //
    while (fread(&buffer, 1, 1, fptr_src) == 1)
    {
        fwrite(&buffer, 1, 1, fptr_dest);
    }
    // Check offsets //
    if (ftell(fptr_src) == ftell(fptr_dest))
    {
        printf("Copied remaining data successfully\n");
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        // Get bit from data (MSB first) //
        int bit = (data >> (7 - i)) & 1;
        // Clear LSB of image_buffer[i] //
        image_buffer[i] = image_buffer[i] & 0xFE;
        // Set the bit into LSB //
        image_buffer[i] = image_buffer[i] | bit;
    }

    return e_success;
}

Status encode_size_to_lsb(int size, char *imageBuffer)
{
    for (int i = 0; i < 32; i++)
    {
        // Get the MSB first //
        int bit = (size >> (31 - i)) & 1;
        // Clear LSB of imageBuffer[i] //
        imageBuffer[i] = imageBuffer[i] & 0xFE;
        // Set the bit into LSB //
        imageBuffer[i] = imageBuffer[i] | bit;
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    printf("Selected Encoding\n");
    // Open files //
    {
        if (open_files_encode(encInfo) != e_success)
            return e_failure;
    }

    // Check capacity //
    if (check_capacity(encInfo) != e_success)
    {
        printf("ERROR: Insufficient image capacity\n");
        return e_failure;
    }

    // Copy BMP header //
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_dest_image) != e_success)
    {
        return e_failure;
    }

    // Encode magic string //
    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
    {
        return e_failure;
    }

    // Get secret file extension //
    char *extn_secret_file = strchr(encInfo->secret_fname, '.');

    // Find extension length //
    int extn_size = strlen(extn_secret_file);

    // Encode extension size //
    {
        if (encode_secret_file_extn_size(extn_size, encInfo) != e_success)
            return e_failure;
    }

    // Encode extension //
    if (encode_secret_file_extn(extn_secret_file, encInfo) != e_success)
    {
        return e_failure;
    }

    // Encode secret file size //
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
    {
        return e_failure;
    }

    // Encode secret file data //
    if (encode_secret_file_data(encInfo) != e_success)
    {
        return e_failure;
    }

    // Copy remaining image data //
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_dest_image) != e_success)
    {
        return e_failure;
    }

    // Close all files //
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_dest_image);
    return e_success;
}