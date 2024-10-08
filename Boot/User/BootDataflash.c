/*'""FILE COMMENT""****************************************************
@ FILE Name : BootDataflash.c
@ FILE ID    : 
@ Programmer : Junbeen Son
@ Description : 
/*""FILE COMMENT END ""************************************************/

#include "BootMain.h"

pfdl_status_t BOOT_startFDL( void )
{
    pfdl_status_t       dtyFdlResult;
    pfdl_descriptor_t   dtyDescriptor;
    
    /* Inputs the initial values */
    dtyDescriptor.fx_MHz_u08            = R_PFDL_FDL_FRQ;  /* Sets the frequency    */
    dtyDescriptor.wide_voltage_mode_u08 = R_PFDL_FDL_VOL;  /* Sets the voltage mode */
    
    /* Executes the PFDL initialization function */
    dtyFdlResult = PFDL_Open( &dtyDescriptor ); 
    
    return dtyFdlResult;
}

void BOOT_FDLWriteByte(U16 mu16WriteBlock, U8* mu8WriteBuffer, U16 mu16Writelength)
{
	pfdl_status_t   dtyFdlResult;     
    pfdl_request_t  dtyRequester;       

	dtyRequester.index_u16     = mu16WriteBlock;
	dtyRequester.data_pu08     = mu8WriteBuffer;
	dtyRequester.bytecount_u16 = mu16Writelength;	
	dtyRequester.command_enu   = PFDL_CMD_ERASE_BLOCK;

    dtyFdlResult = PFDL_Execute( &dtyRequester );

    while( dtyFdlResult == PFDL_BUSY )
    {          
        dtyFdlResult = PFDL_Handler();
    } 
	if(dtyFdlResult == PFDL_OK)
	{
		dtyRequester.index_u16 = mu16WriteBlock*0x400;
     	dtyRequester.command_enu = PFDL_CMD_WRITE_BYTES;
        dtyFdlResult = PFDL_Execute( &dtyRequester );
        while( dtyFdlResult == PFDL_BUSY )
        {
            dtyFdlResult = PFDL_Handler();
        }
		if(dtyFdlResult == PFDL_OK)
		{
            dtyRequester.command_enu = PFDL_CMD_IVERIFY_BYTES;
            dtyFdlResult = PFDL_Execute( &dtyRequester );
            while( dtyFdlResult == PFDL_BUSY )
            {
                 dtyFdlResult = PFDL_Handler();
            }
			if(dtyFdlResult == PFDL_OK)
			{
			}
			else{}
		}
		else{}
	}
	else{}
}

void BOOT_FDLReadByte(U16 mu16ReadBlock, U8* mu8RxDataBuffer, U16 mu16Readlength)
{
	pfdl_status_t   dtyFdlResult;     
    pfdl_request_t  dtyRequester;
	
	dtyRequester.index_u16     = mu16ReadBlock*0x400;
	dtyRequester.data_pu08     = mu8RxDataBuffer;
	dtyRequester.bytecount_u16 = mu16Readlength;	
	dtyRequester.command_enu   = PFDL_CMD_READ_BYTES;
	
	dtyFdlResult = PFDL_Execute( &dtyRequester );

	while( dtyFdlResult == PFDL_BUSY )
    {
        dtyFdlResult = PFDL_Handler();
    }
	if(dtyFdlResult == PFDL_OK)
	{
	}
	else{}
}

