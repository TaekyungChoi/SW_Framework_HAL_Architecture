/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2011, 2021 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : r_cg_serial_user.c
* Version      : CodeGenerator for RL78/G13 V2.05.06.02 [08 Nov 2021]
* Device(s)    : R5F100MG
* Tool-Chain   : CA78K0R
* Description  : This file implements device driver for Serial module.
* Creation Date: 2023-11-24
***********************************************************************************************************************/

/***********************************************************************************************************************
Pragma directive
***********************************************************************************************************************/
#pragma interrupt INTST0 r_uart0_interrupt_send
#pragma interrupt INTSR0 r_uart0_interrupt_receive
#pragma interrupt INTST1 r_uart1_interrupt_send
#pragma interrupt INTSR1 r_uart1_interrupt_receive
#pragma interrupt INTST2 r_uart2_interrupt_send
#pragma interrupt INTSR2 r_uart2_interrupt_receive
#pragma interrupt INTST3 r_uart3_interrupt_send
#pragma interrupt INTSR3 r_uart3_interrupt_receive
#pragma interrupt INTIICA0 r_iica0_interrupt
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Includes
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_serial.h"
/* Start user code for include. Do not edit comment generated here */
#include "HW.h"
#include "Hal_I2C_Interrupt.h"
#include "Hal_I2C.h"
#include "Hal_UART.h"
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Global variables and functions
***********************************************************************************************************************/
extern volatile uint8_t * gp_uart0_tx_address;         /* uart0 send buffer address */
extern volatile uint16_t  g_uart0_tx_count;            /* uart0 send data number */
extern volatile uint8_t * gp_uart0_rx_address;         /* uart0 receive buffer address */
extern volatile uint16_t  g_uart0_rx_count;            /* uart0 receive data number */
extern volatile uint16_t  g_uart0_rx_length;           /* uart0 receive data length */
extern volatile uint8_t * gp_uart1_tx_address;         /* uart1 send buffer address */
extern volatile uint16_t  g_uart1_tx_count;            /* uart1 send data number */
extern volatile uint8_t * gp_uart1_rx_address;         /* uart1 receive buffer address */
extern volatile uint16_t  g_uart1_rx_count;            /* uart1 receive data number */
extern volatile uint16_t  g_uart1_rx_length;           /* uart1 receive data length */
extern volatile uint8_t * gp_uart2_tx_address;         /* uart2 send buffer address */
extern volatile uint16_t  g_uart2_tx_count;            /* uart2 send data number */
extern volatile uint8_t * gp_uart2_rx_address;         /* uart2 receive buffer address */
extern volatile uint16_t  g_uart2_rx_count;            /* uart2 receive data number */
extern volatile uint16_t  g_uart2_rx_length;           /* uart2 receive data length */
extern volatile uint8_t * gp_uart3_tx_address;         /* uart3 send buffer address */
extern volatile uint16_t  g_uart3_tx_count;            /* uart3 send data number */
extern volatile uint8_t * gp_uart3_rx_address;         /* uart3 receive buffer address */
extern volatile uint16_t  g_uart3_rx_count;            /* uart3 receive data number */
extern volatile uint16_t  g_uart3_rx_length;           /* uart3 receive data length */
extern volatile uint8_t   g_iica0_master_status_flag;  /* iica0 master flag */ 
extern volatile uint8_t   g_iica0_slave_status_flag;   /* iica0 slave flag */
extern volatile uint8_t * gp_iica0_rx_address;         /* iica0 receive buffer address */
extern volatile uint16_t  g_iica0_rx_cnt;              /* iica0 receive data length */
extern volatile uint16_t  g_iica0_rx_len;              /* iica0 receive data count */
extern volatile uint8_t * gp_iica0_tx_address;         /* iica0 send buffer address */
extern volatile uint16_t  g_iica0_tx_cnt;              /* iica0 send data count */
/* Start user code for global. Do not edit comment generated here */
MD_STATUS R_IICA0_Master_Receive_START(void);
static void USER_EEPROM_iica0_master_handler(void);
static void HAL_UART_WriteRxBufferToTargetAddr(NUM_UART_T UartNum);

