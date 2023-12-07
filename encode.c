
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */

/* 
 * For selecting, encode or decode 
 * Inputs: CLA vector (especially 2nd argument)
 * Output: Selecting one OperationType 
 * Return Value: e_encode or e_decode or e_unsupported
 */
OperationType check_operation_type(char *argv[])
{
	if (argv[1]==NULL)
		return e_unsupported;
	else if (!strcmp(argv[1],"-e"))
		return e_encode;
	else if (!strcmp(argv[1],"-d"))
	    return e_decode;
	else
		return e_unsupported;
}

/* 
 * Reading and Validate all arguments 
 * Inputs: CLA vector (3,4 and 5(opt) argument)
 * and structure ptr
 * Output: Store src, stego secret file name 
 * and secretfile extension in struct 
 * Return Value: e_success or e_failure on any error
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encinfo)
{
   if(strchr(argv[2],'.') == NULL)
	   return e_failure;
   else if(!(strcmp(strchr(argv[2], '.'),".bmp")))						//Storing Secret file 
	   encinfo-> src_image_fname= argv[2];
   else
	   return e_failure;
   
   if(strchr(argv[3],'.') == NULL)
   {
	   printf("INFO: Error: Pass the Valid Secret File\n");
	   return e_failure;
   }
   else if((!strcmp(strchr(argv[3],'.'),".txt")) || (!strcmp(strchr(argv[3],'.'),".c")) || (!strcmp(strchr(argv[3],'.'),".h"))||(!strcmp(strchr(argv[3],'.'),".sh")))
   {
       encinfo->secret_fname=argv[3];								//Storing secret file name using structure member	   

	   strcpy(encinfo->extn_secret_file, strchr(argv[3],'.'));		//Storing secret file extension
   }
   else
	   return e_failure;
   
   if(argv[4]!=NULL)
   {
		encinfo->stego_image_fname=argv[4];							//if 5th arg mentioned, store that stego image name	
   }
   else if(argv[4]==NULL)
   {
	    printf("INFO: Output File not mentioned.  Creating steged_img.bmp as default\n");
	    strcpy(encinfo->stego_image_fname,"steged_img");			//if not, store "steged_img" as default name
   }
   strcat(encinfo->stego_image_fname, ".bmp");						//finally concatenate .bmp in stegoimagename
   
   return e_success;
}

/* All our functions are called by here only and return to main function
 * Input: Struct only
 * Output: Successfully Encoding
 * Return Value: e_success or e_failure
 */
Status do_encoding(EncodeInfo *encInfo){

    if(open_files(encInfo) == e_success)
	{
		printf("INFO: Done\n## ======== Encoding Procedure Started ======== ##\n");
		if(check_capacity(encInfo) == e_success)
		{
			printf("INFO: Done. Found OK\n");
			if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)
			{
				printf("INFO: Done\n");
            	if(encode_magic_string(MAGIC_STRING, encInfo) == e_success)
				{
					printf("INFO: Done\n");                          
					if(encode_secret_file_extn_size(encInfo) == e_success)
					{
						printf("INFO: Done\n");
						if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
						{
							printf("INFO: Done\n");
							if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
							{
								printf("INFO: Done\n");
								if(encode_secret_file_data(encInfo) == e_success)
								{
									printf("INFO: Done\n");
									if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
									    printf("INFO: Done\n");
									else
									{
									    printf("INFO: Encoding Secret File Remaining Copy Failure\n");
									    return e_failure;
									}
								}
								else
								{
									printf("INFO: Encoding Secret File Data Failure\n");
									return e_failure;
								}
							}
							else
							{
								printf("INFO: Encoding Secret File Size Failure\n");
								return e_failure;
							}
						}
						else
						{  
							printf("INFO: Encoding Secret File Extension Failure\n");
							return e_failure;
						}
					}
					else
					{  
						printf("INFO: Encoding Secret File Extn Size Failure\n");
						return e_failure;
					}
				}
				else
				{				
					printf("INFO: Encoding Magic String Failure\n");
					return e_failure;
				}
			}
			else
			{
				printf("INFO: Failed\n");
				return e_failure;
			}
		}
		else
		{
			printf("INFO: Found Not\n");
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

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
	printf("INFO: Opening required files\n");									
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");				// Src Image file
    if (encInfo->fptr_src_image == NULL)										// Do Error handling
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }
	printf("INFO: Opened %s\n", encInfo->src_image_fname);

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");					// Secret file ptr
    if (encInfo->fptr_secret == NULL)											// Do Error handling
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }
	printf("INFO: Opened %s\n", encInfo->secret_fname);							 

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");			//Stego Image file ptr

    if (encInfo->fptr_stego_image == NULL)										//Do Error handling
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }
	printf("INFO: Opened %s\n", encInfo->stego_image_fname);

    return e_success;															// No failure return e_success
}

