#include <stdint.h>

// Definiciones de registros y direcciones
#define PERIPHERAL_BASE_ADDRESS     0x40000000U
#define AHB_BASE_ADDRESS            (PERIPHERAL_BASE_ADDRESS + 0x00020000U)
#define RCC_BASE_ADDRESS            (AHB_BASE_ADDRESS + 0x00001000U)
#define RCC_IOPENR_ADDRESS          (RCC_BASE_ADDRESS + 0x0000002CU)
#define IOPORT_ADDRESS              (PERIPHERAL_BASE_ADDRESS + 0x10000000U)
#define GPIOA_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000000U)
#define GPIOB_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000400U)
#define GPIOC_BASE_ADDRESS          (IOPORT_ADDRESS + 0x00000800U)

#define GPIOA ((GPIO_RegDef_t*)GPIOA_BASE_ADDRESS)
#define GPIOB ((GPIO_RegDef_t*)GPIOB_BASE_ADDRESS)
#define GPIOC ((GPIO_RegDef_t*)GPIOC_BASE_ADDRESS)
#define RCC ((RCC_RegDef_t*)RCC_BASE_ADDRESS)

uint8_t myfsm = 0x00;
uint8_t inc_second = 0x00;
uint8_t formato_24_horas = 1; // Inicialmente en formato de 24 horas

typedef struct
{
    uint32_t MODER;
    uint32_t OTYPER;
    uint32_t OSPEEDR;
    uint32_t PUPDR;
    uint32_t IDR;
    uint32_t ODR;
    uint32_t BSRR;
    uint32_t LCKR;
    uint32_t AFR[2];
    uint32_t BRR;
} GPIO_RegDef_t;

typedef struct
{
    uint32_t CR;
    uint32_t ICSCR;
    uint32_t CRRCR;
    uint32_t CFGR;
    uint32_t CIER;
    uint32_t CIFR;
    uint32_t CICR;
    uint32_t IOPRSTR;
    uint32_t AHBRSTR;
    uint32_t APB2RSTR;
    uint32_t APB1RSTR;
    uint32_t IOPENR;
} RCC_RegDef_t;

struct Time_t {
    uint8_t hour_decimal;
    uint8_t hour_unit;
    uint8_t minute_decimal;
    uint8_t minute_unit;
    uint8_t second_decimal;
    uint8_t second_unit;
};

struct Time_t watch = {0}; // Inicializar todas las variables a 0

// Macros para los dígitos del display
#define cero  0x3F
#define uno   0x06
#define dos   0x5B
#define tres  0x4F
#define cuatro  0x66
#define cinco   0x6D
#define seis   0x7D
#define siete  0x07
#define ocho   0x7F
#define nueve  0x67

void delay_ms(uint16_t n);
uint8_t decoder(uint8_t value_to_decode);
void formato_hora();

// Tabla de búsqueda para la función decoder
const uint8_t decoder_table[] = {cero, uno, dos, tres, cuatro, cinco, seis, siete, ocho, nueve};

void set_time(uint8_t hour_decimal, uint8_t hour_unit, uint8_t minute_decimal, uint8_t minute_unit, uint8_t second_decimal, uint8_t second_unit) {
    watch.hour_decimal = hour_decimal;
    watch.hour_unit = hour_unit;
    watch.minute_decimal = minute_decimal;
    watch.minute_unit = minute_unit;
    watch.second_decimal = second_decimal;
    watch.second_unit = second_unit;
}