extern Uart_Flag_T Uart_F[NUM_UART_MAX];
extern UartRx_Ringbuffer_T UartRx[NUM_UART_MAX];
extern U8 I2C_ReceiveCommand_F;
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
* Function Name: r_uart0_interrupt_receive
* Description  : This function is INTSR0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart0_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type;

    err_type = (uint8_t)(SSR01 & 0x0007U);
    SIR01 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart0_callback_error(err_type);
    }    

#ifdef USE_UART0_RX_RINGBUFFER
    HAL_UART_WriteRxBufferToTargetAddr(NUM_UART0);
#elif USE_UART0_RX_FIXATION
    rx_data = RXD0;

    if (g_uart0_rx_length > g_uart0_rx_count)
    {
        *gp_uart0_rx_address = rx_data;
        gp_uart0_rx_address++;
        g_uart0_rx_count++;

        if (g_uart0_rx_length == g_uart0_rx_count)
        {
            r_uart0_callback_receiveend();
        }
    }
    else
    {
        r_uart0_callback_softwareoverrun(rx_data);
    }
#endif
}

/***********************************************************************************************************************
* Function Name: r_uart0_interrupt_send
* Description  : This function is INTST0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart0_interrupt_send(void)
{
    if (g_uart0_tx_count > 0U)
    {
        TXD0 = *gp_uart0_tx_address;
        gp_uart0_tx_address++;
        g_uart0_tx_count--;
    }
    else
    {
        r_uart0_callback_sendend();
    }
}

/***********************************************************************************************************************
* Function Name: r_uart0_callback_receiveend
* Description  : This function is a callback function when UART0 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
#ifdef USE_UART0_RX_FIXATION
    Uart_F[NUM_UART0].RxComplete_F = SET;
#endif
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart0_callback_softwareoverrun
* Description  : This function is a callback function when UART0 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart0_callback_sendend
* Description  : This function is a callback function when UART0 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART0].TxBusy_F = CLEAR;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart0_callback_error
* Description  : This function is a callback function when UART0 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart0_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART0].RxErr_F = SET;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart1_interrupt_receive
* Description  : This function is INTSR1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart1_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type;

    err_type = (uint8_t)(SSR03 & 0x0007U);
    SIR03 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart1_callback_error(err_type);
    }    

#ifdef USE_UART0_RX_RINGBUFFER
    HAL_UART_WriteRxBufferToTargetAddr(NUM_UART1);
#elif USE_UART0_RX_FIXATION
    rx_data = RXD1;

    if (g_uart1_rx_length > g_uart1_rx_count)
    {
        *gp_uart1_rx_address = rx_data;
        gp_uart1_rx_address++;
        g_uart1_rx_count++;

        if (g_uart1_rx_length == g_uart1_rx_count)
        {
            r_uart1_callback_receiveend();
        }
    }
    else
    {
        r_uart1_callback_softwareoverrun(rx_data);
    }
#endif
}

/***********************************************************************************************************************
* Function Name: r_uart1_interrupt_send
* Description  : This function is INTST1 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart1_interrupt_send(void)
{
    if (g_uart1_tx_count > 0U)
    {
        TXD1 = *gp_uart1_tx_address;
        gp_uart1_tx_address++;
        g_uart1_tx_count--;
    }
    else
    {
        r_uart1_callback_sendend();
    }
}

/***********************************************************************************************************************
* Function Name: r_uart1_callback_receiveend
* Description  : This function is a callback function when UART1 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
#ifdef USE_UART0_RX_FIXATION
    Uart_F[NUM_UART1].RxComplete_F = SET;
#endif    
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart1_callback_softwareoverrun
* Description  : This function is a callback function when UART1 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart1_callback_sendend
* Description  : This function is a callback function when UART1 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART1].TxBusy_F = CLEAR;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart1_callback_error
* Description  : This function is a callback function when UART1 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart1_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART1].RxErr_F = SET;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_receive
* Description  : This function is INTSR2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart2_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type; 

    err_type = (uint8_t)(SSR11 & 0x0007U);
    SIR11 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart2_callback_error(err_type);
    }    

#ifdef USE_UART0_RX_RINGBUFFER
    HAL_UART_WriteRxBufferToTargetAddr(NUM_UART2);
#elif USE_UART0_RX_FIXATION
    rx_data = RXD2;

    if (g_uart2_rx_length > g_uart2_rx_count)
    {
        *gp_uart2_rx_address = rx_data;
        gp_uart2_rx_address++;
        g_uart2_rx_count++;

        if (g_uart2_rx_length == g_uart2_rx_count)
        {
            r_uart2_callback_receiveend();
        }
    }
    else
    {
        r_uart2_callback_softwareoverrun(rx_data);
    }
#endif
}

/***********************************************************************************************************************
* Function Name: r_uart2_interrupt_send
* Description  : This function is INTST2 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart2_interrupt_send(void)
{
    if (g_uart2_tx_count > 0U)
    {
        TXD2 = *gp_uart2_tx_address;
        gp_uart2_tx_address++;
        g_uart2_tx_count--;
    }
    else
    {
        r_uart2_callback_sendend();
    }
}

/***********************************************************************************************************************
* Function Name: r_uart2_callback_receiveend
* Description  : This function is a callback function when UART2 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
#ifdef USE_UART0_RX_FIXATION
    Uart_F[NUM_UART2].RxComplete_F = SET;
#endif        
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart2_callback_softwareoverrun
* Description  : This function is a callback function when UART2 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart2_callback_sendend
* Description  : This function is a callback function when UART2 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART2].TxBusy_F = CLEAR;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart2_callback_error
* Description  : This function is a callback function when UART2 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart2_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART2].RxErr_F = SET;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart3_interrupt_receive
* Description  : This function is INTSR3 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart3_interrupt_receive(void)
{
    volatile uint8_t rx_data;
    volatile uint8_t err_type;

    err_type = (uint8_t)(SSR13 & 0x0007U);
    SIR13 = (uint16_t)err_type;

    if (err_type != 0U)
    {
        r_uart3_callback_error(err_type);
    }

#ifdef USE_UART0_RX_RINGBUFFER
    HAL_UART_WriteRxBufferToTargetAddr(NUM_UART3);
#elif USE_UART0_RX_FIXATION
    rx_data = RXD3;

    if (g_uart3_rx_length > g_uart3_rx_count)
    {
        *gp_uart3_rx_address = rx_data;
        gp_uart3_rx_address++;
        g_uart3_rx_count++;

        if (g_uart3_rx_length == g_uart3_rx_count)
        {
            r_uart3_callback_receiveend();
        }
    }
    else
    {
        r_uart3_callback_softwareoverrun(rx_data);
    }
#endif      
}

/***********************************************************************************************************************
* Function Name: r_uart3_interrupt_send
* Description  : This function is INTST3 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_uart3_interrupt_send(void)
{
    if (g_uart3_tx_count > 0U)
    {
        TXD3 = *gp_uart3_tx_address;
        gp_uart3_tx_address++;
        g_uart3_tx_count--;
    }
    else
    {
        r_uart3_callback_sendend();
    }
}

/***********************************************************************************************************************
* Function Name: r_uart3_callback_receiveend
* Description  : This function is a callback function when UART3 finishes reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_receiveend(void)
{
    /* Start user code. Do not edit comment generated here */
