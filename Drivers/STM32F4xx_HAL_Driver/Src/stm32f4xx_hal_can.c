<<<<<<< HEAD
/**
  ******************************************************************************
  * @file    stm32f4xx_hal_can.c
  * @author  MCD Application Team
  * @version V1.7.1
  * @date    14-April-2017
  * @brief   This file provides firmware functions to manage the following 
  *          functionalities of the Controller Area Network (CAN) peripheral:
  *           + Initialization and de-initialization functions 
  *           + IO operation functions
  *           + Peripheral Control functions
  *           + Peripheral State and Error functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]            
      (#) Enable the CAN controller interface clock using 
          __HAL_RCC_CAN1_CLK_ENABLE() for CAN1, __HAL_RCC_CAN2_CLK_ENABLE() for CAN2
         and __HAL_RCC_CAN3_CLK_ENABLE() for CAN3
      -@- In case you are using CAN2 only, you have to enable the CAN1 clock.
       
      (#) CAN pins configuration
        (++) Enable the clock for the CAN GPIOs using the following function:
             __GPIOx_CLK_ENABLE()   
        (++) Connect and configure the involved CAN pins to AF9 using the 
              following function HAL_GPIO_Init() 
              
      (#) Initialize and configure the CAN using CAN_Init() function.   
                 
      (#) Transmit the desired CAN frame using HAL_CAN_Transmit() function.

      (#) Or transmit the desired CAN frame using HAL_CAN_Transmit_IT() function.
           
      (#) Receive a CAN frame using HAL_CAN_Receive() function.

      (#) Or receive a CAN frame using HAL_CAN_Receive_IT() function.

     *** Polling mode IO operation ***
     =================================
     [..]    
       (+) Start the CAN peripheral transmission and wait the end of this operation 
           using HAL_CAN_Transmit(), at this stage user can specify the value of timeout
           according to his end application
       (+) Start the CAN peripheral reception and wait the end of this operation 
           using HAL_CAN_Receive(), at this stage user can specify the value of timeout
           according to his end application 
       
     *** Interrupt mode IO operation ***    
     ===================================
     [..]    
       (+) Start the CAN peripheral transmission using HAL_CAN_Transmit_IT()
       (+) Start the CAN peripheral reception using HAL_CAN_Receive_IT()         
       (+) Use HAL_CAN_IRQHandler() called under the used CAN Interrupt subroutine
       (+) At CAN end of transmission HAL_CAN_TxCpltCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_CAN_TxCpltCallback 
       (+) In case of CAN Error, HAL_CAN_ErrorCallback() function is executed and user can 
            add his own code by customization of function pointer HAL_CAN_ErrorCallback
 
     *** CAN HAL driver macros list ***
     ============================================= 
     [..]
       Below the list of most used macros in CAN HAL driver.
       
      (+) __HAL_CAN_ENABLE_IT: Enable the specified CAN interrupts
      (+) __HAL_CAN_DISABLE_IT: Disable the specified CAN interrupts
      (+) __HAL_CAN_GET_IT_SOURCE: Check if the specified CAN interrupt source is enabled or disabled
      (+) __HAL_CAN_CLEAR_FLAG: Clear the CAN's pending flags
      (+) __HAL_CAN_GET_FLAG: Get the selected CAN's flag status
      
     [..] 
      (@) You can refer to the CAN HAL driver header file for more useful macros 
                
  @endverbatim
           
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup CAN CAN
  * @brief CAN driver modules
  * @{
  */ 
  
#ifdef HAL_CAN_MODULE_ENABLED  

#if defined(STM32F405xx) || defined(STM32F415xx) || defined(STM32F407xx) || defined(STM32F417xx) ||\
    defined(STM32F427xx) || defined(STM32F437xx) || defined(STM32F429xx) || defined(STM32F439xx) ||\
    defined(STM32F446xx) || defined(STM32F469xx) || defined(STM32F479xx) || defined(STM32F412Zx) ||\
    defined(STM32F412Vx) || defined(STM32F412Rx) || defined(STM32F412Cx) || defined(STM32F413xx) ||\
    defined(STM32F423xx)  
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @addtogroup CAN_Private_Constants
  * @{
  */
#define CAN_TIMEOUT_VALUE  10U
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/** @addtogroup CAN_Private_Functions
  * @{
  */
static HAL_StatusTypeDef CAN_Receive_IT(CAN_HandleTypeDef* hcan, uint8_t FIFONumber);
static HAL_StatusTypeDef CAN_Transmit_IT(CAN_HandleTypeDef* hcan);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup CAN_Exported_Functions CAN Exported Functions
  * @{
  */

/** @defgroup CAN_Exported_Functions_Group1 Initialization and de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Initialize and configure the CAN. 
      (+) De-initialize the CAN. 
         
@endverbatim
  * @{
  */
  
/**
  * @brief  Initializes the CAN peripheral according to the specified
  *         parameters in the CAN_InitStruct.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef* hcan)
{
  uint32_t InitStatus = CAN_INITSTATUS_FAILED;
  uint32_t tickstart = 0U;
  
  /* Check CAN handle */
  if(hcan == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_CAN_ALL_INSTANCE(hcan->Instance));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.TTCM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.ABOM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.AWUM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.NART));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.RFLM));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.TXFP));
  assert_param(IS_CAN_MODE(hcan->Init.Mode));
  assert_param(IS_CAN_SJW(hcan->Init.SJW));
  assert_param(IS_CAN_BS1(hcan->Init.BS1));
  assert_param(IS_CAN_BS2(hcan->Init.BS2));
  assert_param(IS_CAN_PRESCALER(hcan->Init.Prescaler));
  

  if(hcan->State == HAL_CAN_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    hcan->Lock = HAL_UNLOCKED;
    /* Init the low level hardware */
    HAL_CAN_MspInit(hcan);
  }
  
  /* Initialize the CAN state*/
  hcan->State = HAL_CAN_STATE_BUSY;
  
  /* Exit from sleep mode */
  hcan->Instance->MCR &= (~(uint32_t)CAN_MCR_SLEEP);

  /* Request initialisation */
  hcan->Instance->MCR |= CAN_MCR_INRQ ;
  
  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait the acknowledge */
  while((hcan->Instance->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
  {
    if((HAL_GetTick() - tickstart ) > CAN_TIMEOUT_VALUE)
    {
      hcan->State= HAL_CAN_STATE_TIMEOUT;
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
      return HAL_TIMEOUT;
    }
  }

  /* Check acknowledge */
  if ((hcan->Instance->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)
  {
    /* Set the time triggered communication mode */
    if (hcan->Init.TTCM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_TTCM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_TTCM;
    }

    /* Set the automatic bus-off management */
    if (hcan->Init.ABOM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_ABOM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_ABOM;
    }

    /* Set the automatic wake-up mode */
    if (hcan->Init.AWUM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_AWUM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_AWUM;
    }

    /* Set the no automatic retransmission */
    if (hcan->Init.NART == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_NART;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_NART;
    }

    /* Set the receive FIFO locked mode */
    if (hcan->Init.RFLM == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_RFLM;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_RFLM;
    }

    /* Set the transmit FIFO priority */
    if (hcan->Init.TXFP == ENABLE)
    {
      hcan->Instance->MCR |= CAN_MCR_TXFP;
    }
    else
    {
      hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_TXFP;
    }

    /* Set the bit timing register */
    hcan->Instance->BTR = (uint32_t)((uint32_t)hcan->Init.Mode) | \
                ((uint32_t)hcan->Init.SJW) | \
                ((uint32_t)hcan->Init.BS1) | \
                ((uint32_t)hcan->Init.BS2) | \
               ((uint32_t)hcan->Init.Prescaler - 1U);

    /* Request leave initialisation */
    hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_INRQ;

  /* Get tick */
  tickstart = HAL_GetTick();

   /* Wait the acknowledge */
   while((hcan->Instance->MSR & CAN_MSR_INAK) == CAN_MSR_INAK)
   {
    if((HAL_GetTick() - tickstart ) > CAN_TIMEOUT_VALUE)
     {
       hcan->State= HAL_CAN_STATE_TIMEOUT;
       /* Process unlocked */
       __HAL_UNLOCK(hcan);
       return HAL_TIMEOUT;
     }
   }

    /* Check acknowledged */
    if ((hcan->Instance->MSR & CAN_MSR_INAK) != CAN_MSR_INAK)
    {
      InitStatus = CAN_INITSTATUS_SUCCESS;
    }
  }
 
  if(InitStatus == CAN_INITSTATUS_SUCCESS)
  {
    /* Set CAN error code to none */
    hcan->ErrorCode = HAL_CAN_ERROR_NONE;
    
    /* Initialize the CAN state */
    hcan->State = HAL_CAN_STATE_READY;
  
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Initialize the CAN state */
    hcan->State = HAL_CAN_STATE_ERROR;
    
    /* Return function status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Configures the CAN reception filter according to the specified
  *         parameters in the CAN_FilterInitStruct.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  sFilterConfig: pointer to a CAN_FilterConfTypeDef structure that
  *         contains the filter configuration information.
  * @retval None
  */
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef* hcan, CAN_FilterConfTypeDef* sFilterConfig)
{
  uint32_t filternbrbitpos = 0U;
  CAN_TypeDef *can_ip;
  
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* Check the parameters */
  assert_param(IS_CAN_FILTER_NUMBER(sFilterConfig->FilterNumber));
  assert_param(IS_CAN_FILTER_MODE(sFilterConfig->FilterMode));
  assert_param(IS_CAN_FILTER_SCALE(sFilterConfig->FilterScale));
  assert_param(IS_CAN_FILTER_FIFO(sFilterConfig->FilterFIFOAssignment));
  assert_param(IS_FUNCTIONAL_STATE(sFilterConfig->FilterActivation));
  assert_param(IS_CAN_BANKNUMBER(sFilterConfig->BankNumber));
  
  filternbrbitpos = 1U << sFilterConfig->FilterNumber;
#if defined (CAN3)
  /* Check the CAN instance */
  if(hcan->Instance == CAN3)
  {	
    can_ip = CAN3;
  }
  else
  {
    can_ip = CAN1;    
  }
#else
  can_ip = CAN1;
#endif
  
  /* Initialisation mode for the filter */
  can_ip->FMR |= (uint32_t)CAN_FMR_FINIT;

#if defined (CAN2)  
  /* Select the start slave bank */
  can_ip->FMR &= ~((uint32_t)CAN_FMR_CAN2SB);
  can_ip->FMR |= (uint32_t)(sFilterConfig->BankNumber << 8U);
#endif
     
  /* Filter Deactivation */
  can_ip->FA1R &= ~(uint32_t)filternbrbitpos;

  /* Filter Scale */
  if (sFilterConfig->FilterScale == CAN_FILTERSCALE_16BIT)
  {
    /* 16-bit scale for the filter */
    can_ip->FS1R &= ~(uint32_t)filternbrbitpos;

    /* First 16-bit identifier and First 16-bit mask */
    /* Or First 16-bit identifier and Second 16-bit identifier */
    can_ip->sFilterRegister[sFilterConfig->FilterNumber].FR1 = 
       ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdLow) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdLow);

    /* Second 16-bit identifier and Second 16-bit mask */
    /* Or Third 16-bit identifier and Fourth 16-bit identifier */
    can_ip->sFilterRegister[sFilterConfig->FilterNumber].FR2 = 
       ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdHigh);
  }

  if (sFilterConfig->FilterScale == CAN_FILTERSCALE_32BIT)
  {
    /* 32-bit scale for the filter */
    can_ip->FS1R |= filternbrbitpos;
    
    /* 32-bit identifier or First 32-bit identifier */
    can_ip->sFilterRegister[sFilterConfig->FilterNumber].FR1 = 
       ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdHigh) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdLow);
    /* 32-bit mask or Second 32-bit identifier */
    can_ip->sFilterRegister[sFilterConfig->FilterNumber].FR2 = 
       ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdLow);
  }

  /* Filter Mode */
  if (sFilterConfig->FilterMode == CAN_FILTERMODE_IDMASK)
  {
    /*Id/Mask mode for the filter*/
    can_ip->FM1R &= ~(uint32_t)filternbrbitpos;
  }
  else /* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
  {
    /*Identifier list mode for the filter*/
    can_ip->FM1R |= (uint32_t)filternbrbitpos;
  }

  /* Filter FIFO assignment */
  if (sFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO0)
  {
    /* FIFO 0 assignation for the filter */
    can_ip->FFA1R &= ~(uint32_t)filternbrbitpos;
  }

  if (sFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO1)
  {
    /* FIFO 1 assignation for the filter */
    can_ip->FFA1R |= (uint32_t)filternbrbitpos;
  }
  
  /* Filter activation */
  if (sFilterConfig->FilterActivation == ENABLE)
  {
    can_ip->FA1R |= filternbrbitpos;
  }

  /* Leave the initialisation mode for the filter */
  can_ip->FMR &= ~((uint32_t)CAN_FMR_FINIT);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Deinitializes the CANx peripheral registers to their default reset values. 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_DeInit(CAN_HandleTypeDef* hcan)
{
  /* Check CAN handle */
  if(hcan == NULL)
  {
     return HAL_ERROR;
  }
  
  /* Check the parameters */
  assert_param(IS_CAN_ALL_INSTANCE(hcan->Instance));
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_BUSY;
  
  /* DeInit the low level hardware */
  HAL_CAN_MspDeInit(hcan);
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(hcan);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CAN MSP.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval None
  */
__weak void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_MspInit could be implemented in the user file
   */ 
}