int main(void)
{
    // Habilitar el reloj para GPIOC y GPIOB
    RCC->IOPENR |= (1 << 2) | (1 << 1) | (1 << 0);

    // Definir máscaras para los modos de los pines del puerto C como salida
    uint32_t portC_masks = (0b01 << 8) |(0b01 << 10) | (0b01 << 12) | (0b01 << 14) | (0b01 << 16) | (0b01 << 18);

    // Limpiar y configurar los modos de los pines del puerto C como salida
    GPIOC->MODER &= ~(0b11 << 8 | 0b11 << 10 | 0b11 << 12 | 0b11 << 14 | 0b11 << 16 | 0b11 << 18);
    GPIOC->MODER |= portC_masks;

    // Definir máscaras para los modos de los pines del puerto B como salida
    uint32_t portB_masks = (0b01 << 0) | (0b01 << 2) | (0b01 << 4) | (0b01 << 6) |
                           (0b01 << 8) | (0b01 << 10) | (0b01 << 12) | (0b01 << 14);

    // Limpiar y configurar los modos de los pines del puerto B como salida
    GPIOB->MODER &= ~(0xFFFF); // Limpiar todos los bits
    GPIOB->MODER |= portB_masks;

    // Definir máscaras para los modos de los pines del puerto C como entrada
    uint32_t portC_masks_input = (0b00 << 20); // PC0 como entrada

    // Limpiar y configurar los modos de los pines del puerto C como entrada
    GPIOA->MODER &= ~(0b11 << 20); // Limpiar los bits correspondientes
    GPIOA->MODER |= portC_masks_input;

    set_time(1, 2, 5, 9, 3, 3); // 1:05:45 PM

    while (1)
    {
        // Verificar si el botón está presionado (suponiendo que el botón está conectado al pin PC0)
        if ((GPIOA->IDR & (1 << 10)) == 0)
        {
            // Guardar el tiempo actual antes de convertir al formato de 12 horas
            uint8_t temp_hour = watch.hour_decimal * 10 + watch.hour_unit;
            if (formato_24_horas == 1)
            {
                if (temp_hour >= 13)
                {
                    temp_hour -= 12; // Convertir a formato de 12 horas
                }
                else if (temp_hour == 0)
                {
                    temp_hour = 12; // 12 AM en lugar de 0 AM
                }
            }
            else
            {
                if (temp_hour == 12)
                {
                    temp_hour = 0; // 12 PM en lugar de 12 AM
                }
                else if (temp_hour < 12)
                {
                    temp_hour += 12; // Convertir a formato de 24 horas
                }
            }

            // Mostrar el tiempo en formato de 12 horas
            set_time(temp_hour / 10, temp_hour % 10, watch.minute_decimal, watch.minute_unit, watch.second_decimal, watch.second_unit);

            // Cambiar el formato de hora para la próxima vez que se presione el botón
            formato_24_horas = !formato_24_horas;

            // Retraso para evitar falsos contactos (debouncing)
            delay_ms(100); // Ajusta este valor según sea necesario
        }

        // Llamar a la función para mostrar la hora en el display
        formato_hora();
    }

}