/*
 * Checking capacity: 1. Secret file have data or not
 * 2. Is all our data would be have encode to src image or not?
 * Inputs: get_file_size function and get_image_size_for_bmp function
 * Output: sizeofsecret file and size of source image
 * can be encode or not ? 
 * Return Value: e_success or e_failure on any error
 */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity= get_image_size_for_bmp(encInfo->fptr_src_image); 
	printf("INFO: Checking for %s size\n", encInfo->secret_fname);
	if((encInfo->size_secret_file = get_file_size(encInfo->fptr_secret)) > 1)   //Here we are checking secret file have data or not
	    {
			printf("INFO: Done. Not Empty\n");
		}  
	else
	 {	
		 printf("INFO: Empty Secret File\n");
		 return e_failure;
	 }
	
	printf("INFO: Checking for %s capacity to handle %s\n", encInfo->src_image_fname, encInfo->secret_fname);	

	/*Now Checking 5 different data can be store in image_capacity excluding headerfile or not (they are 1. magicstring 2.sizeof secret file extension 3. secret file extension 4. sizeof secretfile 5. secret file data )*/

    if((encInfo->image_capacity-54)>=((strlen(MAGIC_STRING)+4+strlen(encInfo->extn_secret_file)+4+ encInfo->size_secret_file)*8));
	else
	{   
		printf("INFO: %s file is very small to encode\n", encInfo->src_image_fname );
		return e_failure;
	}
	return e_success;
}

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    fseek(fptr_image, 18, SEEK_SET);						 	//In 18 and 19th position width and height are stored in bmp file

    fread(&width, sizeof(int), 1, fptr_image);
    printf("INFO: Getting Image Size:  width = %u, ", width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);
    rewind(fptr_image);

    return (width * height * BYTESPERPIXEL);
}

/* Get file size
 * Input: Secret file ptr
 * Output: Secret File Size
 * Description: In Secret file all are characters,
 * go to the last char, and use ftell to know its position
 * thats our size; (1char and EOF takes 2bytes)
 * in our case 25 bytes
 */
uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret,0,SEEK_END);								//now the offset set to last position in the secret file
	uint secretfilesize = ftell(fptr_secret); 
    rewind(fptr_secret);	

	return (secretfilesize);
}

/* Copying headerinfo of src image to stego image(0 to 53 bytes)
 * Input: file pts of src image and stego image
 * Output: copied 54bytes to stego image
 * Return: e_success or e_failure on any file errors
 */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    unsigned char buffer[54];
	fread(&buffer, sizeof(char), 54, fptr_src_image);		  //Directly copy 54 bytes to buffer[54]
	if (ferror(fptr_src_image))
	{
		fprintf(stderr, "INFO: Error while fetching data from Source Image");
		clearerr(fptr_src_image);
		return e_failure;
	}
	fwrite(&buffer, 1, 54, fptr_dest_image);				 //store buffer[54] to stego image(0 to 53 bytes) 

	if (ferror(fptr_dest_image))
	{
		fprintf(stderr, "INFO: Error while writing data to Destination Image");
		clearerr(fptr_src_image);
		return e_failure;
	}
	return e_success;
}

/* Encoding Magic string
 * Input: Magic string and EncodeInfo struct
 * Output: Size of magic string and sent all required
 * data to encode_data_to_image function
 * Return: e_success or e_failure on error occured on that fn.
 */
Status encode_magic_string(const char*magic_string, EncodeInfo *encInfo)
{
	int size=strlen(magic_string);
	if(encode_data_to_image((char*)magic_string, size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
         printf("INFO: Encoding Magic String Signature\n");
	else
	   {
	        return e_failure;
	   }  
   return e_success;
}

/* Encode data to image
 * Input: size how many data to encode and file ptrs and data to encode
 * Output: fetching 8 bytes from src image and goes to encode_byte_to_lsb
 * function and after puts that 8 bytes modified array to stego image
 * Return: e_success or e_failure on any errors
 */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	 char bufarr[MAX_IMAGE_BUF_SIZE];												//Create character array of 8 bytes
	 for(int i=0; i<size; i++)
	{
	 fread(bufarr, 1, MAX_IMAGE_BUF_SIZE, fptr_src_image);							//Read and store 8 bytes from the src image

	 if(encode_byte_to_lsb(data[i],bufarr) == e_success)
         fwrite(bufarr, 1, MAX_IMAGE_BUF_SIZE, fptr_stego_image);					//Store encoded 8bytes to bufarr   
     else
	 {
		fprintf(stderr,"INFO: Error in Encoding byte to lsb function\n");
		return e_failure;
	 }
	}

	return e_success;
}

/* Encoding our char 8 bits in 1 least lsb bit of 8 bytes 
 * Input: buffer[8] that has stego 8 bytes
 * Output: our 8 bits embedded in that buffer[8](i.e 8 bytes)
 * Return Value: e_success, there is no error would be happen 
 */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for(int i=0; i<MAX_IMAGE_BUF_SIZE; i++)
    {
        image_buffer[i]=image_buffer[i] & 0xFE;									//clearing lsb bit
        char bit=(data >> (7-i)) & 0x01;										//moving our data to least lsb (msb to lsb)
        image_buffer[i]=image_buffer[i] | bit;									//appending to stego image buffer[8]
    } 
    return e_success;
}