/**
  * @brief  DeInitializes the CAN MSP.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval None
  */
__weak void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_MspDeInit could be implemented in the user file
   */ 
}

/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group2 IO operation functions
 *  @brief    IO operation functions 
 *
@verbatim   
  ==============================================================================
                      ##### IO operation functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) Transmit a CAN frame message.
      (+) Receive a CAN frame message.
      (+) Enter CAN peripheral in sleep mode. 
      (+) Wake up the CAN peripheral from sleep mode.
               
@endverbatim
  * @{
  */

/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  Timeout: Specify Timeout value   
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Transmit(CAN_HandleTypeDef* hcan, uint32_t Timeout)
{
  uint32_t transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
  uint32_t tickstart = 0U;

  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
  assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
  assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));

  if(((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0) || \
     ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1) || \
     ((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2))
  {  
    /* Process locked */
    __HAL_LOCK(hcan);
  
    /* Change CAN state */
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_RX0):
          hcan->State = HAL_CAN_STATE_BUSY_TX_RX0;
          break;
      case(HAL_CAN_STATE_BUSY_RX1):
          hcan->State = HAL_CAN_STATE_BUSY_TX_RX1;
          break;
      case(HAL_CAN_STATE_BUSY_RX0_RX1):
          hcan->State = HAL_CAN_STATE_BUSY_TX_RX0_RX1;
          break;
      default: /* HAL_CAN_STATE_READY */
          hcan->State = HAL_CAN_STATE_BUSY_TX;
          break;
    }
  
    /* Select one empty transmit mailbox */
    if ((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
    {
      transmitmailbox = CAN_TXMAILBOX_0;
    }
    else if ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
    {
      transmitmailbox = CAN_TXMAILBOX_1;
    }
    else
    {
      transmitmailbox = CAN_TXMAILBOX_2;
    }

    /* Set up the Id */
    hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
    if (hcan->pTxMsg->IDE == CAN_ID_STD)
    {
      assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));  
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21U) | \
                                                  hcan->pTxMsg->RTR);
    }
    else
    {
      assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3U) | \
                                                  hcan->pTxMsg->IDE | \
                                                  hcan->pTxMsg->RTR);
    }
    
    /* Set up the DLC */
    hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
    hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0U;
    hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

    /* Set up the data field */
    hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3U] << 24U) | 
                                             ((uint32_t)hcan->pTxMsg->Data[2U] << 16U) |
                                             ((uint32_t)hcan->pTxMsg->Data[1U] << 8U) | 
                                             ((uint32_t)hcan->pTxMsg->Data[0U]));
    hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7U] << 24U) | 
                                             ((uint32_t)hcan->pTxMsg->Data[6U] << 16U) |
                                             ((uint32_t)hcan->pTxMsg->Data[5U] << 8U) |
                                             ((uint32_t)hcan->pTxMsg->Data[4U]));
    /* Request transmission */
    hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;
  
    /* Get tick */
    tickstart = HAL_GetTick();
  
    /* Check End of transmission flag */
    while(!(__HAL_CAN_TRANSMIT_STATUS(hcan, transmitmailbox)))
    {
      /* Check for the Timeout */
      if(Timeout != HAL_MAX_DELAY)
      {
       if((Timeout == 0U)||((HAL_GetTick() - tickstart ) > Timeout))
       {
         hcan->State = HAL_CAN_STATE_TIMEOUT;

         __HAL_CAN_CANCEL_TRANSMIT(hcan, transmitmailbox);

         /* Process unlocked */
         __HAL_UNLOCK(hcan);
         return HAL_TIMEOUT;
        }
      }
    }

    /* Change CAN state */
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX_RX0):
          hcan->State = HAL_CAN_STATE_BUSY_RX0;
          break;
      case(HAL_CAN_STATE_BUSY_TX_RX1):
          hcan->State = HAL_CAN_STATE_BUSY_RX1;
          break;
      case(HAL_CAN_STATE_BUSY_TX_RX0_RX1):
          hcan->State = HAL_CAN_STATE_BUSY_RX0_RX1;
          break;
      default: /* HAL_CAN_STATE_BUSY_TX */
          hcan->State = HAL_CAN_STATE_READY;
          break;
    }

    /* Process unlocked */
    __HAL_UNLOCK(hcan);
    
    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_ERROR; 

    /* Return function status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Transmit_IT(CAN_HandleTypeDef* hcan)
{
  uint32_t  transmitmailbox = CAN_TXSTATUS_NOMAILBOX;
  
  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(hcan->pTxMsg->IDE));
  assert_param(IS_CAN_RTR(hcan->pTxMsg->RTR));
  assert_param(IS_CAN_DLC(hcan->pTxMsg->DLC));
  
  if(((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0) || \
     ((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1) || \
     ((hcan->Instance->TSR&CAN_TSR_TME2) == CAN_TSR_TME2))
  {
    /* Process Locked */
    __HAL_LOCK(hcan);

    /* Select one empty transmit mailbox */
    if((hcan->Instance->TSR&CAN_TSR_TME0) == CAN_TSR_TME0)
    {
      transmitmailbox = CAN_TXMAILBOX_0;
    }
    else if((hcan->Instance->TSR&CAN_TSR_TME1) == CAN_TSR_TME1)
    {
      transmitmailbox = CAN_TXMAILBOX_1;
    }
    else
    {
      transmitmailbox = CAN_TXMAILBOX_2;
    }

    /* Set up the Id */
    hcan->Instance->sTxMailBox[transmitmailbox].TIR &= CAN_TI0R_TXRQ;
    if(hcan->pTxMsg->IDE == CAN_ID_STD)
    {
      assert_param(IS_CAN_STDID(hcan->pTxMsg->StdId));  
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->StdId << 21U) | \
                                                hcan->pTxMsg->RTR);
    }
    else
    {
      assert_param(IS_CAN_EXTID(hcan->pTxMsg->ExtId));
      hcan->Instance->sTxMailBox[transmitmailbox].TIR |= ((hcan->pTxMsg->ExtId << 3U) | \
                                                hcan->pTxMsg->IDE | \
                                                hcan->pTxMsg->RTR);
    }
    
    /* Set up the DLC */
    hcan->pTxMsg->DLC &= (uint8_t)0x0000000F;
    hcan->Instance->sTxMailBox[transmitmailbox].TDTR &= (uint32_t)0xFFFFFFF0U;
    hcan->Instance->sTxMailBox[transmitmailbox].TDTR |= hcan->pTxMsg->DLC;

    /* Set up the data field */
    hcan->Instance->sTxMailBox[transmitmailbox].TDLR = (((uint32_t)hcan->pTxMsg->Data[3U] << 24U) |
                                           ((uint32_t)hcan->pTxMsg->Data[2U] << 16U) |
                                           ((uint32_t)hcan->pTxMsg->Data[1U] << 8U) | 
                                           ((uint32_t)hcan->pTxMsg->Data[0U]));
    hcan->Instance->sTxMailBox[transmitmailbox].TDHR = (((uint32_t)hcan->pTxMsg->Data[7U] << 24U) |
                                           ((uint32_t)hcan->pTxMsg->Data[6U] << 16U) |
                                           ((uint32_t)hcan->pTxMsg->Data[5U] << 8U) |
                                           ((uint32_t)hcan->pTxMsg->Data[4U]));

    /* Change CAN state */
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_RX0):
          hcan->State = HAL_CAN_STATE_BUSY_TX_RX0;
          break;
      case(HAL_CAN_STATE_BUSY_RX1):
          hcan->State = HAL_CAN_STATE_BUSY_TX_RX1;
          break;
      case(HAL_CAN_STATE_BUSY_RX0_RX1):
          hcan->State = HAL_CAN_STATE_BUSY_TX_RX0_RX1;
          break;
      default: /* HAL_CAN_STATE_READY */
          hcan->State = HAL_CAN_STATE_BUSY_TX;
          break;
    }

    /* Set CAN error code to none */
    hcan->ErrorCode = HAL_CAN_ERROR_NONE;

    /* Process Unlocked */
    __HAL_UNLOCK(hcan);

    /* Request transmission */
    hcan->Instance->sTxMailBox[transmitmailbox].TIR |= CAN_TI0R_TXRQ;

    /* Enable Error warning, Error passive, Bus-off,
       Last error and Error Interrupts */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EWG |
                              CAN_IT_EPV |
                              CAN_IT_BOF |
                              CAN_IT_LEC |
                              CAN_IT_ERR |
                              CAN_IT_TME);
  }
  else
  {
    /* Change CAN state */
    hcan->State = HAL_CAN_STATE_ERROR; 

    /* Return function status */
    return HAL_ERROR;
  }
  
  return HAL_OK;
}

