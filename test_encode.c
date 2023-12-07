#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc, char* argv[])
{
	if(check_operation_type(argv)==e_encode)
	{   
    	EncodeInfo STRUCT;										    	//check src file is .bmp, secret file, output file is passed 
    	if(argc==4 || argc==5)											//checking CLA argument is get passed inproper
		{
			if(read_and_validate_encode_args(argv, &STRUCT) == e_success)
			{
				printf("INFO: Read and Validate Success \n");
				if(do_encoding(&STRUCT) == e_success)
				{
					printf("## ======== Encoding Done Successfully ======== ##\n");
				}
				else
				{
					printf("## ======== Encoding Failed ======== ##\n");
					return e_failure;
				}
			}	
			else
			{	
				printf("INFO: Read and Validate Error\n");
				return e_failure;
			}
		}
		else
		{
			printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n", argv[0], argv[0]);
		}	
	}
    else if(check_operation_type(argv)==e_decode)
	{
		DecodeInfo STRUCT;
        if(argc==3 || argc==4)
		{
           if(read_and_validate_decode_args(argv, &STRUCT) == e_success)
		   {
			   printf("INFO: Read and Validate Success\n");
			   if(do_decoding(&STRUCT, argv) == e_success)
				   printf("## ======== Decoding Done Successfully ======== ##\n");
			   else
			   {
				   printf("## ======== Decoding Failed ======== ##\n");
				   return e_failure;
			   }
		   }
		   else
		   {
			   printf("INFO: Read and Validate Failure\n");
			   return e_failure;
		   }
		}
		else
		{
			printf("%s: Decoding: %s -e <.bmp file> [output file]\n", argv[0], argv[0]);
		}
	}

    else
       {
		   printf("%s: Encoding: %s -e <.bmp file> <.txt file> [output file]\n", argv[0], argv[0]);
		   printf("%s: Decoding: %s -d <.bmp file> [output file]\n", argv[0], argv[0]);    
	   }
    return 0;
}