/*
 * Encoding Secret file extension Size
 * Inputs: Extension and EncodeInfo struct
 * Output: Store 32 bytes from source image and encode 32bits
 * to that 32 element array and now
 * store that 8 modified bytes to our stego image,
 * Return Value: e_success or e_failure on any error
 */
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{   
	printf("INFO: Encoding %s File Extension Size\n", encInfo->secret_fname);
	uint len=strlen(encInfo->extn_secret_file);
	uint sizebit= MAX_IMAGE_BUF_SIZE * sizeof(int);
	char data[sizebit];
	fread(&data, 1, 32, encInfo->fptr_src_image);								//int has 32 bits for that copy 32 bytes to data[32] from src image
	if (ferror(encInfo->fptr_src_image))
	{
 		fprintf(stderr,"Error: While Reading the data from Source Image File\n");
		clearerr(encInfo->fptr_src_image);
		return e_failure;
	}

	for(int i=0; i<sizebit; i++)
	{
        data[i]=data[i] & 0xFE;
        char bit=(len >> (31-i)) & 0x01;
        data[i]=data[i] | bit;

	}
	fwrite(&data, 1, 32, encInfo->fptr_stego_image);							//Store modified data[32] to stego image

	if (ferror(encInfo->fptr_stego_image))
	{
 		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
}

/*
 * Encoding Secret file extension Size
 * Inputs: Extension and EncodeInfo struct
 * Output: Store 8 bytes from source image and sent 
 * to the function of encode_byte_to_lsb after that 
 * returns store that 8 modified bytes to our stego image,
 * this run till according to our extion siz3
 * Return Value: e_success or e_failure on any error
 */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{   
	printf("INFO: Encoding %s File extension\n", encInfo->secret_fname);
	int fileext=strlen(file_extn);
    char buffer[MAX_IMAGE_BUF_SIZE];										//buffer that have size of 8
	for(int i=0; i<fileext; i++)
	{
		fread(&buffer, 1, MAX_IMAGE_BUF_SIZE, encInfo->fptr_src_image);						//Reading 8 bytes from source image
		if(encode_byte_to_lsb(file_extn[i], buffer) == e_success)
			fwrite(&buffer, 1, MAX_IMAGE_BUF_SIZE, encInfo->fptr_stego_image);				//After function, store modified buffer arr in stego img
		else
		{
			return e_failure; 
		}
	}
	return e_success;
}

/*
 * Encoding Secret file Size
 * Same as  Secret extension size
 */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Size\n", encInfo->secret_fname);
    uint len=file_size;
	uint sizebit= MAX_IMAGE_BUF_SIZE * sizeof(int);
	char data[sizebit];
	fread(&data, 1, 32, encInfo->fptr_src_image);

	if (ferror(encInfo->fptr_src_image))
	{
 		fprintf(stderr,"Error: While Reading the data from Source Image File\n");
		clearerr(encInfo->fptr_src_image);
		return e_failure;
	}
	
	for(int i=0; i<sizebit; i++)
	{
        data[i]=data[i] & 0xFE;
        char bit=(len >> (31-i)) & 0x01;
        data[i]=data[i] | bit;

	}
	fwrite(&data, 1, 32, encInfo->fptr_stego_image);

	if (ferror(encInfo->fptr_stego_image))
	{
 		fprintf(stderr,"Error: While writing the data to destination image file\n");
		clearerr(encInfo->fptr_stego_image);
		return e_failure;
	}
	return e_success;
} 

/*
 * Encoding the Secret file data
 * Its same as how we did secret file extension
 */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
	printf("INFO: Encoding %s File Data\n", encInfo->secret_fname); 
	char data[encInfo->size_secret_file];
	fread(data, sizeof(char), encInfo->size_secret_file, encInfo->fptr_secret);
	if (ferror(encInfo->fptr_secret))
	{
		fprintf(stderr,"Error: While reading the data to secret file\n");
		clearerr(encInfo->fptr_secret);
		return e_failure;
	}
	if (encode_data_to_image(data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
	{
		fprintf(stderr, "Error: %s function failed\n", "encode_data_to_image()");
		return e_failure;
	}
	return e_success;
}

/* Copying remaining data
 * Input: fptr src image and fptr stego image
 * Output: after encoding our secret file
 * copy remaining bytes to stego image for looks srcimage
 * if not done file would be corrupted because stego bmp header shows (width* height )*3
 * Return: e_success, e_failure on any file error
 */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	printf("INFO: Copying Left Over Data\n");
	int ch;
	while ((ch=getc(fptr_src))!=EOF)										//copying till EOF comes 
    {
		if (ferror(fptr_src))
		{
			fprintf(stderr, "INFO: Error unable to read from source image\n");
			clearerr(fptr_src);
			return e_failure;
		}
      	putc(ch, fptr_dest);												//storing that ch value in stego image
		if (ferror(fptr_dest))
		{
			fprintf(stderr, "INFO: Error unable to write to destination image\n");
			clearerr(fptr_dest);
			return e_failure;
		}
    }
	return e_success;
}