/**
  * @brief  Receives a correct CAN frame.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  FIFONumber: FIFO Number value
  * @param  Timeout: Specify Timeout value 
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Receive(CAN_HandleTypeDef* hcan, uint8_t FIFONumber, uint32_t Timeout)
{
  uint32_t tickstart = 0U;
  CanRxMsgTypeDef* pRxMsg = NULL;
 
  /* Check the parameters */
  assert_param(IS_CAN_FIFO(FIFONumber));

  /* Check if CAN state is not busy for RX FIFO0 */
  if ((FIFONumber == CAN_FIFO0) && ((hcan->State == HAL_CAN_STATE_BUSY_RX0) ||         \
                                    (hcan->State == HAL_CAN_STATE_BUSY_TX_RX0) ||      \
                                    (hcan->State == HAL_CAN_STATE_BUSY_RX0_RX1) ||     \
                                    (hcan->State == HAL_CAN_STATE_BUSY_TX_RX0_RX1)))
  {
    return HAL_BUSY;
  }

  /* Check if CAN state is not busy for RX FIFO1 */
  if ((FIFONumber == CAN_FIFO1) && ((hcan->State == HAL_CAN_STATE_BUSY_RX1) ||         \
                                    (hcan->State == HAL_CAN_STATE_BUSY_TX_RX1) ||      \
                                    (hcan->State == HAL_CAN_STATE_BUSY_RX0_RX1) ||     \
                                    (hcan->State == HAL_CAN_STATE_BUSY_TX_RX0_RX1)))
  {
    return HAL_BUSY;
  }

  /* Process locked */
  __HAL_LOCK(hcan);

  /* Change CAN state */
  if (FIFONumber == CAN_FIFO0)
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0;
        break;
      case(HAL_CAN_STATE_BUSY_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_RX0_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0_RX1;
        break;
      default: /* HAL_CAN_STATE_READY */
        hcan->State = HAL_CAN_STATE_BUSY_RX0;
        break;
    }
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_RX0):
        hcan->State = HAL_CAN_STATE_BUSY_RX0_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX0):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0_RX1;
        break;
      default: /* HAL_CAN_STATE_READY */
        hcan->State = HAL_CAN_STATE_BUSY_RX1;
        break;
    }
  }

  /* Get tick */ 
  tickstart = HAL_GetTick();
  
  /* Check pending message */
  while(__HAL_CAN_MSG_PENDING(hcan, FIFONumber) == 0U)
  {
    /* Check for the Timeout */
    if(Timeout != HAL_MAX_DELAY)
    {
      if((Timeout == 0U)||((HAL_GetTick() - tickstart ) > Timeout))
      {
        hcan->State = HAL_CAN_STATE_TIMEOUT;
        /* Process unlocked */
        __HAL_UNLOCK(hcan);
        return HAL_TIMEOUT;
      }
    }
  }

  /* Set RxMsg pointer */
  if(FIFONumber == CAN_FIFO0)
  {
    pRxMsg = hcan->pRxMsg;
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    pRxMsg = hcan->pRx1Msg;
  }

  /* Get the Id */
  pRxMsg->IDE = (uint8_t)0x04 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  if (pRxMsg->IDE == CAN_ID_STD)
  {
    pRxMsg->StdId = 0x000007FFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 21U);
  }
  else
  {
    pRxMsg->ExtId = 0x1FFFFFFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 3U);
  }
  
  pRxMsg->RTR = (uint8_t)0x02 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  /* Get the DLC */
  pRxMsg->DLC = (uint8_t)0x0F & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR;
  /* Get the FMI */
  pRxMsg->FMI = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDTR >> 8U);
  /* Get the FIFONumber */
  pRxMsg->FIFONumber = FIFONumber;
  /* Get the data field */
  pRxMsg->Data[0] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDLR;
  pRxMsg->Data[1] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 8U);
  pRxMsg->Data[2] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 16U);
  pRxMsg->Data[3] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 24U);
  pRxMsg->Data[4] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDHR;
  pRxMsg->Data[5] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 8U);
  pRxMsg->Data[6] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 16U);
  pRxMsg->Data[7] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 24U);

  /* Release the FIFO */
  if(FIFONumber == CAN_FIFO0)
  {
    /* Release FIFO0 */
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO0);
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    /* Release FIFO1 */
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO1);
  }

  /* Change CAN state */
  if (FIFONumber == CAN_FIFO0)
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX_RX0):
        hcan->State = HAL_CAN_STATE_BUSY_TX;
        break;
      case(HAL_CAN_STATE_BUSY_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX1;
        break;
      default: /* HAL_CAN_STATE_BUSY_RX0 */
        hcan->State = HAL_CAN_STATE_READY;
        break;
    }
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX;
        break;
      case(HAL_CAN_STATE_BUSY_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_RX0;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0;
        break;
      default: /* HAL_CAN_STATE_BUSY_RX1 */
        hcan->State = HAL_CAN_STATE_READY;
        break;
    }
  }

  /* Process unlocked */
  __HAL_UNLOCK(hcan);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Receives a correct CAN frame.
  * @param  hcan:       Pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  FIFONumber: Specify the FIFO number    
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Receive_IT(CAN_HandleTypeDef* hcan, uint8_t FIFONumber)
{
  /* Check the parameters */
  assert_param(IS_CAN_FIFO(FIFONumber));
  
  /* Check if CAN state is not busy for RX FIFO0 */
  if((FIFONumber == CAN_FIFO0) && ((hcan->State == HAL_CAN_STATE_BUSY_RX0) ||         \
                                   (hcan->State == HAL_CAN_STATE_BUSY_TX_RX0) ||      \
                                   (hcan->State == HAL_CAN_STATE_BUSY_RX0_RX1) ||     \
                                   (hcan->State == HAL_CAN_STATE_BUSY_TX_RX0_RX1)))
  {
    return HAL_BUSY;
  }

  /* Check if CAN state is not busy for RX FIFO1 */
  if((FIFONumber == CAN_FIFO1) && ((hcan->State == HAL_CAN_STATE_BUSY_RX1) ||         \
                                   (hcan->State == HAL_CAN_STATE_BUSY_TX_RX1) ||      \
                                   (hcan->State == HAL_CAN_STATE_BUSY_RX0_RX1) ||     \
                                   (hcan->State == HAL_CAN_STATE_BUSY_TX_RX0_RX1)))
  {
    return HAL_BUSY;
  }

  /* Process locked */
  __HAL_LOCK(hcan);

  /* Change CAN state */
  if(FIFONumber == CAN_FIFO0)
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0;
        break;
      case(HAL_CAN_STATE_BUSY_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_RX0_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0_RX1;
        break;
      default: /* HAL_CAN_STATE_READY */
        hcan->State = HAL_CAN_STATE_BUSY_RX0;
        break;
    }
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_RX0):
        hcan->State = HAL_CAN_STATE_BUSY_RX0_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX0):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0_RX1;
        break;
      default: /* HAL_CAN_STATE_READY */
        hcan->State = HAL_CAN_STATE_BUSY_RX1;
        break;
    }
  }
  /* Set CAN error code to none */
  hcan->ErrorCode = HAL_CAN_ERROR_NONE;

  /* Enable interrupts: */
  /*  - Enable Error warning Interrupt */
  /*  - Enable Error passive Interrupt */
  /*  - Enable Bus-off Interrupt */
  /*  - Enable Last error code Interrupt */
  /*  - Enable Error Interrupt */
  /*  - Enable Transmit mailbox empty Interrupt */
  __HAL_CAN_ENABLE_IT(hcan, CAN_IT_EWG |
                            CAN_IT_EPV |
                            CAN_IT_BOF |
                            CAN_IT_LEC |
                            CAN_IT_ERR |
                            CAN_IT_TME);

  /* Process unlocked */
   __HAL_UNLOCK(hcan);

  if(FIFONumber == CAN_FIFO0)
  {
    /* Enable FIFO 0 overrun and message pending Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FOV0 | CAN_IT_FMP0);
  }
  else
  {
    /* Enable FIFO 1 overrun and message pending Interrupt */
    __HAL_CAN_ENABLE_IT(hcan, CAN_IT_FOV1 | CAN_IT_FMP1);
  }

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Enters the Sleep (low power) mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_CAN_Sleep(CAN_HandleTypeDef* hcan)
{
  uint32_t tickstart = 0U;
   
  /* Process locked */
  __HAL_LOCK(hcan);
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_BUSY; 
    
  /* Request Sleep mode */
   hcan->Instance->MCR = (((hcan->Instance->MCR) & (uint32_t)(~(uint32_t)CAN_MCR_INRQ)) | CAN_MCR_SLEEP);

  /* Sleep mode status */
  if ((hcan->Instance->MSR & (CAN_MSR_SLAK|CAN_MSR_INAK)) != CAN_MSR_SLAK)
  {
    /* Process unlocked */
    __HAL_UNLOCK(hcan);

    /* Return function status */
    return HAL_ERROR;
  }

  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Wait the acknowledge */
  while((hcan->Instance->MSR & (CAN_MSR_SLAK|CAN_MSR_INAK)) != CAN_MSR_SLAK)
  {
    if((HAL_GetTick()  - tickstart) > CAN_TIMEOUT_VALUE)
    {
      hcan->State = HAL_CAN_STATE_TIMEOUT;
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
      return HAL_TIMEOUT;
    }
  }

  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_READY;

  /* Process unlocked */
  __HAL_UNLOCK(hcan);

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Wakes up the CAN peripheral from sleep mode, after that the CAN peripheral
  *         is in the normal mode.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_CAN_WakeUp(CAN_HandleTypeDef* hcan)
{
  uint32_t tickstart = 0U;
    
  /* Process locked */
  __HAL_LOCK(hcan);
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_BUSY;  
 
  /* Wake up request */
  hcan->Instance->MCR &= ~(uint32_t)CAN_MCR_SLEEP;

  /* Get tick */ 
  tickstart = HAL_GetTick();

  /* Sleep mode status */
  while((hcan->Instance->MSR & CAN_MSR_SLAK) == CAN_MSR_SLAK)
  {
    if((HAL_GetTick()  - tickstart) > CAN_TIMEOUT_VALUE)
    {
      hcan->State= HAL_CAN_STATE_TIMEOUT;
      /* Process unlocked */
      __HAL_UNLOCK(hcan);
      return HAL_TIMEOUT;
    }
  }
  if((hcan->Instance->MSR & CAN_MSR_SLAK) == CAN_MSR_SLAK)
  {
    /* Process unlocked */
    __HAL_UNLOCK(hcan);
 
    /* Return function status */
    return HAL_ERROR;
  }
  
  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_READY; 
  
  /* Process unlocked */
  __HAL_UNLOCK(hcan);
  
  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Handles CAN interrupt request  
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_IRQHandler(CAN_HandleTypeDef* hcan)
{
  uint32_t tmp1 = 0U, tmp2 = 0U, tmp3 = 0U;
  uint32_t errorcode = HAL_CAN_ERROR_NONE;

  /* Check Overrun flag for FIFO0 */
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_FOV0);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_FOV0);
  if(tmp1 && tmp2)
  {
    /* Set CAN error code to FOV0 error */
    errorcode |= HAL_CAN_ERROR_FOV0;

    /* Clear FIFO0 Overrun Flag */
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV0);
  }
  /* Check Overrun flag for FIFO1 */
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_FOV1);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_FOV1);

  if(tmp1 && tmp2)
  {
    /* Set CAN error code to FOV1 error */
    errorcode |= HAL_CAN_ERROR_FOV1;

    /* Clear FIFO1 Overrun Flag */
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV1);
  }

  /* Check End of transmission flag */
  if(__HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_TME))
  {
    tmp1 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_0);
    tmp2 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_1);
    tmp3 = __HAL_CAN_TRANSMIT_STATUS(hcan, CAN_TXMAILBOX_2);
    if(tmp1 || tmp2 || tmp3)  
    {
      tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_TXOK0);
      tmp2 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_TXOK1);
      tmp3 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_TXOK2);
      /* Check Transmit success */
      if(tmp1 || tmp2 || tmp3)
      {
        /* Call transmit function */
        CAN_Transmit_IT(hcan);
      }
      else /* Transmit failure */
      {
        /* Set CAN error code to TXFAIL error */
        errorcode |= HAL_CAN_ERROR_TXFAIL;
      }

      /* Clear transmission status flags (RQCPx and TXOKx) */
      SET_BIT(hcan->Instance->TSR, CAN_TSR_RQCP0  | CAN_TSR_RQCP1  | CAN_TSR_RQCP2 | \
                                   CAN_FLAG_TXOK0 | CAN_FLAG_TXOK1 | CAN_FLAG_TXOK2);
    }
  }

  tmp1 = __HAL_CAN_MSG_PENDING(hcan, CAN_FIFO0);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_FMP0);
  /* Check End of reception flag for FIFO0 */
  if((tmp1 != 0U) && tmp2)
  {
    /* Call receive function */
    CAN_Receive_IT(hcan, CAN_FIFO0);
  }

  tmp1 = __HAL_CAN_MSG_PENDING(hcan, CAN_FIFO1);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_FMP1);
  /* Check End of reception flag for FIFO1 */
  if((tmp1 != 0U) && tmp2)
  {
    /* Call receive function */
    CAN_Receive_IT(hcan, CAN_FIFO1);
  }

  /* Set error code in handle */
  hcan->ErrorCode |= errorcode;

  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_EWG);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_EWG);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR);
  /* Check Error Warning Flag */
  if(tmp1 && tmp2 && tmp3)
  {
    /* Set CAN error code to EWG error */
    hcan->ErrorCode |= HAL_CAN_ERROR_EWG;
  }
  
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_EPV);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_EPV);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR); 
  /* Check Error Passive Flag */
  if(tmp1 && tmp2 && tmp3)
  {
    /* Set CAN error code to EPV error */
    hcan->ErrorCode |= HAL_CAN_ERROR_EPV;
  }
  
  tmp1 = __HAL_CAN_GET_FLAG(hcan, CAN_FLAG_BOF);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_BOF);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR);  
  /* Check Bus-Off Flag */
  if(tmp1 && tmp2 && tmp3)
  {
    /* Set CAN error code to BOF error */
    hcan->ErrorCode |= HAL_CAN_ERROR_BOF;
  }
  
  tmp1 = HAL_IS_BIT_CLR(hcan->Instance->ESR, CAN_ESR_LEC);
  tmp2 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_LEC);
  tmp3 = __HAL_CAN_GET_IT_SOURCE(hcan, CAN_IT_ERR);
  /* Check Last error code Flag */
  if((!tmp1) && tmp2 && tmp3)
  {
    tmp1 = (hcan->Instance->ESR) & CAN_ESR_LEC;
    switch(tmp1)
    {
      case(CAN_ESR_LEC_0):
          /* Set CAN error code to STF error */
          hcan->ErrorCode |= HAL_CAN_ERROR_STF;
          break;
      case(CAN_ESR_LEC_1):
          /* Set CAN error code to FOR error */
          hcan->ErrorCode |= HAL_CAN_ERROR_FOR;
          break;
      case(CAN_ESR_LEC_1 | CAN_ESR_LEC_0):
          /* Set CAN error code to ACK error */
          hcan->ErrorCode |= HAL_CAN_ERROR_ACK;
          break;
      case(CAN_ESR_LEC_2):
          /* Set CAN error code to BR error */
          hcan->ErrorCode |= HAL_CAN_ERROR_BR;
          break;
      case(CAN_ESR_LEC_2 | CAN_ESR_LEC_0):
          /* Set CAN error code to BD error */
          hcan->ErrorCode |= HAL_CAN_ERROR_BD;
          break;
      case(CAN_ESR_LEC_2 | CAN_ESR_LEC_1):
          /* Set CAN error code to CRC error */
          hcan->ErrorCode |= HAL_CAN_ERROR_CRC;
          break;
      default:
          break;
    }

    /* Clear Last error code Flag */ 
    hcan->Instance->ESR &= ~(CAN_ESR_LEC);
  }
  
  /* Call the Error call Back in case of Errors */
  if(hcan->ErrorCode != HAL_CAN_ERROR_NONE)
  {
    /* Clear ERRI Flag */ 
    hcan->Instance->MSR = CAN_MSR_ERRI; 
    /* Set the CAN state ready to be able to start again the process */
    hcan->State = HAL_CAN_STATE_READY;

    /* Disable interrupts: */
    /*  - Disable Error warning Interrupt */
    /*  - Disable Error passive Interrupt */
    /*  - Disable Bus-off Interrupt */
    /*  - Disable Last error code Interrupt */
    /*  - Disable Error Interrupt */
    /*  - Disable FIFO 0 message pending Interrupt */
    /*  - Disable FIFO 0 Overrun Interrupt */
    /*  - Disable FIFO 1 message pending Interrupt */
    /*  - Disable FIFO 1 Overrun Interrupt */
    /*  - Disable Transmit mailbox empty Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EWG |
                               CAN_IT_EPV |
                               CAN_IT_BOF |
                               CAN_IT_LEC |
                               CAN_IT_ERR |
                               CAN_IT_FMP0|
                               CAN_IT_FOV0|
                               CAN_IT_FMP1|
                               CAN_IT_FOV1|
                               CAN_IT_TME);

    /* Call Error callback function */
    HAL_CAN_ErrorCallback(hcan);
  }  
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Transmission  complete callback in non blocking mode 
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxCpltCallback could be implemented in the user file
   */
}

