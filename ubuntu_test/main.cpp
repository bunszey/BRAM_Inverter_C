
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include <iostream>
#include <string>
//#include "libs/platform.h"
//#include "libs/xil_printf.h"
#include "libs/xinverter.h"
#include "BRAM-uio-driver/src/bram_uio.h"


#define BRAMSIZE 125000
#define XST_FAILURE 1L

BRAM BRAM1(0,BRAMSIZE);
BRAM BRAM2(1,BRAMSIZE);

XInverter ip_inst;

void writing_()
{
    {
    	for (int i = 0; i < 25440; ++i) {
    		uint32_t data_write = 0;
			unsigned char* data_w_bytes = (unsigned char*)&data_write;

			data_w_bytes[0] = rand()%255;
			data_w_bytes[1] = rand()%255;
			data_w_bytes[2] = rand()%255;
			data_w_bytes[3] = rand()%255;

			BRAM1[i] = data_write;
			
		}
    }
}

void reading_()
{
    {
    	for (int i = 0; i < 25440; ++i) {
    		int data_read;
			data_read = BRAM2[i];

			unsigned char* data_r_bytes = (unsigned char*)&data_read;
		}
    }
}

void randomSample(){
	for (int i = 0; i < 10; ++i) {
		int data_read1, data_read2, k, j;
		k = rand()%25440;
		data_read1 = BRAM1[k];
		data_read2 = BRAM2[k];
		j = rand()%4;

		unsigned char* data_r_bytes1 = (unsigned char*)&data_read1;
		unsigned char* data_r_bytes2 = (unsigned char*)&data_read2;

		printf("Word,byte: %d,%d: in: %d out %d\r\n", k, j, data_r_bytes1[j], data_r_bytes2[j]);
	}


}

void difference(){
	int fails = 0;

	for (int i = 0; i < 25440; ++i) {
		int data_read1, data_read2;
		data_read1 = BRAM1[i];
		data_read2 = BRAM2[i];

		unsigned char* data_r_bytes1 = (unsigned char*)&data_read1;
		unsigned char* data_r_bytes2 = (unsigned char*)&data_read2;

		for (int j = 0; j < 4; ++j) {
			if (data_r_bytes2[j] != 255 - data_r_bytes1[j]) {
				++fails;
				printf("Fail on %d,%d: in: %d out %d\r\n", i, j, data_r_bytes1[j], data_r_bytes2[j]);
			}
		}
	}

	printf("Fails: %d\r\n", fails);
}

int main()
{
    //init_platform();
    printf("Hello World\n\r");

    char instance_name[ 20 ];
	sprintf(instance_name, "inverter");

    // Initialize the IP core
    int status = XInverter_Initialize(&ip_inst, instance_name);
	if (status != XST_SUCCESS) {
		printf("Error: Could not initialize the IP core.\n\r");
        	return XST_FAILURE;
	}




	writing_();
	XInverter_Start(&ip_inst);
	sleep(1);
	reading_();
	difference();
	randomSample();

	XInverter_DisableAutoRestart(&ip_inst);
    XInverter_Release(&ip_inst);
    printf("Successfully ran Hello World application\r\n");
    //cleanup_platform();
    return 0;
}


