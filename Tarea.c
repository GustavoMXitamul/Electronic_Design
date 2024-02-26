#include <stdint.h>

// Definiciones de registros y direcciones
#define PERIPHERAL_BASE_ADDRESS     0x40000000U
#define AHB_BASE_ADDRESS            (PERIPHERAL_BASE_ADDRESS + 0x00020000U)
#define RCC_BASE_ADDRESS            (AHB_BASE_ADDRESS + 0x00001000U)
#define RCC_IOPENR_ADDRESS          (RCC_BASE_ADDRESS + 0x0000002CU)
#define IOPORT_ADDRESS              (PERIPHERAL_BASE_ADDRESS + 0x10000000U)

#define GPIOA_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000000U)
#define GPIOA_MODER_REG             (GPIOA_BASE_ADDRESS + 0x00000000U)
#define GPIOA_ODR_REG               (GPIOA_BASE_ADDRESS + 0x00000014U)
#define GPIOA_IDR_REG               (GPIOA_BASE_ADDRESS + 0x00000010U)

#define GPIOC_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000800U)
#define GPIOC_MODER_REG             (GPIOC_BASE_ADDRESS + 0x00000000U)
#define GPIOC_ODR_REG               (GPIOC_BASE_ADDRESS + 0x00000014U)
#define GPIOC_IDR_REG               (GPIOC_BASE_ADDRESS + 0x00000010U)
#define GPIOC_PUPD_REG              (GPIOC_BASE_ADDRESS + 0x0000000CU)

void delay_ms(uint16_t n);

int main(void) {
    uint32_t *ptr_rcc_iopenr = (uint32_t *)RCC_IOPENR_ADDRESS;
    uint32_t *ptr_gpioa_moder = (uint32_t *)GPIOA_MODER_REG;
    uint32_t *ptr_gpioa_odr = (uint32_t *)GPIOA_ODR_REG;
    uint32_t *ptr_gpioa_idr = (uint32_t *)GPIOA_IDR_REG;


    *ptr_rcc_iopenr |= (1 << 0);  // GPIOA


     // Configurar PA9 como salida para el LED (pin D8)
     *ptr_gpioa_moder |= (1 << 18);  // Establecer el bit 18 para modo de salida
     *ptr_gpioa_moder &= ~(1 << 19); // Limpiar el bit 19 para modo de salida

     // Configurar PA8 como entrada para los botones
     *ptr_gpioa_moder &= ~(3 << 16);  // Limpiar bits 16 y 17 para PA8

     // Configurar el pin PA10 como entrada para el switch externo (pin D2)
     *ptr_gpioa_moder &= ~(3 << 20); // Limpiar bits 20 y 21 para modo de entrada en PA10

     uint8_t contador = 0;

     while (1) {
         // Verificar el estado del switch externo (pin D2)
         if (*ptr_gpioa_idr & (1 << 10)) { // Si el pin PA10 está en alto (switch externo abierto)
             // Parpadear el LED a 500ms
             *ptr_gpioa_odr ^= (1 << 9); // Invertira el estado del LED (pin D8)
             delay_ms(500);
         } else {
             // Parpadear el LED a 200ms
             *ptr_gpioa_odr ^= (1 << 9); // Invertir el estado del LED (pin D8)
              delay_ms(200);
         }

         // Verificar el estado del botón2 (pin PA8)
                 if (!(*ptr_gpioa_idr & (1 << 8))) { // Si el pin PA7 está en bajo (botón2 presionado)
                     // Incrementar la variable en 1
                     contador++;
                     // Esperar un corto tiempo para evitar rebotes
                     delay_ms(10);
                 }

     }
}




void delay_ms(uint16_t n) {
    uint16_t i;
    for (; n > 0; n--) {
        for (i = 0; i < 140; i++) {
            // Pequeño retraso para lograr una pausa de 1 ms aproximadamente
            asm("NOP");
        }
    }
}