/**
  * @brief  Error CAN callback.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group3 Peripheral State and Error functions
  *  @brief   CAN Peripheral State functions 
  *
@verbatim   
  ==============================================================================
            ##### Peripheral State and Error functions #####
  ==============================================================================
    [..]
    This subsection provides functions allowing to :
      (+) Check the CAN state.
      (+) Check CAN Errors detected during interrupt process
         
@endverbatim
  * @{
  */

/**
  * @brief  return the CAN state
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL state
  */
HAL_CAN_StateTypeDef HAL_CAN_GetState(CAN_HandleTypeDef* hcan)
{
  /* Return CAN state */
  return hcan->State;
}

/**
  * @brief  Return the CAN error code
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval CAN Error Code
  */
uint32_t HAL_CAN_GetError(CAN_HandleTypeDef *hcan)
{
  return hcan->ErrorCode;
}

/**
  * @}
  */
/**
  * @brief  Initiates and transmits a CAN frame message.
  * @param  hcan: pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @retval HAL status
  */
static HAL_StatusTypeDef CAN_Transmit_IT(CAN_HandleTypeDef* hcan)
{
  /* Disable Transmit mailbox empty Interrupt */
  __HAL_CAN_DISABLE_IT(hcan, CAN_IT_TME);
  
  if(hcan->State == HAL_CAN_STATE_BUSY_TX)
  {   
    /* Disable Error warning, Error passive, Bus-off, Last error code
       and Error Interrupts */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EWG |
                               CAN_IT_EPV |
                               CAN_IT_BOF |
                               CAN_IT_LEC |
                               CAN_IT_ERR );
  }

  /* Change CAN state */
  switch(hcan->State)
  {
    case(HAL_CAN_STATE_BUSY_TX_RX0):
      hcan->State = HAL_CAN_STATE_BUSY_RX0;
      break;
    case(HAL_CAN_STATE_BUSY_TX_RX1):
      hcan->State = HAL_CAN_STATE_BUSY_RX1;
      break;
    case(HAL_CAN_STATE_BUSY_TX_RX0_RX1):
      hcan->State = HAL_CAN_STATE_BUSY_RX0_RX1;
      break;
    default: /* HAL_CAN_STATE_BUSY_TX */
      hcan->State = HAL_CAN_STATE_READY;
      break;
  }

  /* Transmission complete callback */ 
  HAL_CAN_TxCpltCallback(hcan);
  
  return HAL_OK;
}

/**
  * @brief  Receives a correct CAN frame.
  * @param  hcan:       Pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.  
  * @param  FIFONumber: Specify the FIFO number    
  * @retval HAL status
  * @retval None
  */