#ifdef USE_UART0_RX_FIXATION
    Uart_F[NUM_UART3].RxComplete_F = SET;
#endif
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart3_callback_softwareoverrun
* Description  : This function is a callback function when UART3 receives an overflow data.
* Arguments    : rx_data -
*                    receive data
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_softwareoverrun(uint16_t rx_data)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart3_callback_sendend
* Description  : This function is a callback function when UART3 finishes transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_sendend(void)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART3].TxBusy_F = CLEAR;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_uart3_callback_error
* Description  : This function is a callback function when UART3 reception error occurs.
* Arguments    : err_type -
*                    error type value
* Return Value : None
***********************************************************************************************************************/
static void r_uart3_callback_error(uint8_t err_type)
{
    /* Start user code. Do not edit comment generated here */
    Uart_F[NUM_UART3].RxErr_F = SET;
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_iica0_interrupt
* Description  : This function is INTIICA0 interrupt service routine.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
__interrupt static void r_iica0_interrupt(void)
{
    if ((IICS0 & _80_IICA_STATUS_MASTER) == 0x80U)
    {
        //iica0_master_handler();
        USER_EEPROM_iica0_master_handler();
    }
}

/***********************************************************************************************************************
* Function Name: iica0_master_handler
* Description  : This function is IICA0 master handler.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void iica0_master_handler(void)
{
    /* Detection of stop condition handling */
    if ((0U == IICBSY0) && (g_iica0_tx_cnt != 0U))
    {
        r_iica0_callback_master_error(MD_SPT);
    }
    else
    {
        /* Control for sended address */
        if ((g_iica0_master_status_flag & _80_IICA_ADDRESS_COMPLETE) == 0U)
        {
            if (1U == ACKD0)
            {
                g_iica0_master_status_flag |= _80_IICA_ADDRESS_COMPLETE;
                
                if (1U == TRC0)
                {
                    WTIM0 = 1U;
                    
                    if (g_iica0_tx_cnt > 0U)
                    {
                        IICA0 = *gp_iica0_tx_address;
                        gp_iica0_tx_address++;
                        g_iica0_tx_cnt--;
                    }
                    else
                    {
                        r_iica0_callback_master_sendend();
                    }
                }
                else
                {
                    ACKE0 = 1U;
                    WTIM0 = 0U;
                    WREL0 = 1U;
                }
            }
            else
            {
                r_iica0_callback_master_error(MD_NACK);
            }
        }
        else
        {
            /* Master send control */
            if (1U == TRC0)
            {
                if ((0U == ACKD0) && (g_iica0_tx_cnt != 0U))
                {
                    r_iica0_callback_master_error(MD_NACK);
                }
                else
                {
                    if (g_iica0_tx_cnt > 0U)
                    {
                        IICA0 = *gp_iica0_tx_address;
                        gp_iica0_tx_address++;
                        g_iica0_tx_cnt--;
                    }
                    else
                    {
                        r_iica0_callback_master_sendend();
                    }
                }
            }
            /* Master receive control */
            else
            {
                if (g_iica0_rx_cnt < g_iica0_rx_len)
                {
                    *gp_iica0_rx_address = IICA0;
                    gp_iica0_rx_address++;
                    g_iica0_rx_cnt++;
                    
                    if (g_iica0_rx_cnt == g_iica0_rx_len)
                    {
                        ACKE0 = 0U;
                        WTIM0 = 1U;
                        WREL0 = 1U;
                    }
                    else
                    {
                        WREL0 = 1U;
                    }
                }
                else
                {
                    r_iica0_callback_master_receiveend();
                }
            }
        }
    }
}

