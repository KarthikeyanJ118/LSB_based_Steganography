#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h"

typedef struct _DecodeInfo
{
    // Secret File Info 
    char extn_secret_file[5]; //  Secret file extension
    long size_secret_file;    //  secret file size

    // dest Image Info 
    char *dest_image_fname; // To store the dest file name
    FILE *fptr_dest_image;  // To store the address of dest image

    // Output File Info
    char *output_fname;
    FILE *fptr_output_file;

} DecodeInfo;

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/*Encode extension size*/
Status decode_secret_file_extn_size(DecodeInfo *decInfo, int *extn_size);

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo, int extn_size);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode a byte into LSB of image data array */
char decode_lsb_to_byte(char *image_buffer);

// Encode a size to lsb
long decode_lsb_to_size(char *imageBuffer);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

#endif