static HAL_StatusTypeDef CAN_Receive_IT(CAN_HandleTypeDef* hcan, uint8_t FIFONumber)
{
  uint32_t tmp1 = 0U;
  CanRxMsgTypeDef* pRxMsg = NULL;

  /* Set RxMsg pointer */
  if(FIFONumber == CAN_FIFO0)
  {
    pRxMsg = hcan->pRxMsg;
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    pRxMsg = hcan->pRx1Msg;
  }

  /* Get the Id */
  pRxMsg->IDE = (uint8_t)0x04 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  if (pRxMsg->IDE == CAN_ID_STD)
  {
    pRxMsg->StdId = 0x000007FFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 21U);
  }
  else
  {
    pRxMsg->ExtId = 0x1FFFFFFFU & (hcan->Instance->sFIFOMailBox[FIFONumber].RIR >> 3U);
  }
  
  pRxMsg->RTR = (uint8_t)0x02 & hcan->Instance->sFIFOMailBox[FIFONumber].RIR;
  /* Get the DLC */
  pRxMsg->DLC = (uint8_t)0x0F & hcan->Instance->sFIFOMailBox[FIFONumber].RDTR;
  /* Get the FIFONumber */
  pRxMsg->FIFONumber = FIFONumber;
  /* Get the FMI */
  pRxMsg->FMI = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDTR >> 8U);
  /* Get the data field */
  pRxMsg->Data[0] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDLR;
  pRxMsg->Data[1] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 8U);
  pRxMsg->Data[2] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 16U);
  pRxMsg->Data[3] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDLR >> 24U);
  pRxMsg->Data[4] = (uint8_t)0xFF & hcan->Instance->sFIFOMailBox[FIFONumber].RDHR;
  pRxMsg->Data[5] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 8U);
  pRxMsg->Data[6] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 16U);
  pRxMsg->Data[7] = (uint8_t)0xFF & (hcan->Instance->sFIFOMailBox[FIFONumber].RDHR >> 24U);
  /* Release the FIFO */
  /* Release FIFO0 */
  if (FIFONumber == CAN_FIFO0)
  {
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO0);

    /* Disable FIFO 0 overrun and message pending Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_FOV0 | CAN_IT_FMP0);
  }
  /* Release FIFO1 */
  else /* FIFONumber == CAN_FIFO1 */
  {
    __HAL_CAN_FIFO_RELEASE(hcan, CAN_FIFO1);

    /* Disable FIFO 1 overrun and message pending Interrupt */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_FOV1 | CAN_IT_FMP1);
  }

  tmp1 = hcan->State;
  if((tmp1 == HAL_CAN_STATE_BUSY_RX0) || (tmp1 == HAL_CAN_STATE_BUSY_RX1))
  {   
    /* Disable Error warning, Error passive, Bus-off, Last error code
       and Error Interrupts */
    __HAL_CAN_DISABLE_IT(hcan, CAN_IT_EWG |
                               CAN_IT_EPV |
                               CAN_IT_BOF |
                               CAN_IT_LEC |
                               CAN_IT_ERR);
  }

  /* Change CAN state */
  if (FIFONumber == CAN_FIFO0)
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX_RX0):
        hcan->State = HAL_CAN_STATE_BUSY_TX;
        break;
      case(HAL_CAN_STATE_BUSY_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_RX1;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX1;
        break;
      default: /* HAL_CAN_STATE_BUSY_RX0 */
        hcan->State = HAL_CAN_STATE_READY;
        break;
    }
  }
  else /* FIFONumber == CAN_FIFO1 */
  {
    switch(hcan->State)
    {
      case(HAL_CAN_STATE_BUSY_TX_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX;
        break;
      case(HAL_CAN_STATE_BUSY_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_RX0;
        break;
      case(HAL_CAN_STATE_BUSY_TX_RX0_RX1):
        hcan->State = HAL_CAN_STATE_BUSY_TX_RX0;
        break;
      default: /* HAL_CAN_STATE_BUSY_RX1 */
        hcan->State = HAL_CAN_STATE_READY;
        break;
    }
  }

  /* Receive complete callback */ 
  HAL_CAN_RxCpltCallback(hcan);

  /* Return function status */
  return HAL_OK;
}

/**
  * @}
  */
#endif /* STM32F405xx || STM32F415xx || STM32F407xx || STM32F417xx || STM32F427xx || STM32F437xx ||\
          STM32F429xx || STM32F439xx || STM32F446xx || STM32F469xx || STM32F479xx || STM32F412Zx ||\
          STM32F412Vx || STM32F412Rx || STM32F412Cx || STM32F413xx || STM32F423xx */

#endif /* HAL_CAN_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
=======
/**
  ******************************************************************************
  * @file    stm32f4xx_hal_can.c
  * @author  MCD Application Team
  * @brief   CAN HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Controller Area Network (CAN) peripheral:
  *           + Initialization and de-initialization functions
  *           + Configuration functions
  *           + Control functions
  *           + Interrupts management
  *           + Callbacks functions
  *           + Peripheral State and Error functions
  *
  @verbatim
  ==============================================================================
                        ##### How to use this driver #####
  ==============================================================================
    [..]
      (#) Initialize the CAN low level resources by implementing the
          HAL_CAN_MspInit():
         (++) Enable the CAN interface clock using __HAL_RCC_CANx_CLK_ENABLE()
         (++) Configure CAN pins
             (+++) Enable the clock for the CAN GPIOs
             (+++) Configure CAN pins as alternate function open-drain
         (++) In case of using interrupts (e.g. HAL_CAN_ActivateNotification())
             (+++) Configure the CAN interrupt priority using
                   HAL_NVIC_SetPriority()
             (+++) Enable the CAN IRQ handler using HAL_NVIC_EnableIRQ()
             (+++) In CAN IRQ handler, call HAL_CAN_IRQHandler()

      (#) Initialize the CAN peripheral using HAL_CAN_Init() function. This
          function resorts to HAL_CAN_MspInit() for low-level initialization.

      (#) Configure the reception filters using the following configuration
          functions:
            (++) HAL_CAN_ConfigFilter()

      (#) Start the CAN module using HAL_CAN_Start() function. At this level
          the node is active on the bus: it receive messages, and can send
          messages.

      (#) To manage messages transmission, the following Tx control functions
          can be used:
            (++) HAL_CAN_AddTxMessage() to request transmission of a new
                 message.
            (++) HAL_CAN_AbortTxRequest() to abort transmission of a pending
                 message.
            (++) HAL_CAN_GetTxMailboxesFreeLevel() to get the number of free Tx
                 mailboxes.
            (++) HAL_CAN_IsTxMessagePending() to check if a message is pending
                 in a Tx mailbox.
            (++) HAL_CAN_GetTxTimestamp() to get the timestamp of Tx message
                 sent, if time triggered communication mode is enabled.

      (#) When a message is received into the CAN Rx FIFOs, it can be retrieved
          using the HAL_CAN_GetRxMessage() function. The function
          HAL_CAN_GetRxFifoFillLevel() allows to know how many Rx message are
          stored in the Rx Fifo.

      (#) Calling the HAL_CAN_Stop() function stops the CAN module.

      (#) The deinitialization is achieved with HAL_CAN_DeInit() function.


      *** Polling mode operation ***
      ==============================
    [..]
      (#) Reception:
            (++) Monitor reception of message using HAL_CAN_GetRxFifoFillLevel()
                 until at least one message is received.
            (++) Then get the message using HAL_CAN_GetRxMessage().

      (#) Transmission:
            (++) Monitor the Tx mailboxes availability until at least one Tx
                 mailbox is free, using HAL_CAN_GetTxMailboxesFreeLevel().
            (++) Then request transmission of a message using
                 HAL_CAN_AddTxMessage().


      *** Interrupt mode operation ***
      ================================
    [..]
      (#) Notifications are activated using HAL_CAN_ActivateNotification()
          function. Then, the process can be controlled through the
          available user callbacks: HAL_CAN_xxxCallback(), using same APIs
          HAL_CAN_GetRxMessage() and HAL_CAN_AddTxMessage().

      (#) Notifications can be deactivated using
          HAL_CAN_DeactivateNotification() function.

      (#) Special care should be taken for CAN_IT_RX_FIFO0_MSG_PENDING and
          CAN_IT_RX_FIFO1_MSG_PENDING notifications. These notifications trig
          the callbacks HAL_CAN_RxFIFO0MsgPendingCallback() and
          HAL_CAN_RxFIFO1MsgPendingCallback(). User has two possible options
          here.
            (++) Directly get the Rx message in the callback, using
                 HAL_CAN_GetRxMessage().
            (++) Or deactivate the notification in the callback without
                 getting the Rx message. The Rx message can then be got later
                 using HAL_CAN_GetRxMessage(). Once the Rx message have been
                 read, the notification can be activated again.


      *** Sleep mode ***
      ==================
    [..]
      (#) The CAN peripheral can be put in sleep mode (low power), using
          HAL_CAN_RequestSleep(). The sleep mode will be entered as soon as the
          current CAN activity (transmission or reception of a CAN frame) will
          be completed.

      (#) A notification can be activated to be informed when the sleep mode
          will be entered.

      (#) It can be checked if the sleep mode is entered using
          HAL_CAN_IsSleepActive().
          Note that the CAN state (accessible from the API HAL_CAN_GetState())
          is HAL_CAN_STATE_SLEEP_PENDING as soon as the sleep mode request is
          submitted (the sleep mode is not yet entered), and become
          HAL_CAN_STATE_SLEEP_ACTIVE when the sleep mode is effective.

      (#) The wake-up from sleep mode can be trigged by two ways:
            (++) Using HAL_CAN_WakeUp(). When returning from this function,
                 the sleep mode is exited (if return status is HAL_OK).
            (++) When a start of Rx CAN frame is detected by the CAN peripheral,
                 if automatic wake up mode is enabled.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

#if defined(CAN1)

/** @defgroup CAN CAN
  * @brief CAN driver modules
  * @{
  */

#ifdef HAL_CAN_MODULE_ENABLED

#ifdef HAL_CAN_LEGACY_MODULE_ENABLED
  #error "The CAN driver cannot be used with its legacy, Please enable only one CAN module at once"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup CAN_Private_Constants CAN Private Constants
  * @{
  */
#define CAN_TIMEOUT_VALUE 10U
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

/** @defgroup CAN_Exported_Functions CAN Exported Functions
  * @{
  */

/** @defgroup CAN_Exported_Functions_Group1 Initialization and de-initialization functions
 *  @brief    Initialization and Configuration functions
 *
@verbatim
  ==============================================================================
              ##### Initialization and de-initialization functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_CAN_Init                       : Initialize and configure the CAN.
      (+) HAL_CAN_DeInit                     : De-initialize the CAN.
      (+) HAL_CAN_MspInit                    : Initialize the CAN MSP.
      (+) HAL_CAN_MspDeInit                  : DeInitialize the CAN MSP.

@endverbatim
  * @{
  */

/**
  * @brief  Initializes the CAN peripheral according to the specified
  *         parameters in the CAN_InitStruct.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef *hcan)
{
  uint32_t tickstart = 0U;

  /* Check CAN handle */
  if (hcan == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CAN_ALL_INSTANCE(hcan->Instance));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.TimeTriggeredMode));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.AutoBusOff));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.AutoWakeUp));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.AutoRetransmission));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.ReceiveFifoLocked));
  assert_param(IS_FUNCTIONAL_STATE(hcan->Init.TransmitFifoPriority));
  assert_param(IS_CAN_MODE(hcan->Init.Mode));
  assert_param(IS_CAN_SJW(hcan->Init.SyncJumpWidth));
  assert_param(IS_CAN_BS1(hcan->Init.TimeSeg1));
  assert_param(IS_CAN_BS2(hcan->Init.TimeSeg2));
  assert_param(IS_CAN_PRESCALER(hcan->Init.Prescaler));

  if (hcan->State == HAL_CAN_STATE_RESET)
  {
    /* Init the low level hardware: CLOCK, NVIC */
    HAL_CAN_MspInit(hcan);
  }

  /* Exit from sleep mode */
  CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_SLEEP);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Check Sleep mode leave acknowledge */
  while ((hcan->Instance->MSR & CAN_MSR_SLAK) != RESET)
  {
    if ((HAL_GetTick() - tickstart) > CAN_TIMEOUT_VALUE)
    {
      /* Update error code */
      hcan->ErrorCode |= HAL_CAN_ERROR_TIMEOUT;

      /* Change CAN state */
      hcan->State = HAL_CAN_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Request initialisation */
  SET_BIT(hcan->Instance->MCR, CAN_MCR_INRQ);

  /* Get tick */
  tickstart = HAL_GetTick();

  /* Wait initialisation acknowledge */
  while ((hcan->Instance->MSR & CAN_MSR_INAK) == RESET)
  {
    if ((HAL_GetTick() - tickstart) > CAN_TIMEOUT_VALUE)
    {
      /* Update error code */
      hcan->ErrorCode |= HAL_CAN_ERROR_TIMEOUT;

      /* Change CAN state */
      hcan->State = HAL_CAN_STATE_ERROR;

      return HAL_ERROR;
    }
  }

  /* Set the time triggered communication mode */
  if (hcan->Init.TimeTriggeredMode == ENABLE)
  {
    SET_BIT(hcan->Instance->MCR, CAN_MCR_TTCM);
  }
  else
  {
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_TTCM);
  }

  /* Set the automatic bus-off management */
  if (hcan->Init.AutoBusOff == ENABLE)
  {
    SET_BIT(hcan->Instance->MCR, CAN_MCR_ABOM);
  }
  else
  {
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_ABOM);
  }

  /* Set the automatic wake-up mode */
  if (hcan->Init.AutoWakeUp == ENABLE)
  {
    SET_BIT(hcan->Instance->MCR, CAN_MCR_AWUM);
  }
  else
  {
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_AWUM);
  }

  /* Set the automatic retransmission */
  if (hcan->Init.AutoRetransmission == ENABLE)
  {
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_NART);
  }
  else
  {
    SET_BIT(hcan->Instance->MCR, CAN_MCR_NART);
  }

  /* Set the receive FIFO locked mode */
  if (hcan->Init.ReceiveFifoLocked == ENABLE)
  {
    SET_BIT(hcan->Instance->MCR, CAN_MCR_RFLM);
  }
  else
  {
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_RFLM);
  }

  /* Set the transmit FIFO priority */
  if (hcan->Init.TransmitFifoPriority == ENABLE)
  {
    SET_BIT(hcan->Instance->MCR, CAN_MCR_TXFP);
  }
  else
  {
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_TXFP);
  }

  /* Set the bit timing register */
  WRITE_REG(hcan->Instance->BTR, (uint32_t)(hcan->Init.Mode           |
                                            hcan->Init.SyncJumpWidth  |
                                            hcan->Init.TimeSeg1       |
                                            hcan->Init.TimeSeg2       |
                                            (hcan->Init.Prescaler - 1U)));

  /* Initialize the error code */
  hcan->ErrorCode = HAL_CAN_ERROR_NONE;

  /* Initialize the CAN state */
  hcan->State = HAL_CAN_STATE_READY;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Deinitializes the CAN peripheral registers to their default
  *         reset values.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_DeInit(CAN_HandleTypeDef *hcan)
{
  /* Check CAN handle */
  if (hcan == NULL)
  {
    return HAL_ERROR;
  }

  /* Check the parameters */
  assert_param(IS_CAN_ALL_INSTANCE(hcan->Instance));

  /* Stop the CAN module */
  HAL_CAN_Stop(hcan);

  /* DeInit the low level hardware: CLOCK, NVIC */
  HAL_CAN_MspDeInit(hcan);

  /* Reset the CAN peripheral */
  SET_BIT(hcan->Instance->MCR, CAN_MCR_RESET);

  /* Reset the CAN ErrorCode */
  hcan->ErrorCode = HAL_CAN_ERROR_NONE;

  /* Change CAN state */
  hcan->State = HAL_CAN_STATE_RESET;

  /* Return function status */
  return HAL_OK;
}

/**
  * @brief  Initializes the CAN MSP.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_MspInit could be implemented in the user file
   */
}

/**
  * @brief  DeInitializes the CAN MSP.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_MspDeInit could be implemented in the user file
   */
}


/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group2 Configuration functions
 *  @brief    Configuration functions.
 *
@verbatim
  ==============================================================================
              ##### Configuration functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_CAN_ConfigFilter            : Configure the CAN reception filters

@endverbatim
  * @{
  */

/**
  * @brief  Configures the CAN reception filter according to the specified
  *         parameters in the CAN_FilterInitStruct.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  sFilterConfig pointer to a CAN_FilterTypeDef structure that
  *         contains the filter configuration information.
  * @retval None
  */
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan, CAN_FilterTypeDef *sFilterConfig)
{
  uint32_t filternbrbitpos = 0U;
  CAN_TypeDef *can_ip = hcan->Instance;

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check the parameters */
    assert_param(IS_CAN_FILTER_ID_HALFWORD(sFilterConfig->FilterIdHigh));
    assert_param(IS_CAN_FILTER_ID_HALFWORD(sFilterConfig->FilterIdLow));
    assert_param(IS_CAN_FILTER_ID_HALFWORD(sFilterConfig->FilterMaskIdHigh));
    assert_param(IS_CAN_FILTER_ID_HALFWORD(sFilterConfig->FilterMaskIdLow));
    assert_param(IS_CAN_FILTER_MODE(sFilterConfig->FilterMode));
    assert_param(IS_CAN_FILTER_SCALE(sFilterConfig->FilterScale));
    assert_param(IS_CAN_FILTER_FIFO(sFilterConfig->FilterFIFOAssignment));
    assert_param(IS_FUNCTIONAL_STATE(sFilterConfig->FilterActivation));

#if defined(CAN3)
    /* Check the CAN instance */
    if (hcan->Instance == CAN3)
    {
      /* CAN3 is single instance with 14 dedicated filters banks */

      /* Check the parameters */
      assert_param(IS_CAN_FILTER_BANK_SINGLE(sFilterConfig->FilterBank));
    }
    else
    {
      /* CAN1 and CAN2 are dual instances with 28 common filters banks */
      /* Select master instance to access the filter banks */
      can_ip = CAN1;

      /* Check the parameters */
      assert_param(IS_CAN_FILTER_BANK_DUAL(sFilterConfig->FilterBank));
      assert_param(IS_CAN_FILTER_BANK_DUAL(sFilterConfig->SlaveStartFilterBank));
    }
#elif defined(CAN2)
    /* CAN1 and CAN2 are dual instances with 28 common filters banks */
    /* Select master instance to access the filter banks */
    can_ip = CAN1;

    /* Check the parameters */
    assert_param(IS_CAN_FILTER_BANK_DUAL(sFilterConfig->FilterBank));
    assert_param(IS_CAN_FILTER_BANK_DUAL(sFilterConfig->SlaveStartFilterBank));
#else
    /* CAN1 is single instance with 14 dedicated filters banks */

    /* Check the parameters */
    assert_param(IS_CAN_FILTER_BANK_SINGLE(sFilterConfig->FilterBank));
#endif

    /* Initialisation mode for the filter */
    SET_BIT(can_ip->FMR, CAN_FMR_FINIT);

#if defined(CAN3)
    /* Check the CAN instance */
    if (can_ip == CAN1)
    {
      /* Select the start filter number of CAN2 slave instance */
      CLEAR_BIT(can_ip->FMR, CAN_FMR_CAN2SB);
      SET_BIT(can_ip->FMR, sFilterConfig->SlaveStartFilterBank << CAN_FMR_CAN2SB_Pos);
    }

#elif defined(CAN2)
    /* Select the start filter number of CAN2 slave instance */
    CLEAR_BIT(can_ip->FMR, CAN_FMR_CAN2SB);
    SET_BIT(can_ip->FMR, sFilterConfig->SlaveStartFilterBank << CAN_FMR_CAN2SB_Pos);

#endif
    /* Convert filter number into bit position */
    filternbrbitpos = (1U) << sFilterConfig->FilterBank;

    /* Filter Deactivation */
    CLEAR_BIT(can_ip->FA1R, filternbrbitpos);

    /* Filter Scale */
    if (sFilterConfig->FilterScale == CAN_FILTERSCALE_16BIT)
    {
      /* 16-bit scale for the filter */
      CLEAR_BIT(can_ip->FS1R, filternbrbitpos);

      /* First 16-bit identifier and First 16-bit mask */
      /* Or First 16-bit identifier and Second 16-bit identifier */
      can_ip->sFilterRegister[sFilterConfig->FilterBank].FR1 =
        ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdLow) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdLow);

      /* Second 16-bit identifier and Second 16-bit mask */
      /* Or Third 16-bit identifier and Fourth 16-bit identifier */
      can_ip->sFilterRegister[sFilterConfig->FilterBank].FR2 =
        ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdHigh);
    }

    if (sFilterConfig->FilterScale == CAN_FILTERSCALE_32BIT)
    {
      /* 32-bit scale for the filter */
      SET_BIT(can_ip->FS1R, filternbrbitpos);

      /* 32-bit identifier or First 32-bit identifier */
      can_ip->sFilterRegister[sFilterConfig->FilterBank].FR1 =
        ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdHigh) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterIdLow);

      /* 32-bit mask or Second 32-bit identifier */
      can_ip->sFilterRegister[sFilterConfig->FilterBank].FR2 =
        ((0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdHigh) << 16U) |
        (0x0000FFFFU & (uint32_t)sFilterConfig->FilterMaskIdLow);
    }

    /* Filter Mode */
    if (sFilterConfig->FilterMode == CAN_FILTERMODE_IDMASK)
    {
      /* Id/Mask mode for the filter*/
      CLEAR_BIT(can_ip->FM1R, filternbrbitpos);
    }
    else /* CAN_FilterInitStruct->CAN_FilterMode == CAN_FilterMode_IdList */
    {
      /* Identifier list mode for the filter*/
      SET_BIT(can_ip->FM1R, filternbrbitpos);
    }

    /* Filter FIFO assignment */
    if (sFilterConfig->FilterFIFOAssignment == CAN_FILTER_FIFO0)
    {
      /* FIFO 0 assignation for the filter */
      CLEAR_BIT(can_ip->FFA1R, filternbrbitpos);
    }
    else
    {
      /* FIFO 1 assignation for the filter */
      SET_BIT(can_ip->FFA1R, filternbrbitpos);
    }

    /* Filter activation */
    if (sFilterConfig->FilterActivation == ENABLE)
    {
      SET_BIT(can_ip->FA1R, filternbrbitpos);
    }

    /* Leave the initialisation mode for the filter */
    CLEAR_BIT(can_ip->FMR, CAN_FMR_FINIT);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group3 Control functions
 *  @brief    Control functions
 *
