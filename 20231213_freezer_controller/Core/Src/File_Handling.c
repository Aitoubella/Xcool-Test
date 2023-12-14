

#include "File_Handling.h"
#include "printf.h"
/* =============================>>>>>>>> NO CHANGES AFTER THIS LINE =====================================>>>>>>> */



extern char USBHPath[4];   /* USBH logical drive path */
extern FATFS USBHFatFS;    /* File system object for USBH logical drive */
extern FIL USBHFile;       /* File object for USBH */

FILINFO USBHfno;
FRESULT fresult;  // result
UINT br, bw;  // File read/write count

/**** capacity related *****/
FATFS *pUSBHFatFS;
DWORD fre_clust;
uint32_t total, free_space;

uint8_t usb_buffer[MAX_USB_BUFFER];
static char path[257];
void Send_Uart (char *string)
{
	printf("%s");
}


void Mount_USB (void)
{
	fresult = f_mount(&USBHFatFS, USBHPath, 1);
	if (fresult != FR_OK) Send_Uart ("\nERROR!!! in mounting USB ...");
	else Send_Uart("\nUSB mounted successfully...");
}

void Unmount_USB (void)
{
	fresult = f_mount(NULL, USBHPath, 1);
	if (fresult == FR_OK) Send_Uart ("\nUSB UNMOUNTED successfully...");
	else Send_Uart("\nERROR!!! in UNMOUNTING USB ");
}

/* Start node to be scanned (***also used as work area***) */
FRESULT Scan_USB (char* pat)
{
    DIR dir;
    UINT i;
    sprintf (path, "%s",pat);

    fresult = f_opendir(&dir, path);                       /* Open the directory */
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &USBHfno);                   /* Read a directory item */
            if (fresult != FR_OK || USBHfno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (USBHfno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", USBHfno.fname))) continue;
            	if (!(strcmp("System Volume Information", USBHfno.fname))) continue;
            	printf ("Dir: %s\r\n", USBHfno.fname);
                i = strlen(path);
                sprintf(&path[i], "/%s", USBHfno.fname);
                fresult = Scan_USB(path);                     /* Enter the directory */
                if (fresult != FR_OK) break;
                path[i] = 0;
            }
            else
            {   /* It is a file. */
               printf("File: %s/%s\n", path, USBHfno.fname);
            }
        }
        f_closedir(&dir);
    }
    return fresult;
}

/* Only supports removing files from home directory */
FRESULT Format_USB (void)
{
    DIR dir;
    char *path = malloc(20*sizeof (char));
    sprintf (path, "%s","/");

    fresult = f_opendir(&dir, path);                       /* Open the directory */
    if (fresult == FR_OK)
    {
        for (;;)
        {
            fresult = f_readdir(&dir, &USBHfno);                   /* Read a directory item */
            if (fresult != FR_OK || USBHfno.fname[0] == 0) break;  /* Break on error or end of dir */
            if (USBHfno.fattrib & AM_DIR)     /* It is a directory */
            {
            	if (!(strcmp ("SYSTEM~1", USBHfno.fname))) continue;
            	if (!(strcmp("System Volume Information", USBHfno.fname))) continue;
            	fresult = f_unlink(USBHfno.fname);
            	if (fresult == FR_DENIED) continue;
            }
            else
            {   /* It is a file. */
               fresult = f_unlink(USBHfno.fname);
            }
        }
        f_closedir(&dir);
    }
    free(path);
    return fresult;
}




FRESULT Write_File (char *name, char *data)
{

	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
		printf ("\nERROR!!! *%s* does not exists", name);
	    return fresult;
	}

	else
	{
	    /* Create a file with read write access and open it */
	    fresult = f_open(&USBHFile, name, FA_OPEN_EXISTING | FA_WRITE);
	    if (fresult != FR_OK)
	    {
	    	printf("\nERROR!!! No. %d in opening file *%s*", fresult, name);
	        return fresult;
	    }

	    else
	    {
	    	printf("\nOpening file-->  *%s*  To WRITE data in it\n", name);
	    	fresult = f_write(&USBHFile, data, strlen(data), &bw);
	    	if (fresult != FR_OK)
	    	{
	    		printf ("\nERROR!!! No. %d while writing to the FILE *%s*", fresult, name);
	    	}

	    	/* Close file */
	    	fresult = f_close(&USBHFile);
	    	if (fresult != FR_OK)
	    	{
	    		printf ("\nERROR!!! No. %d in closing file *%s* after writing it", fresult, name);
	    	}
	    	else
	    	{
	    		printf ("\nFile *%s* is WRITTEN and CLOSED successfully", name);
	    	}
	    }
	    return fresult;
	}
}

