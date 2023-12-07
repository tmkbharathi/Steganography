#include <stdio.h>
#include "decode.h"
#include "types.h"
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Read and Validate arguments
 * Input: CLA 3rd argument and DecodeInfo struct
 * Ouput: Store stego image
 * Return: e_success or e_failure on any error
 */
Status read_and_validate_decode_args(char **argv, DecodeInfo *decInfo)
{
	if(strchr(argv[2], '.') == NULL)
	{
		printf("INFO: Error: Pass the Encoded File with extension\n");
		return e_failure;
	}
	else if(!strcmp(strchr(argv[2],'.'),".bmp"))										//file would be in .bmp
	{
		decInfo->stego_image_fname=argv[2];
	}
	else
	{
		printf("INFO: Stego File Extension Wrong\n");
		return e_failure;
	}

	return e_success;
}

/* All Decoding functions are calling here and return to main function
 * Input: All functions
 * Ouput: Decode Successfully
 * Return: e_success or e_failure on ay error
 */
Status do_decoding(DecodeInfo *decInfo, char* argv[])
{
	printf("## ======== Decoding Procedure Started ======== ##\n");
	if(open_filds(decInfo) == e_success)
	{
		if(decode_magic_string(MAGIC_STRING, decInfo) == e_success)
		{
            printf("INFO: Done\n");
            if(decode_secret_file_extn_size(decInfo) == e_success)
			{
				printf("INFO: Done\n");
				if(decode_secret_file_extn(decInfo) == e_success)
				{
					printf("INFO: Done\n");
					char buf[20];
					if(argv[3] == NULL)
					{
					 	printf("INFO: Output File Not mentioned. Creating decoded%s\n", decInfo->extn_secret_file);
						strcpy(buf,"decoded");
						decInfo->secret_fname=buf;
					}
					else
					{
					     strcpy(buf,argv[3]);
					}
					decInfo->secret_fname=strcat(buf, decInfo->extn_secret_file);
					decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");

					if(decInfo->fptr_secret == NULL)
					{
                      fprintf(stderr, "Error while opening %s\n", decInfo->secret_fname );
					  return e_failure;
					}
					printf("INFO: Opened %s\n", decInfo->secret_fname);
					if(decode_secret_file_size(decInfo) == e_success)
					{
						printf("INFO: Done\n");
						if(decode_secret_file_data(decInfo) == e_success)
						{
							printf("INFO: Done\n");
						}
						else
						{
							printf("INFO: Decoding Secret File\n");
							return e_failure;
						}
					}
					else
					{
						printf("INFO: Decoding Secret File Size Failure\n");
						return e_failure;
					}
				}
				else
				{
					printf("INFO: Decoding Secret file extension failure\n");
					return e_failure;
				}
			}
			else
			{
				printf("INFO: Decoding Secret File extension Size failure\n");
				return e_failure;
			}
		}
		else
		{
			printf("INFO: Decoding Magic String Failure\n");
			return e_failure;
		}
	}
	else
	{
		printf("INFO: Opening Files Failed\n");
		return e_failure;
	}

    return e_success;
}

/* Opening files
 * Input:  Stego name stored in Decinfo struct
 * Output: Store fptr stego image by opening stego name
 * Return: e_success or e_failure on any failure
 */
Status open_filds(DecodeInfo *decInfo)
{
	printf("INFO: Opening required files\n");
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");				//Opening stego image
    
	if(decInfo->stego_image_fname == NULL)											//Do error handling
	{
     perror("fopen\n");
	 fprintf(stderr, "Unable to open the %s file\n", decInfo->stego_image_fname);
	 return e_failure;
	}
	printf("INFO: Opened %s\n", decInfo->stego_image_fname);

	return e_success;
}

/* Decoding magic string 
 * Input:  magic string and Decodeinfo struct
 * Output: decode first 2*8 bytes and store in data[2] 
 * after that check our magic string is equal to decoded magic string
 * Return: e_success or e_failure on any error
 */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
	printf("INFO: Decoding Magic String\n");
	char data[2];																	//arr of data to store decoded magicstring
	fseek(decInfo->fptr_stego_image, 54, SEEK_SET);									//now offset to 55th bit(ie. 0to54)
	if(decode_data_from_image(data, 2, decInfo->fptr_stego_image) == e_success)
	{
		if(strcmp(data, MAGIC_STRING))												//comparing magicstring not then failure
		{
			printf("INFO: Magic String Not Matched\n");
			return e_failure;
		}
	}
	return e_success;
}