@verbatim
  ==============================================================================
                      ##### Control functions #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_CAN_Start                    : Start the CAN module
      (+) HAL_CAN_Stop                     : Stop the CAN module
      (+) HAL_CAN_RequestSleep             : Request sleep mode entry.
      (+) HAL_CAN_WakeUp                   : Wake up from sleep mode.
      (+) HAL_CAN_IsSleepActive            : Check is sleep mode is active.
      (+) HAL_CAN_AddTxMessage             : Add a message to the Tx mailboxes
                                             and activate the corresponding
                                             transmission request
      (+) HAL_CAN_AbortTxRequest           : Abort transmission request
      (+) HAL_CAN_GetTxMailboxesFreeLevel  : Return Tx mailboxes free level
      (+) HAL_CAN_IsTxMessagePending       : Check if a transmission request is
                                             pending on the selected Tx mailbox
      (+) HAL_CAN_GetRxMessage             : Get a CAN frame from the Rx FIFO
      (+) HAL_CAN_GetRxFifoFillLevel       : Return Rx FIFO fill level

@endverbatim
  * @{
  */

/**
  * @brief  Start the CAN module.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Start(CAN_HandleTypeDef *hcan)
{
  uint32_t tickstart = 0U;

  if (hcan->State == HAL_CAN_STATE_READY)
  {
    /* Change CAN peripheral state */
    hcan->State = HAL_CAN_STATE_LISTENING;

    /* Request leave initialisation */
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_INRQ);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait the acknowledge */
    while ((hcan->Instance->MSR & CAN_MSR_INAK) != RESET)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart) > CAN_TIMEOUT_VALUE)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_TIMEOUT;

        /* Change CAN state */
        hcan->State = HAL_CAN_STATE_ERROR;

        return HAL_ERROR;
      }
    }

    /* Reset the CAN ErrorCode */
    hcan->ErrorCode = HAL_CAN_ERROR_NONE;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_READY;

    return HAL_ERROR;
  }
}

/**
  * @brief  Stop the CAN module and enable access to configuration registers.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_Stop(CAN_HandleTypeDef *hcan)
{
  uint32_t tickstart = 0U;

  if (hcan->State == HAL_CAN_STATE_LISTENING)
  {
    /* Request initialisation */
    SET_BIT(hcan->Instance->MCR, CAN_MCR_INRQ);

    /* Get tick */
    tickstart = HAL_GetTick();

    /* Wait the acknowledge */
    while ((hcan->Instance->MSR & CAN_MSR_INAK) == RESET)
    {
      /* Check for the Timeout */
      if ((HAL_GetTick() - tickstart) > CAN_TIMEOUT_VALUE)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_TIMEOUT;

        /* Change CAN state */
        hcan->State = HAL_CAN_STATE_ERROR;

        return HAL_ERROR;
      }
    }

    /* Exit from sleep mode */
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_SLEEP);

    /* Change CAN peripheral state */
    hcan->State = HAL_CAN_STATE_READY;

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_STARTED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Request the sleep mode (low power) entry.
  *         When returning from this function, Sleep mode will be entered
  *         as soon as the current CAN activity (transmission or reception
  *         of a CAN frame) has been completed.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_CAN_RequestSleep(CAN_HandleTypeDef *hcan)
{
  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Request Sleep mode */
    SET_BIT(hcan->Instance->MCR, CAN_MCR_SLEEP);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    /* Return function status */
    return HAL_ERROR;
  }
}

/**
  * @brief  Wake up from sleep mode.
  *         When returning with HAL_OK status from this function, Sleep mode
  *         is exited.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status.
  */
HAL_StatusTypeDef HAL_CAN_WakeUp(CAN_HandleTypeDef *hcan)
{
  __IO uint32_t count = 0;
  uint32_t timeout = 1000000U;

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Wake up request */
    CLEAR_BIT(hcan->Instance->MCR, CAN_MCR_SLEEP);

    /* Wait sleep mode is exited */
    do
    {
      /* Check if timeout is reached */
      if (++count > timeout)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_TIMEOUT;

        return HAL_ERROR;
      }
    }
    while ((hcan->Instance->MSR & CAN_MSR_SLAK) != RESET);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Check is sleep mode is active.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval Status
  *          - 0 : Sleep mode is not active.
  *          - 1 : Sleep mode is active.
  */
uint32_t HAL_CAN_IsSleepActive(CAN_HandleTypeDef *hcan)
{
  uint32_t status = 0U;

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check Sleep mode */
    if ((hcan->Instance->MSR & CAN_MSR_SLAK) != RESET)
    {
      status = 1U;
    }
  }

  /* Return function status */
  return status;
}

/**
  * @brief  Add a message to the first free Tx mailbox and activate the
  *         corresponding transmission request.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  pHeader pointer to a CAN_TxHeaderTypeDef structure.
  * @param  aData array containing the payload of the Tx frame.
  * @param  pTxMailbox pointer to a variable where the function will return
  *         the TxMailbox used to store the Tx message.
  *         This parameter can be a value of @arg CAN_Tx_Mailboxes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan, CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox)
{
  uint32_t transmitmailbox;

  /* Check the parameters */
  assert_param(IS_CAN_IDTYPE(pHeader->IDE));
  assert_param(IS_CAN_RTR(pHeader->RTR));
  assert_param(IS_CAN_DLC(pHeader->DLC));
  if (pHeader->IDE == CAN_ID_STD)
  {
    assert_param(IS_CAN_STDID(pHeader->StdId));
  }
  else
  {
    assert_param(IS_CAN_EXTID(pHeader->ExtId));
  }
  assert_param(IS_FUNCTIONAL_STATE(pHeader->TransmitGlobalTime));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check that all the Tx mailboxes are not full */
    if (((hcan->Instance->TSR & CAN_TSR_TME0) != RESET) ||
        ((hcan->Instance->TSR & CAN_TSR_TME1) != RESET) ||
        ((hcan->Instance->TSR & CAN_TSR_TME2) != RESET))
    {
      /* Select an empty transmit mailbox */
      transmitmailbox = (hcan->Instance->TSR & CAN_TSR_CODE) >> CAN_TSR_CODE_Pos;

      /* Store the Tx mailbox */
      *pTxMailbox = 1U << transmitmailbox;

      /* Set up the Id */
      if (pHeader->IDE == CAN_ID_STD)
      {
        hcan->Instance->sTxMailBox[transmitmailbox].TIR = ((pHeader->StdId << CAN_TI0R_STID_Pos) |
                                                           pHeader->RTR);
      }
      else
      {
        hcan->Instance->sTxMailBox[transmitmailbox].TIR = ((pHeader->ExtId << CAN_TI0R_EXID_Pos) |
                                                           pHeader->IDE |
                                                           pHeader->RTR);
      }

      /* Set up the DLC */
      hcan->Instance->sTxMailBox[transmitmailbox].TDTR = (pHeader->DLC);

      /* Set up the Transmit Global Time mode */
      if (pHeader->TransmitGlobalTime == ENABLE)
      {
        SET_BIT(hcan->Instance->sTxMailBox[transmitmailbox].TDTR, CAN_TDT0R_TGT);
      }

      /* Set up the data field */
      WRITE_REG(hcan->Instance->sTxMailBox[transmitmailbox].TDHR,
                ((uint32_t)aData[7] << CAN_TDH0R_DATA7_Pos) |
                ((uint32_t)aData[6] << CAN_TDH0R_DATA6_Pos) |
                ((uint32_t)aData[5] << CAN_TDH0R_DATA5_Pos) |
                ((uint32_t)aData[4] << CAN_TDH0R_DATA4_Pos));
      WRITE_REG(hcan->Instance->sTxMailBox[transmitmailbox].TDLR,
                ((uint32_t)aData[3] << CAN_TDL0R_DATA3_Pos) |
                ((uint32_t)aData[2] << CAN_TDL0R_DATA2_Pos) |
                ((uint32_t)aData[1] << CAN_TDL0R_DATA1_Pos) |
                ((uint32_t)aData[0] << CAN_TDL0R_DATA0_Pos));

      /* Request transmission */
      SET_BIT(hcan->Instance->sTxMailBox[transmitmailbox].TIR, CAN_TI0R_TXRQ);

      /* Return function status */
      return HAL_OK;
    }
    else
    {
      /* Update error code */
      hcan->ErrorCode |= HAL_CAN_ERROR_PARAM;

      return HAL_ERROR;
    }
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Abort transmission requests
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  TxMailboxes List of the Tx Mailboxes to abort.
  *         This parameter can be any combination of @arg CAN_Tx_Mailboxes.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_AbortTxRequest(CAN_HandleTypeDef *hcan, uint32_t TxMailboxes)
{
  /* Check function parameters */
  assert_param(IS_CAN_TX_MAILBOX_LIST(TxMailboxes));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check Tx Mailbox 0 */
    if ((TxMailboxes & CAN_TX_MAILBOX0) != RESET)
    {
      /* Add cancellation request for Tx Mailbox 0 */
      SET_BIT(hcan->Instance->TSR, CAN_TSR_ABRQ0);
    }

    /* Check Tx Mailbox 1 */
    if ((TxMailboxes & CAN_TX_MAILBOX1) != RESET)
    {
      /* Add cancellation request for Tx Mailbox 1 */
      SET_BIT(hcan->Instance->TSR, CAN_TSR_ABRQ1);
    }

    /* Check Tx Mailbox 2 */
    if ((TxMailboxes & CAN_TX_MAILBOX2) != RESET)
    {
      /* Add cancellation request for Tx Mailbox 2 */
      SET_BIT(hcan->Instance->TSR, CAN_TSR_ABRQ2);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Return Tx Mailboxes free level: number of free Tx Mailboxes.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval Number of free Tx Mailboxes.
  */
uint32_t HAL_CAN_GetTxMailboxesFreeLevel(CAN_HandleTypeDef *hcan)
{
  uint32_t freelevel = 0U;

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check Tx Mailbox 0 status */
    if ((hcan->Instance->TSR & CAN_TSR_TME0) != RESET)
    {
      freelevel++;
    }

    /* Check Tx Mailbox 1 status */
    if ((hcan->Instance->TSR & CAN_TSR_TME1) != RESET)
    {
      freelevel++;
    }

    /* Check Tx Mailbox 2 status */
    if ((hcan->Instance->TSR & CAN_TSR_TME2) != RESET)
    {
      freelevel++;
    }
  }

  /* Return Tx Mailboxes free level */
  return freelevel;
}

/**
  * @brief  Check if a transmission request is pending on the selected Tx
  *         Mailboxes.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  TxMailboxes List of Tx Mailboxes to check.
  *         This parameter can be any combination of @arg CAN_Tx_Mailboxes.
  * @retval Status
  *          - 0 : No pending transmission request on any selected Tx Mailboxes.
  *          - 1 : Pending transmission request on at least one of the selected
  *                Tx Mailbox.
  */
uint32_t HAL_CAN_IsTxMessagePending(CAN_HandleTypeDef *hcan, uint32_t TxMailboxes)
{
  uint32_t status = 0U;

  /* Check function parameters */
  assert_param(IS_CAN_TX_MAILBOX_LIST(TxMailboxes));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check pending transmission request on the selected Tx Mailboxes */
    if ((hcan->Instance->TSR & (TxMailboxes << CAN_TSR_TME0_Pos)) != (TxMailboxes << CAN_TSR_TME0_Pos))
    {
      status = 1U;
    }
  }

  /* Return status */
  return status;
}

