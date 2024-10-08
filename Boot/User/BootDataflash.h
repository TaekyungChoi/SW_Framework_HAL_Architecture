/*'""FILE COMMENT""****************************************************
@ FILE Name : BootDataflash.h
@ FILE ID    : 
@ Programmer : Junbeen Son
@ Description : 
/*""FILE COMMENT END ""************************************************/

#ifndef _BOOTDATAFLASH_H_
#define _BOOTDATAFLASH_H_

//#define R_PFDL_FDL_FRQ      16      /* Sets the frequency (16 MHz)             */
#define R_PFDL_FDL_VOL      0x00    /* Sets the voltage mode (full-speed mode) */

#define DATAFLASH_ADDR_OTASTATUS	0x00
#define DATAFLASH_ADDR_BOOTFAILCNT 	0x01


pfdl_status_t BOOT_startFDL(void);
void BOOT_FDLWriteByte(U16 mu16WriteBlock, U8* mu8WriteBuffer, U16 mu16Writelength);
void BOOT_FDLReadByte(U16 mu16ReadBlock, U8* mu8RxDataBuffer, U16 mu16Readlength);

#endif