/***********************************************************************************************************************
* Function Name: r_iica0_callback_master_error
* Description  : This function is a callback function when IICA0 master error occurs.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_iica0_callback_master_error(MD_STATUS flag)
{
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_iica0_callback_master_receiveend
* Description  : This function is a callback function when IICA0 finishes master reception.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_iica0_callback_master_receiveend(void)
{
    SPT0 = 1U;
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/***********************************************************************************************************************
* Function Name: r_iica0_callback_master_sendend
* Description  : This function is a callback function when IICA0 finishes master transmission.
* Arguments    : None
* Return Value : None
***********************************************************************************************************************/
static void r_iica0_callback_master_sendend(void)
{
    SPT0 = 1U;
    /* Start user code. Do not edit comment generated here */
    /* End user code. Do not edit comment generated here */
}

/* Start user code for adding. Do not edit comment generated here */
static void USER_EEPROM_iica0_master_handler(void)
{
	MD_STATUS status;
    /* Detection of stop condition handling */
    if ((0U == IICBSY0) && (g_iica0_tx_cnt != 0U))
    {
        r_iica0_callback_master_error(MD_SPT);
    }
    else
    {
        /* Control for sended address */
        if ((g_iica0_master_status_flag & _80_IICA_ADDRESS_COMPLETE) == 0U)
        {
            if (1U == ACKD0)
            {
                g_iica0_master_status_flag |= _80_IICA_ADDRESS_COMPLETE;
                
                if (1U == TRC0)
                {
                    WTIM0 = 1U;
                    
                    if (g_iica0_tx_cnt > 0U)
                    {
                        IICA0 = *gp_iica0_tx_address;
                        gp_iica0_tx_address++;
                        g_iica0_tx_cnt--;
                    }
                    else
                    {
                        if(I2C_ReceiveCommand_F == SET)
                        {
                            I2C_ReceiveCommand_F = CLEAR;
                            status = R_IICA0_Master_Receive_START();
                        }
                        else
                        {
                            r_iica0_callback_master_sendend();
                        }
                    }
                }
                else
                {
                    ACKE0 = 1U;
                    WTIM0 = 0U;
                    WREL0 = 1U;
                }
            }
            else
            {
                r_iica0_callback_master_error(MD_NACK);
            }
        }
        else
        {
            /* Master send control */
            if (1U == TRC0)
            {
                if ((0U == ACKD0) && (g_iica0_tx_cnt != 0U))
                {
                    r_iica0_callback_master_error(MD_NACK);
                }
                else
                {
                    if (g_iica0_tx_cnt > 0U)
                    {
                        IICA0 = *gp_iica0_tx_address;
                        gp_iica0_tx_address++;
                        g_iica0_tx_cnt--;
                    }
                    else
                    {
                        if(I2C_ReceiveCommand_F == SET)
                        {
                            I2C_ReceiveCommand_F = CLEAR;
                            status = R_IICA0_Master_Receive_START();
                        }
                        else
                        {
                            r_iica0_callback_master_sendend();
                        }
                    }
                }
            }
            /* Master receive control */
            else
            {
                if (g_iica0_rx_cnt < g_iica0_rx_len)
                {
                    *gp_iica0_rx_address = IICA0;
                    gp_iica0_rx_address++;
                    g_iica0_rx_cnt++;
                    
                    if (g_iica0_rx_cnt == g_iica0_rx_len)
                    {
                        ACKE0 = 0U;
                        WTIM0 = 1U;
                        WREL0 = 1U;
                    }
                    else
                    {
                        WREL0 = 1U;
                    }
                }
                else
                {
                    r_iica0_callback_master_receiveend();
                }
            }
        }
    }
}