/**
  * @brief  Return timestamp of Tx message sent, if time triggered communication
            mode is enabled.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  TxMailbox Tx Mailbox where the timestamp of message sent will be
  *         read.
  *         This parameter can be one value of @arg CAN_Tx_Mailboxes.
  * @retval Timestamp of message sent from Tx Mailbox.
  */
uint32_t HAL_CAN_GetTxTimestamp(CAN_HandleTypeDef *hcan, uint32_t TxMailbox)
{
  uint32_t timestamp = 0U;
  uint32_t transmitmailbox;

  /* Check function parameters */
  assert_param(IS_CAN_TX_MAILBOX(TxMailbox));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Select the Tx mailbox */
    transmitmailbox = POSITION_VAL(TxMailbox);

    /* Get timestamp */
    timestamp = (hcan->Instance->sTxMailBox[transmitmailbox].TDTR & CAN_TDT0R_TIME) >> CAN_TDT0R_TIME_Pos;
  }

  /* Return the timestamp */
  return timestamp;
}

/**
  * @brief  Get an CAN frame from the Rx FIFO zone into the message RAM.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  RxFifo Fifo number of the received message to be read.
  *         This parameter can be a value of @arg CAN_receive_FIFO_number.
  * @param  pHeader pointer to a CAN_RxHeaderTypeDef structure where the header
  *         of the Rx frame will be stored.
  * @param  aData array where the payload of the Rx frame will be stored.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan, uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[])
{
  assert_param(IS_CAN_RX_FIFO(RxFifo));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check the Rx FIFO */
    if (RxFifo == CAN_RX_FIFO0) /* Rx element is assigned to Rx FIFO 0 */
    {
      /* Check that the Rx FIFO 0 is not empty */
      if ((hcan->Instance->RF0R & CAN_RF0R_FMP0) == RESET)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_PARAM;

        return HAL_ERROR;
      }
    }
    else if (RxFifo == CAN_RX_FIFO1) /* Rx element is assigned to Rx FIFO 1 */
    {
      /* Check that the Rx FIFO 1 is not empty */
      if ((hcan->Instance->RF1R & CAN_RF1R_FMP1) == RESET)
      {
        /* Update error code */
        hcan->ErrorCode |= HAL_CAN_ERROR_PARAM;

        return HAL_ERROR;
      }
    }

    /* Get the header */
    pHeader->IDE = CAN_RI0R_IDE & hcan->Instance->sFIFOMailBox[RxFifo].RIR;
    if (pHeader->IDE == CAN_ID_STD)
    {
      pHeader->StdId = (CAN_RI0R_STID & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_TI0R_STID_Pos;
    }
    else
    {
      pHeader->ExtId = ((CAN_RI0R_EXID | CAN_RI0R_STID) & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_RI0R_EXID_Pos;
    }
    pHeader->RTR = (CAN_RI0R_RTR & hcan->Instance->sFIFOMailBox[RxFifo].RIR) >> CAN_RI0R_RTR_Pos;
    pHeader->DLC = (CAN_RDT0R_DLC & hcan->Instance->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_DLC_Pos;
    pHeader->FilterMatchIndex = (CAN_RDT0R_FMI & hcan->Instance->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_FMI_Pos;
    pHeader->Timestamp = (CAN_RDT0R_TIME & hcan->Instance->sFIFOMailBox[RxFifo].RDTR) >> CAN_RDT0R_TIME_Pos;

    /* Get the data */
    aData[0] = (CAN_RDL0R_DATA0 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA0_Pos;
    aData[1] = (CAN_RDL0R_DATA1 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA1_Pos;
    aData[2] = (CAN_RDL0R_DATA2 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA2_Pos;
    aData[3] = (CAN_RDL0R_DATA3 & hcan->Instance->sFIFOMailBox[RxFifo].RDLR) >> CAN_RDL0R_DATA3_Pos;
    aData[4] = (CAN_RDH0R_DATA4 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA4_Pos;
    aData[5] = (CAN_RDH0R_DATA5 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA5_Pos;
    aData[6] = (CAN_RDH0R_DATA6 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA6_Pos;
    aData[7] = (CAN_RDH0R_DATA7 & hcan->Instance->sFIFOMailBox[RxFifo].RDHR) >> CAN_RDH0R_DATA7_Pos;

    /* Release the FIFO */
    if (RxFifo == CAN_RX_FIFO0) /* Rx element is assigned to Rx FIFO 0 */
    {
      /* Release RX FIFO 0 */
      SET_BIT(hcan->Instance->RF0R, CAN_RF0R_RFOM0);
    }
    else if (RxFifo == CAN_RX_FIFO1) /* Rx element is assigned to Rx FIFO 1 */
    {
      /* Release RX FIFO 1 */
      SET_BIT(hcan->Instance->RF1R, CAN_RF1R_RFOM1);
    }

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Return Rx FIFO fill level.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  RxFifo Rx FIFO.
  *         This parameter can be a value of @arg CAN_receive_FIFO_number.
  * @retval Number of messages available in Rx FIFO.
  */
uint32_t HAL_CAN_GetRxFifoFillLevel(CAN_HandleTypeDef *hcan, uint32_t RxFifo)
{
  uint32_t filllevel = 0U;

  /* Check function parameters */
  assert_param(IS_CAN_RX_FIFO(RxFifo));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    if (RxFifo == CAN_RX_FIFO0)
    {
      filllevel = hcan->Instance->RF0R & CAN_RF0R_FMP0;
    }
    else /* RxFifo == CAN_RX_FIFO1 */
    {
      filllevel = hcan->Instance->RF1R & CAN_RF1R_FMP1;
    }
  }

  /* Return Rx FIFO fill level */
  return filllevel;
}

/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group4 Interrupts management
 *  @brief    Interrupts management
 *
@verbatim
  ==============================================================================
                       ##### Interrupts management #####
  ==============================================================================
    [..]  This section provides functions allowing to:
      (+) HAL_CAN_ActivateNotification      : Enable interrupts
      (+) HAL_CAN_DeactivateNotification    : Disable interrupts
      (+) HAL_CAN_IRQHandler                : Handles CAN interrupt request

@endverbatim
  * @{
  */

/**
  * @brief  Enable interrupts.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  ActiveITs indicates which interrupts will be enabled.
  *         This parameter can be any combination of @arg CAN_Interrupts.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_ActivateNotification(CAN_HandleTypeDef *hcan, uint32_t ActiveITs)
{
  /* Check function parameters */
  assert_param(IS_CAN_IT(ActiveITs));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Enable the selected interrupts */
    __HAL_CAN_ENABLE_IT(hcan, ActiveITs);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Disable interrupts.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @param  InactiveITs indicates which interrupts will be disabled.
  *         This parameter can be any combination of @arg CAN_Interrupts.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_DeactivateNotification(CAN_HandleTypeDef *hcan, uint32_t InactiveITs)
{
  /* Check function parameters */
  assert_param(IS_CAN_IT(InactiveITs));

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Disable the selected interrupts */
    __HAL_CAN_DISABLE_IT(hcan, InactiveITs);

    /* Return function status */
    return HAL_OK;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    return HAL_ERROR;
  }
}

/**
  * @brief  Handles CAN interrupt request
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
void HAL_CAN_IRQHandler(CAN_HandleTypeDef *hcan)
{
  uint32_t errorcode = HAL_CAN_ERROR_NONE;
  uint32_t interrupts = READ_REG(hcan->Instance->IER);
  uint32_t msrflags = READ_REG(hcan->Instance->MSR);
  uint32_t tsrflags = READ_REG(hcan->Instance->TSR);
  uint32_t rf0rflags = READ_REG(hcan->Instance->RF0R);
  uint32_t rf1rflags = READ_REG(hcan->Instance->RF1R);
  uint32_t esrflags = READ_REG(hcan->Instance->ESR);

  /* Transmit Mailbox empty interrupt management *****************************/
  if ((interrupts & CAN_IT_TX_MAILBOX_EMPTY) != RESET)
  {
    /* Transmit Mailbox 0 management *****************************************/
    if ((tsrflags & CAN_TSR_RQCP0) != RESET)
    {
      /* Clear the Transmission Complete flag (and TXOK0,ALST0,TERR0 bits) */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_RQCP0);

      if ((tsrflags & CAN_TSR_TXOK0) != RESET)
      {
        /* Transmission Mailbox 0 complete callback */
        /* Call weak (surcharged) callback */
        HAL_CAN_TxMailbox0CompleteCallback(hcan);
      }
      else
      {
        if ((tsrflags & CAN_TSR_ALST0) != RESET)
        {
          /* Update error code */
          errorcode |= HAL_CAN_ERROR_TX_ALST0;
        }
        else if ((tsrflags & CAN_TSR_TERR0) != RESET)
        {
          /* Update error code */
          errorcode |= HAL_CAN_ERROR_TX_TERR0;
        }
        else
        {
          /* Transmission Mailbox 0 abort callback */
          /* Call weak (surcharged) callback */
          HAL_CAN_TxMailbox0AbortCallback(hcan);
        }
      }
    }

    /* Transmit Mailbox 1 management *****************************************/
    if ((tsrflags & CAN_TSR_RQCP1) != RESET)
    {
      /* Clear the Transmission Complete flag (and TXOK1,ALST1,TERR1 bits) */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_RQCP1);

      if ((tsrflags & CAN_TSR_TXOK1) != RESET)
      {
        /* Transmission Mailbox 1 complete callback */
        /* Call weak (surcharged) callback */
        HAL_CAN_TxMailbox1CompleteCallback(hcan);
      }
      else
      {
        if ((tsrflags & CAN_TSR_ALST1) != RESET)
        {
          /* Update error code */
          errorcode |= HAL_CAN_ERROR_TX_ALST1;
        }
        else if ((tsrflags & CAN_TSR_TERR1) != RESET)
        {
          /* Update error code */
          errorcode |= HAL_CAN_ERROR_TX_TERR1;
        }
        else
        {
          /* Transmission Mailbox 1 abort callback */
          /* Call weak (surcharged) callback */
          HAL_CAN_TxMailbox1AbortCallback(hcan);
        }
      }
    }

    /* Transmit Mailbox 2 management *****************************************/
    if ((tsrflags & CAN_TSR_RQCP2) != RESET)
    {
      /* Clear the Transmission Complete flag (and TXOK2,ALST2,TERR2 bits) */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_RQCP2);

      if ((tsrflags & CAN_TSR_TXOK2) != RESET)
      {
        /* Transmission Mailbox 2 complete callback */
        /* Call weak (surcharged) callback */
        HAL_CAN_TxMailbox2CompleteCallback(hcan);
      }
      else
      {
        if ((tsrflags & CAN_TSR_ALST2) != RESET)
        {
          /* Update error code */
          errorcode |= HAL_CAN_ERROR_TX_ALST2;
        }
        else if ((tsrflags & CAN_TSR_TERR2) != RESET)
        {
          /* Update error code */
          errorcode |= HAL_CAN_ERROR_TX_TERR2;
        }
        else
        {
          /* Transmission Mailbox 2 abort callback */
          /* Call weak (surcharged) callback */
          HAL_CAN_TxMailbox2AbortCallback(hcan);
        }
      }
    }
  }

  /* Receive FIFO 0 overrun interrupt management *****************************/
  if ((interrupts & CAN_IT_RX_FIFO0_OVERRUN) != RESET)
  {
    if ((rf0rflags & CAN_RF0R_FOVR0) != RESET)
    {
      /* Set CAN error code to Rx Fifo 0 overrun error */
      errorcode |= HAL_CAN_ERROR_RX_FOV0;

      /* Clear FIFO0 Overrun Flag */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV0);
    }
  }

  /* Receive FIFO 0 full interrupt management ********************************/
  if ((interrupts & CAN_IT_RX_FIFO0_FULL) != RESET)
  {
    if ((rf0rflags & CAN_RF0R_FULL0) != RESET)
    {
      /* Clear FIFO 0 full Flag */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FF0);

      /* Receive FIFO 0 full Callback */
      /* Call weak (surcharged) callback */
      HAL_CAN_RxFifo0FullCallback(hcan);
    }
  }

  /* Receive FIFO 0 message pending interrupt management *********************/
  if ((interrupts & CAN_IT_RX_FIFO0_MSG_PENDING) != RESET)
  {
    /* Check if message is still pending */
    if ((hcan->Instance->RF0R & CAN_RF0R_FMP0) != RESET)
    {
      /* Receive FIFO 0 mesage pending Callback */
      /* Call weak (surcharged) callback */
      HAL_CAN_RxFifo0MsgPendingCallback(hcan);
    }
  }

  /* Receive FIFO 1 overrun interrupt management *****************************/
  if ((interrupts & CAN_IT_RX_FIFO1_OVERRUN) != RESET)
  {
    if ((rf1rflags & CAN_RF1R_FOVR1) != RESET)
    {
      /* Set CAN error code to Rx Fifo 1 overrun error */
      errorcode |= HAL_CAN_ERROR_RX_FOV1;

      /* Clear FIFO1 Overrun Flag */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FOV1);
    }
  }

  /* Receive FIFO 1 full interrupt management ********************************/
  if ((interrupts & CAN_IT_RX_FIFO1_FULL) != RESET)
  {
    if ((rf1rflags & CAN_RF1R_FULL1) != RESET)
    {
      /* Clear FIFO 1 full Flag */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_FF1);

      /* Receive FIFO 1 full Callback */
      /* Call weak (surcharged) callback */
      HAL_CAN_RxFifo1FullCallback(hcan);
    }
  }

  /* Receive FIFO 1 message pending interrupt management *********************/
  if ((interrupts & CAN_IT_RX_FIFO1_MSG_PENDING) != RESET)
  {
    /* Check if message is still pending */
    if ((hcan->Instance->RF1R & CAN_RF1R_FMP1) != RESET)
    {
      /* Receive FIFO 1 mesage pending Callback */
      /* Call weak (surcharged) callback */
      HAL_CAN_RxFifo1MsgPendingCallback(hcan);
    }
  }

  /* Sleep interrupt management *********************************************/
  if ((interrupts & CAN_IT_SLEEP_ACK) != RESET)
  {
    if ((msrflags & CAN_MSR_SLAKI) != RESET)
    {
      /* Clear Sleep interrupt Flag */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_SLAKI);

      /* Sleep Callback */
      /* Call weak (surcharged) callback */
      HAL_CAN_SleepCallback(hcan);
    }
  }

  /* WakeUp interrupt management *********************************************/
  if ((interrupts & CAN_IT_WAKEUP) != RESET)
  {
    if ((msrflags & CAN_MSR_WKUI) != RESET)
    {
      /* Clear WakeUp Flag */
      __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_WKU);

      /* WakeUp Callback */
      /* Call weak (surcharged) callback */
      HAL_CAN_WakeUpFromRxMsgCallback(hcan);
    }
  }

  /* Error interrupts management *********************************************/
  if ((interrupts & CAN_IT_ERROR) != RESET)
  {
    if ((msrflags & CAN_MSR_ERRI) != RESET)
    {
      /* Check Error Warning Flag */
      if (((interrupts & CAN_IT_ERROR_WARNING) != RESET) &&
          ((esrflags & CAN_ESR_EWGF) != RESET))
      {
        /* Set CAN error code to Error Warning */
        errorcode |= HAL_CAN_ERROR_EWG;

        /* No need for clear of Error Warning Flag as read-only */
      }

      /* Check Error Passive Flag */
      if (((interrupts & CAN_IT_ERROR_PASSIVE) != RESET) &&
          ((esrflags & CAN_ESR_EPVF) != RESET))
      {
        /* Set CAN error code to Error Passive */
        errorcode |= HAL_CAN_ERROR_EPV;

        /* No need for clear of Error Passive Flag as read-only */
      }

      /* Check Bus-off Flag */
      if (((interrupts & CAN_IT_BUSOFF) != RESET) &&
          ((esrflags & CAN_ESR_BOFF) != RESET))
      {
        /* Set CAN error code to Bus-Off */
        errorcode |= HAL_CAN_ERROR_BOF;

        /* No need for clear of Error Bus-Off as read-only */
      }

      /* Check Last Error Code Flag */
      if (((interrupts & CAN_IT_LAST_ERROR_CODE) != RESET) &&
          ((esrflags & CAN_ESR_LEC) != RESET))
      {
        switch (esrflags & CAN_ESR_LEC)
        {
          case (CAN_ESR_LEC_0):
            /* Set CAN error code to Stuff error */
            errorcode |= HAL_CAN_ERROR_STF;
            break;
          case (CAN_ESR_LEC_1):
            /* Set CAN error code to Form error */
            errorcode |= HAL_CAN_ERROR_FOR;
            break;
          case (CAN_ESR_LEC_1 | CAN_ESR_LEC_0):
            /* Set CAN error code to Acknowledgement error */
            errorcode |= HAL_CAN_ERROR_ACK;
            break;
          case (CAN_ESR_LEC_2):
            /* Set CAN error code to Bit recessive error */
            errorcode |= HAL_CAN_ERROR_BR;
            break;
          case (CAN_ESR_LEC_2 | CAN_ESR_LEC_0):
            /* Set CAN error code to Bit Dominant error */
            errorcode |= HAL_CAN_ERROR_BD;
            break;
          case (CAN_ESR_LEC_2 | CAN_ESR_LEC_1):
            /* Set CAN error code to CRC error */
            errorcode |= HAL_CAN_ERROR_CRC;
            break;
          default:
            break;
        }

        /* Clear Last error code Flag */
        CLEAR_BIT(hcan->Instance->ESR, CAN_ESR_LEC);
      }
    }

    /* Clear ERRI Flag */
    __HAL_CAN_CLEAR_FLAG(hcan, CAN_FLAG_ERRI);
  }

  /* Call the Error call Back in case of Errors */
  if (errorcode != HAL_CAN_ERROR_NONE)
  {
    /* Update error code in handle */
    hcan->ErrorCode |= errorcode;

    /* Call Error callback function */
    /* Call weak (surcharged) callback */
    HAL_CAN_ErrorCallback(hcan);
  }
}