void formato_hora()
{
    // Limpiar los puertos antes de encender los segmentos correspondientes y los dígitos
    GPIOB->BSRR |= 0xFFFF0000; // Limpiar todos los bits del puerto B
    GPIOC->BSRR |= 0xFFFF0000; // Limpiar todos los bits del puerto C

    switch (myfsm)
    {
    case 0:
        // Encender segmento correspondiente al dígito 7 en formato 24h
        GPIOC->BSRR |= 0X01 << 5;
        // Encender el dígito 7 en formato 24h
        GPIOB->BSRR |= decoder(watch.second_unit);
        myfsm = 1; // Cambiar al siguiente estado
        break;

    case 1:
        // Encender segmento correspondiente al dígito 3 en formato 24h
        GPIOC->BSRR |= 0X01 << 6;
        // Encender el dígito 3 en formato 24h
        GPIOB->BSRR |= decoder(watch.second_decimal);
        myfsm++; // Avanzar al siguiente estado
        break;

    case 2:
        // Encender segmento correspondiente al dígito X en formato 24h
        GPIOC->BSRR |= 0X01 << 8;
        // Encender el dígito X en formato 24h
        GPIOB->BSRR |= decoder(watch.minute_unit);
        myfsm++; // Avanzar al siguiente estado
        break;

    case 3:
        // Encender segmento correspondiente al dígito X en formato 24h
        GPIOC->BSRR |= 0X01 << 9;
        // Encender el dígito X en formato 24h
        GPIOB->BSRR |= decoder(watch.minute_decimal);
        myfsm++; // Avanzar al siguiente estado
        break;

    case 4:
        // Encender segmento correspondiente al dígito X en formato 24h
        GPIOC->BSRR |= 0X01 << 7;
        // Encender el dígito X en formato 24h
        GPIOB->BSRR |= decoder(watch.hour_unit % 10);
        myfsm++; // Avanzar al siguiente estado
        break;

    case 5:
        // Encender segmento correspondiente al dígito X en formato 24h
        GPIOC->BSRR |= 0X01 << 4;
        // Encender el dígito X en formato 24h
        GPIOB->BSRR |= decoder(watch.hour_decimal);
        myfsm = 0; // Volver al estado inicial
        break;

    default:
        myfsm = 0; // En caso de cualquier otro estado, volver al estado inicial
        break;
    }

    delay_ms(1);
    inc_second++;

    if (inc_second == 100) {
        inc_second = 0; // Reiniciar el contador de milisegundos
        watch.second_unit++; // Incrementar las unidades de segundo

        if (watch.second_unit == 10) {
            watch.second_unit = 0; // Reiniciar las unidades de segundo a cero
            watch.second_decimal++; // Incrementar las decenas de segundo

            if (watch.second_decimal == 6) {
                watch.second_decimal = 0; // Reiniciar las decenas de segundo a cero
                watch.minute_unit++; // Incrementar las unidades de minuto

                if (watch.minute_unit == 10) {
                    watch.minute_unit = 0; // Reiniciar las unidades de minuto a cero
                    watch.minute_decimal++; // Incrementar las decenas de minuto

                    if (watch.minute_decimal == 6) {
                        watch.minute_decimal = 0; // Reiniciar las decenas de minuto a cero
                        watch.hour_unit++; // Incrementar las unidades de hora

                        if (formato_24_horas == 1) {
                            // Si es 12:59:00, ajustar a 01:00:00 (AM o PM)
                            if (watch.hour_decimal == 1 && watch.hour_unit == 2 && watch.minute_unit == 5 && watch.minute_decimal == 9) {
                                if (watch.hour_unit == 2) {
                                    // Si es 12:59:00 PM, cambiar a 01:00:00 AM
                                    if (watch.hour_decimal == 2) {
                                        watch.hour_decimal = 1; // AM
                                    } else {
                                        // Si es 12:59:00 AM, cambiar a 01:00:00 PM
                                        watch.hour_decimal = 2; // PM
                                    }
                                }
                                watch.hour_unit = 0; // 1
                                watch.minute_unit = 0; // 0
                                watch.minute_decimal = 0; // 0
                            }
                            // Si las horas llegan a 13, cambiar de PM a AM y reiniciar a 1:00
                            else if (watch.hour_unit == 3) {
                                watch.hour_decimal = 0; // AM
                                watch.hour_unit = 1; // 1
                            }
                            // Si las horas llegan a 12, cambiar de AM a PM
                            else if (watch.hour_unit == 2 && watch.hour_decimal == 1) {
                                watch.hour_decimal = 2; // PM
                            }
                            // Si las horas llegan a 12 (mediodía), cambiar de AM a PM
                            else if (watch.hour_unit == 2 && watch.hour_decimal == 0) {
                                watch.hour_decimal = 2; // PM
                            }
                        }
                    }
                }
            }
        }
    }
}

void delay_ms(uint16_t n)
{
    uint16_t i;
    for (; n > 0; n--)
    {
        for (i = 0; i < 140; i++)
        {
            // Pequeño retraso para lograr una pausa de 1 ms aproximadamente
            asm("NOP");
        }
    }
}

uint8_t decoder(uint8_t value_to_decode)
{
    if (value_to_decode < 10)
    {
        return decoder_table[value_to_decode];
    }
    else
    {
        // Manejar el caso de un valor fuera de rango
        return 0; // O cualquier otro valor de retorno que desees para indicar un error
    }
}
