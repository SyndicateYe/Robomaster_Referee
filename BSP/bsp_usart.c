#include "bsp_usart.h"

void usart_communicate_config(usart_param_struct usart_param)
{
	LL_DMA_EnableDoubleBufferMode(usart_param.rx_DMAx,usart_param.rx_Stream);
	LL_DMA_SetMemoryAddress(usart_param.rx_DMAx,usart_param.rx_Stream,usart_param.rx_usart_dma_memory_base_address_0);
	LL_DMA_SetPeriphAddress(usart_param.rx_DMAx,usart_param.rx_Stream,(u32)&usart_param.USARTx->DR);
	LL_DMA_SetDataLength(usart_param.rx_DMAx,usart_param.rx_Stream,usart_param.rx_usart_dma_buffer_size);
	LL_DMA_SetMemory1Address(usart_param.rx_DMAx,usart_param.rx_Stream,usart_param.rx_usart_dma_memory_base_address_1);
	
	LL_DMA_SetPeriphAddress(usart_param.tx_DMAx,usart_param.tx_Stream,(u32)&usart_param.USARTx->DR);
	
	LL_USART_EnableIT_IDLE(usart_param.USARTx);
        LL_USART_EnableDMAReq_RX(usart_param.USARTx);
	LL_USART_EnableDMAReq_TX(usart_param.USARTx);
	
	LL_DMA_EnableIT_TC(usart_param.rx_DMAx, usart_param.rx_Stream);
	LL_DMA_EnableIT_TC(usart_param.tx_DMAx, usart_param.tx_Stream);

	LL_DMA_EnableStream(usart_param.rx_DMAx, usart_param.rx_Stream);
	
}

void usart_inturrupt_processed(usart_param_struct usart_param)
{
		if(((usart_param.USARTx->SR)&(1<<4))!=0)
		{
			__IO uint32_t tmpreg = 0x00U;
			tmpreg = usart_param.USARTx->SR;
			tmpreg = usart_param.USARTx->DR;
			UNUSED(tmpreg);
			osSignalSet(*(usart_param.task), UART_IDLE_SIGNAL);
		}
}



void dma_tx_inturrupt(usart_param_struct usart_param)
{
	
}



void usart_dma_send(usart_param_struct *usart_param,u32 dma_addr,u32 dma_buffer_size)
{
		
		if(usart_param->tx_finish_flag)
		{
			LL_DMA_SetMemoryAddress(usart_param->tx_DMAx,usart_param->tx_Stream,dma_addr);
	
			LL_DMA_SetDataLength(usart_param->tx_DMAx,usart_param->tx_Stream,dma_buffer_size);
		
			LL_DMA_EnableStream(usart_param->tx_DMAx, usart_param->tx_Stream); //¿ªÆô´«Êä
			
			usart_param->tx_finish_flag =0;
		}
}


uint8_t dma_current_memory_target(DMA_Stream_TypeDef *dma_stream)
{
  uint8_t tmp = 0;

  /* Get the current memory target */
  if ((dma_stream->CR & DMA_SxCR_CT) != 0)
  {
    /* Current memory buffer used is Memory 1 */
    tmp = 1;
  }
  else
  {
    /* Current memory buffer used is Memory 0 */
    tmp = 0;
  }
  return tmp;
}
uint16_t remain_data_counter_temp=0;
uint16_t dma_current_data_counter(DMA_Stream_TypeDef *dma_stream)
{
  /* Return the number of remaining data units for DMAy Streamx */
  return ((uint16_t)(dma_stream->NDTR));
}

void get_dma_memory_msg(DMA_Stream_TypeDef *dma_stream, uint8_t *mem_id, uint16_t *remain_cnt)
{
  *mem_id     = dma_current_memory_target(dma_stream);
  *remain_cnt =  dma_current_data_counter(dma_stream);
}

void DMAUsart5DataFinishedHandle(void)
{}