/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group5 Callback functions
 *  @brief   CAN Callback functions
 *
@verbatim
  ==============================================================================
                          ##### Callback functions #####
  ==============================================================================
    [..]
    This subsection provides the following callback functions:
      (+) HAL_CAN_TxMailbox0CompleteCallback
      (+) HAL_CAN_TxMailbox1CompleteCallback
      (+) HAL_CAN_TxMailbox2CompleteCallback
      (+) HAL_CAN_TxMailbox0AbortCallback
      (+) HAL_CAN_TxMailbox1AbortCallback
      (+) HAL_CAN_TxMailbox2AbortCallback
      (+) HAL_CAN_RxFifo0MsgPendingCallback
      (+) HAL_CAN_RxFifo0FullCallback
      (+) HAL_CAN_RxFifo1MsgPendingCallback
      (+) HAL_CAN_RxFifo1FullCallback
      (+) HAL_CAN_SleepCallback
      (+) HAL_CAN_WakeUpFromRxMsgCallback
      (+) HAL_CAN_ErrorCallback

@endverbatim
  * @{
  */

/**
  * @brief  Transmission Mailbox 0 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxMailbox0CompleteCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Transmission Mailbox 1 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxMailbox1CompleteCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Transmission Mailbox 2 complete callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxMailbox2CompleteCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Transmission Mailbox 0 Cancellation callback.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxMailbox0AbortCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxMailbox0AbortCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Transmission Mailbox 1 Cancellation callback.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxMailbox1AbortCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxMailbox1AbortCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Transmission Mailbox 2 Cancellation callback.
  * @param  hcan pointer to an CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_TxMailbox2AbortCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_TxMailbox2AbortCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Rx FIFO 0 message pending callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxFifo0MsgPendingCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Rx FIFO 0 full callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxFifo0FullCallback could be implemented in the user
            file
   */
}

/**
  * @brief  Rx FIFO 1 message pending callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxFifo1MsgPendingCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Rx FIFO 1 full callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_RxFifo1FullCallback could be implemented in the user
            file
   */
}

/**
  * @brief  Sleep callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_SleepCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_SleepCallback could be implemented in the user file
   */
}

/**
  * @brief  WakeUp from Rx message callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_WakeUpFromRxMsgCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_WakeUpFromRxMsgCallback could be implemented in the
            user file
   */
}

/**
  * @brief  Error CAN callback.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval None
  */
__weak void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hcan);

  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_CAN_ErrorCallback could be implemented in the user file
   */
}

/**
  * @}
  */

/** @defgroup CAN_Exported_Functions_Group6 Peripheral State and Error functions
 *  @brief   CAN Peripheral State functions
 *
@verbatim
  ==============================================================================
            ##### Peripheral State and Error functions #####
  ==============================================================================
    [..]
    This subsection provides functions allowing to :
      (+) HAL_CAN_GetState()  : Return the CAN state.
      (+) HAL_CAN_GetError()  : Return the CAN error codes if any.
      (+) HAL_CAN_ResetError(): Reset the CAN error codes if any.

@endverbatim
  * @{
  */

/**
  * @brief  Return the CAN state.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL state
  */
HAL_CAN_StateTypeDef HAL_CAN_GetState(CAN_HandleTypeDef *hcan)
{
  HAL_CAN_StateTypeDef state = hcan->State;

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Check sleep mode acknowledge flag */
    if ((hcan->Instance->MSR & CAN_MSR_SLAK) != RESET)
    {
      /* Sleep mode is active */
      state = HAL_CAN_STATE_SLEEP_ACTIVE;
    }
    /* Check sleep mode request flag */
    else if ((hcan->Instance->MCR & CAN_MCR_SLEEP) != RESET)
    {
      /* Sleep mode request is pending */
      state = HAL_CAN_STATE_SLEEP_PENDING;
    }
  }

  /* Return CAN state */
  return state;
}

/**
  * @brief  Return the CAN error code.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval CAN Error Code
  */
uint32_t HAL_CAN_GetError(CAN_HandleTypeDef *hcan)
{
  /* Return CAN error code */
  return hcan->ErrorCode;
}

/**
  * @brief  Reset the CAN error code.
  * @param  hcan pointer to a CAN_HandleTypeDef structure that contains
  *         the configuration information for the specified CAN.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_CAN_ResetError(CAN_HandleTypeDef *hcan)
{
  HAL_StatusTypeDef status = HAL_OK;

  if ((hcan->State == HAL_CAN_STATE_READY) ||
      (hcan->State == HAL_CAN_STATE_LISTENING))
  {
    /* Reset CAN error code */
    hcan->ErrorCode = 0U;
  }
  else
  {
    /* Update error code */
    hcan->ErrorCode |= HAL_CAN_ERROR_NOT_INITIALIZED;

    status = HAL_ERROR;
  }

  /* Return the status */
  return status;
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_CAN_MODULE_ENABLED */

/**
  * @}
  */

#endif /* CAN1 */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
>>>>>>> master