FRESULT Read_File (char *name)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
		printf ("\nERRROR!!! *%s* does not exists", name);
	    return fresult;
	}

	else
	{
		/* Open file to read */
		fresult = f_open(&USBHFile, name, FA_READ);
		if (fresult != FR_OK)
		{
			printf ("\nERROR!!! No. %d in opening file *%s*", fresult, name);
		    return fresult;
		}

		/* Read data from the file
		* see the function details for the arguments */

    	printf("\nOpening file-->  *%s*  To READ data from it\n", name);


		char *buffer = malloc(sizeof(f_size(&USBHFile)));
		fresult = f_read (&USBHFile, buffer, f_size(&USBHFile), &br);
		if (fresult != FR_OK)
		{
			free(buffer);
		 	printf("\nERROR!!! No. %d in reading file *%s*", fresult, name);
		  	Send_Uart(buffer);
		}
		else
		{
			Send_Uart(buffer);
			free(buffer);

			/* Close file */
			fresult = f_close(&USBHFile);
			if (fresult != FR_OK)
			{
				printf("\nERROR!!! No. %d in closing file *%s*", fresult, name);
			}
			else
			{
				printf("\nFile *%s* CLOSED successfully", name);
			}
		}
	    return fresult;
	}
}

FRESULT Create_File (char *name)
{
	fresult = f_stat (name, &USBHfno);
	if (fresult == FR_OK)
	{
		printf("\nERROR!!! *%s* already exists!!!!\n use Update_File ",name);
	    return fresult;
	}
	else
	{
		fresult = f_open(&USBHFile, name, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);
		if (fresult != FR_OK)
		{
			printf ("\nERROR!!! No. %d in creating file *%s*", fresult, name);
		    return fresult;
		}
		else
		{
			printf("\n*%s* created successfully\n Now use Write_File to write data\n",name);
		}

		fresult = f_close(&USBHFile);
		if (fresult != FR_OK)
		{
			printf("\nERROR No. %d in closing file *%s*", fresult, name);
		}
		else
		{
			printf("\nFile *%s* CLOSED successfully", name);
		}
	}
    return fresult;
}

FRESULT Update_File (char *name, char *data)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
		printf("\nERROR!!! *%s* does not exists", name);
	    return fresult;
	}

	else
	{
		 /* Create a file with read write access and open it */
	    fresult = f_open(&USBHFile, name, FA_OPEN_APPEND | FA_WRITE);
	    if (fresult != FR_OK)
	    {

	    	printf("\nERROR!!! No. %d in opening file *%s*", fresult, name);
	        return fresult;
	    }
	    printf("\nOpening file-->  *%s*  To UPDATE data in it\n", name);

	    /* Writing text */
	    fresult = f_write(&USBHFile, data, strlen (data), &bw);
	    if (fresult != FR_OK)
	    {
	    	printf("\nERROR!!! No. %d in writing file *%s*", fresult, name);
	    }

	    else
	    {
	    	printf("\n*%s* UPDATED successfully\n", name);
	    }

	    /* Close file */
	    fresult = f_close(&USBHFile);
	    if (fresult != FR_OK)
	    {
	    	printf("\nERROR!!! No. %d in closing file *%s*", fresult, name);
	    }
	    else
	    {
	    	printf("\nFile *%s* CLOSED successfully", name);
	     }
	}
    return fresult;
}

FRESULT Remove_File (char *name)
{
	/**** check whether the file exists or not ****/
	fresult = f_stat (name, &USBHfno);
	if (fresult != FR_OK)
	{
		printf("\nERROR!!! *%s* does not exists", name);
		return fresult;
	}

	else
	{
		fresult = f_unlink (name);
		if (fresult == FR_OK)
		{
			printf ("\n*%s* has been removed successfully", name);
		}

		else
		{
			printf("\nERROR No. %d in removing *%s*",fresult, name);
		}
	}
	return fresult;
}

FRESULT Create_Dir (char *name)
{
    fresult = f_mkdir(name);
    if (fresult == FR_OK)
    {
    	printf ("\n*%s* has been created successfully", name);
    }
    else
    {
    	printf("\nERROR No. %d in creating directory *%s*", fresult,name);
    }
    return fresult;
}

void Check_USB_Details (void)
{
    /* Check free space */
    f_getfree("", &fre_clust, &pUSBHFatFS);

    total = (uint32_t)((pUSBHFatFS->n_fatent - 2) * pUSBHFatFS->csize * 0.5);
    printf ("\nUSB  Total Size: \t%lu\n",total);
    free_space = (uint32_t)(fre_clust * pUSBHFatFS->csize * 0.5);
    printf ("\nUSB Free Space: \t%lu\n",free_space);

}

FRESULT copy_file(char* dest ,char* src)
{
	 FIL fsrc, fdst;      /* Work area (filesystem object) for logical drives */
	 FRESULT fr = FR_OK;          /* FatFs function common result code */
	 fr = f_open(&fsrc, src, FA_READ);
	 if (fr) return (int)fr;

	 /* Create destination file on the drive 0 */
	fr = f_open(&fdst, dest, FA_WRITE | FA_CREATE_ALWAYS);
	if (fr) return (int)fr;

	/* Copy source to destination */
	for (;;) {
		fr = f_read(&fsrc, usb_buffer, MAX_USB_BUFFER , &br);  /* Read a chunk of source file */
		if (fr || br == 0) break; /* error or eof */
		fr = f_write(&fdst, usb_buffer, br, &bw);            /* Write it to the destination file */
		if (fr || bw < br) break; /* error or disk full */
	}

	/* Close open files */
	f_close(&fsrc);
	f_close(&fdst);
	return fr;
}

