#include "main.h"

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if (heth->Instance != ETH) {
    return;
  }

  __HAL_RCC_ETH_CLK_ENABLE();
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  MODIFY_REG(SYSCFG->PMC, SYSCFG_PMC_MII_RMII_SEL, ETH_MEDIA_INTERFACE_RMII);

  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF11_ETH;

  GPIO_InitStruct.Pin = RMII_REF_CLK_Pin | RMII_MDIO_Pin | RMII_CRS_DV_Pin;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RMII_MDC_Pin | RMII_RXD0_Pin | RMII_RXD1_Pin;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RMII_TX_EN_Pin | RMII_TXD0_Pin | RMII_TXD1_Pin;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(ETH_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(ETH_IRQn);
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
  if (heth->Instance != ETH) {
    return;
  }

  __HAL_RCC_ETH_CLK_DISABLE();

  HAL_GPIO_DeInit(GPIOA, RMII_REF_CLK_Pin | RMII_MDIO_Pin | RMII_CRS_DV_Pin);
  HAL_GPIO_DeInit(GPIOC, RMII_MDC_Pin | RMII_RXD0_Pin | RMII_RXD1_Pin);
  HAL_GPIO_DeInit(GPIOG, RMII_TX_EN_Pin | RMII_TXD0_Pin | RMII_TXD1_Pin);

  HAL_NVIC_DisableIRQ(ETH_IRQn);
}