/* decode_data_from_image
 * Input: magicstring, magicstring size and fptr stego image
 * Output: create imagebuffer[8] and read 8bytes from stego image
 * and store into imagebuffer and sent to decode_byte_from_lsb
 * Return: e_success (there is no error occurs)
 */
Status decode_data_from_image(char *data, int size, FILE *fptr_stego_image)
{
	char imagebuffer[8];
	for(int i=0; i<size; i++)
	{
	fread(&imagebuffer, 1, 8, fptr_stego_image);				//Read 8 bytes and store into imagebuffer
    data[i]=decode_byte_from_lsb(imagebuffer);                  //by calling and returned character store into data[2]
	}
	return e_success;
}

/* decode_byte_from_lsb
 * Input: imagebuffer[8] comes from decode_data_from_image
 * Output: now take all last lsb bytes and make one byte as out 
 * Return: out(character type)
 */
char decode_byte_from_lsb(char *image_buffer)
{   
	char out={0};
    for(int i=0; i<8; i++)
	{
     image_buffer[i]= image_buffer[i] & 1;					//clearing bits except last bit 
	 out=out<<1;											//left shifting out (msb to lsb)
	 out=out | image_buffer[i];								//appending out and bits
	}
	return out;
}

/* decode_secret_file_extn
 * Input: DecodeInfo struct
 * Output: decoding 32 bytes and make as int(32 bits) 
 * and store to struct member
 * Return: e_success or e_failure on file errors
 */
Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
	printf("INFO: Decoding Secret File Extension Size\n");
	char buffer[32]={0};
	int bufint=0;
	fread(&buffer, 1, 32, decInfo->fptr_stego_image);						//read 32bytes from stegoimage and store to buffer[32]
	if(ferror(decInfo->fptr_stego_image))
	{
		clearerr(decInfo->fptr_stego_image);
		fprintf(stderr,"Error: while opening %s file", decInfo->stego_image_fname);
		return e_failure;
	}
	for(int i=0; i<32; i++)
	{
      buffer[i]=buffer[i] & 1;							
	  bufint=bufint<<1;
	  bufint=bufint | buffer[i];											//32bytes last 1bits are appended to 32 bits as 1 int
	}
	decInfo->extn_size=bufint;
    return e_success;
}

/* decode_secret_file_extn
 * same as magic string decoding(because these also character)
 */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
	printf("INFO: Decoding Secret File Extension\n");
    char buffer[(decInfo->extn_size)+1];
	if(decode_data_from_image(buffer, decInfo->extn_size, decInfo->fptr_stego_image)== e_success)
	{
       buffer[decInfo->extn_size]=0;
       strcpy(decInfo->extn_secret_file, buffer);
	}
	else
	{
		return e_failure;
	}
	return e_success;
}
/* decode_secret_file_size
 * same as how we decode secret file extension size
 */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	printf("INFO: Decoding Secret File Size\n");
	char buffer[32]={0};
	int bufint=0;
	fread(&buffer, 1, 32, decInfo->fptr_stego_image);
	for(int i=0; i<32; i++)
	{
      buffer[i]=buffer[i] & 1;
	  bufint=bufint<<1;
	  bufint=bufint | buffer[i];
	}
	decInfo->size_secret_file=bufint;
    return e_success;
}

/* decode_secret_file_data
 * same as how we decoding characters in secret file extension 
 */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
	printf("INFO: Decoding Secret File Data\n");
	char image_buffer[8];
	char data;
    for(int i=0; i<decInfo->size_secret_file; i++)
	{
	   fread(&image_buffer, 1, 8, decInfo->fptr_stego_image);
	   data=decode_byte_from_lsb(image_buffer);
	   fputc(data, decInfo->fptr_secret);
	}

	fclose(decInfo->fptr_stego_image);					//this is last function, we decoded our message in secret_fname.txt 
	fclose(decInfo->fptr_secret);						//closing 2 files 
	return e_success;
}

