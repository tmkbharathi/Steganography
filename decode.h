#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Stego Image info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
    long size_secret_file;
	long extn_size;

} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo, char *argv[]);

/* Get File pointers for i/p and o/p files */
Status open_filds(DecodeInfo *decInfo);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* decode secret file file extension size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* decode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image);

/* Decode a byte from LSB of image data array */
char decode_byte_from_lsb(char *image_buffer);

#endif
