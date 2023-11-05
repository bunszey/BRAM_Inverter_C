/******************************************************************************
*
* Copyright (C) 2009 - 2014 Xilinx, Inc.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* Use of the Software is limited solely to applications:
* (a) running on a Xilinx device, or
* (b) that interact with a Xilinx device through a bus or interconnect.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
* XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
* OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Except as contained in this notice, the name of the Xilinx shall not be used
* in advertising or otherwise to promote the sale, use or other dealings in
* this Software without prior written authorization from Xilinx.
*
******************************************************************************/

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //Header file for sleep(). man 3 sleep for details.
#include <pthread.h>
#include "platform.h"
#include "xil_printf.h"
#include "xbram.h"
#include "xinverter.h"

#define BRAM1(A) ((volatile u32*)px_config1->MemBaseAddress)[A]
#define BRAM2(A) ((volatile u32*)px_config2->MemBaseAddress)[A]

XBram x_bram1;
XBram_Config *px_config1;
XBram x_bram2;
XBram_Config *px_config2;

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

			BRAM1(i) = data_write;
			;
		}
    }
}

void reading_()
{
    {
    	for (int i = 0; i < 25440; ++i) {
    		int data_read;
			data_read = BRAM2(i);
			//printf("%d\r\n", data_read);
			unsigned char* data_r_bytes = (unsigned char*)&data_read;

		}
    }
}

void randomSample(){
	for (int i = 0; i < 10; ++i) {
		int data_read1, data_read2, k, j;
		k = rand()%25440;
		data_read1 = BRAM1(k);
		data_read2 = BRAM2(k);
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
		data_read1 = BRAM1(i);
		data_read2 = BRAM2(i);

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
    init_platform();
    print("Hello World\n\r");

    XInverter ip_inst;
    XInverter_Config *ip_cfg;

    // Initialize the IP core
    ip_cfg = XInverter_LookupConfig(XPAR_XINVERTER_0_DEVICE_ID);
    if (ip_cfg == NULL) {
        printf("Error: Could not find the IP core configuration.\n");
        return XST_FAILURE;
    }

    int status = XInverter_CfgInitialize(&ip_inst, ip_cfg);
    if (status != XST_SUCCESS) {
        printf("Error: Could not initialize the IP core.\n");
        return XST_FAILURE;
    }


    px_config1 = XBram_LookupConfig(XPAR_BRAM_0_DEVICE_ID);
	if (px_config1 == (XBram_Config *) NULL) {
		printf("Failed XBram_LookupConfig1\r\n");
		return XST_FAILURE;
	}

	status = XBram_CfgInitialize(&x_bram1, px_config1, px_config1->CtrlBaseAddress);
	if (status != XST_SUCCESS) {
		printf("Failed XBram_CfgInitialize1\r\n");
		return XST_FAILURE;
	}

	px_config2 = XBram_LookupConfig(XPAR_BRAM_1_DEVICE_ID);
	if (px_config2 == (XBram_Config *) NULL) {
		printf("Failed XBram_LookupConfig2\r\n");
		return XST_FAILURE;
	}

	status = XBram_CfgInitialize(&x_bram2, px_config2, px_config2->CtrlBaseAddress);
	if (status != XST_SUCCESS) {
		printf("Failed XBram_CfgInitialize2\r\n");
		return XST_FAILURE;
	}

	printf("BRAM initialization complete.\r\n");



	writing_();
	XInverter_Start(&ip_inst);
	sleep(1);
	reading_();
	difference();
	randomSample();

	XInverter_DisableAutoRestart(&ip_inst);
    print("Successfully ran Hello World application\r\n");
    cleanup_platform();
    return 0;
}