MD_STATUS R_IICA0_Master_Receive_START(void)
{
    uint8_t wait = 40;
    U8 g_iica0_rx_Add = 0;
	  
    U8 status = MD_OK;
    
    IICAMK0 = 1U;  /* disable INTIIA0 interrupt */
    
    STT0 = 1U; /* set IICA0 start condition */
    IICAMK0 = 0U;  /* enable INTIIA0 interrupt */
    
    /* Wait */
    while (wait--)
    {
      ;
    }
    
    /* Set parameter */
    g_iica0_rx_cnt = 0U;
    g_iica0_master_status_flag  = _00_IICA_MASTER_FLAG_CLEAR;
    //g_iica0_rx_Add = DEVICE_ID | READ_COMMAND; /* set receive mode */
    IICA0 = DEVICE_ID | READ_COMMAND; /* receive address */
    
    return (status);
}

static void HAL_UART_WriteRxBufferToTargetAddr(NUM_UART_T UartNum)
{
    U16 IdxCnt = 0;

    IdxCnt = UartRx[UartNum].TargetAddrIdx;

    UartRx[UartNum].DataBuffer[IdxCnt] = RXD0;
    UartRx[UartNum].TargetAddrIdx++;

    if(UartRx[UartNum].TargetAddrIdx > UART_RX_BUFFER_MAX)
    {
        UartRx[UartNum].OverFlow_F = SET;
        UartRx[UartNum].TargetAddrIdx = 0;
    }
}


// #ifdef USE_UART0_RX_RINGBUFFER
//     HAL_UART_WriteRxBufferToTargetAddr(NUM_UART0);
// #elif USE_UART0_RX_FIXATION
//     rx_data = RXD0;

//     if (g_uart0_rx_length > g_uart0_rx_count)
//     {
//         *gp_uart0_rx_address = rx_data;
//         gp_uart0_rx_address++;
//         g_uart0_rx_count++;

//         if (g_uart0_rx_length == g_uart0_rx_count)
//         {
//             r_uart0_callback_receiveend();
//         }
//     }
//     else
//     {
//         r_uart0_callback_softwareoverrun(rx_data);
//     }
/* End user code. Do not edit comment generated here */
