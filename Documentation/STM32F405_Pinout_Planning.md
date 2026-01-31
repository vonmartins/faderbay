# STM32F405 - Planificación de Pines y Periféricos
## Proyecto: FaderBay - Controlador MIDI de 16 canales

---

## 1. Resumen de Periféricos Necesarios

| Periférico | Función | Configuración | Prioridad |
|------------|---------|---------------|-----------|
| ADC1 | Lectura de faders (vía MUX) | 12-bit, Single Channel | Alta |
| TIM2 | Encoder rotativo (modo Encoder) | Encoder Mode TI1 y TI2 | Alta |
| USART1 | MIDI OUT (31250 baud) | TX only, 31250 baud, 8N1 | Alta |
| USART2 | Debug UART | TX/RX, 115200 baud | Media |
| SPI1 | Pantalla OLED | Master, 8-bit, hasta 10 MHz | Alta |
| USB OTG FS | USB MIDI | Device Only, Full Speed | Alta |
| SWD | Programación/Debug | PA13 (SWDIO), PA14 (SWCLK) | Crítica |
| GPIOs | Botones (4), MUX control (4), LEDs (3), OLED control (2) | Input/Output | Alta |
| RCC | Oscilador externo | HSE 8 MHz (para USB) | Crítica |

---

## 2. Configuración del ADC

### ADC1 - Lectura de Faders

| Parámetro | Valor |
|-----------|-------|
| Modo | Single Conversion |
| Resolución | 12 bits |
| Canal | IN0 (PA0) recomendado |
| Frecuencia de muestreo | ~1 kHz por fader (16 ms ciclo completo) |
| Tiempo de conversión | 15 ciclos + 3 ciclos = 18 ciclos |
| Trigger | Software |
| DMA | Opcional (recomendado) |
| Alineación de datos | Right Aligned |

**Pin sugerido:** PA0 (ADC1_IN0)

---

## 3. Configuración del Multiplexor (GPIO)

### Control del MUX (16:1)
Se necesitan 4 pines GPIO para controlar un MUX 16:1 (CD74HC4067 o similar)

| Señal | Pin Sugerido | Función | Modo |
|-------|--------------|---------|------|
| MUX_S0 | PC0 | Select bit 0 | GPIO Output |
| MUX_S1 | PC1 | Select bit 1 | GPIO Output |
| MUX_S2 | PC2 | Select bit 2 | GPIO Output |
| MUX_S3 | PC3 | Select bit 3 | GPIO Output |
| MUX_EN | PC4 | Enable (opcional) | GPIO Output |

**Configuración:** Push-Pull, No Pull, Low Speed

---

## 4. Configuración de Botones (GPIO Input)

| Botón | Pin Sugerido | Configuración |
|-------|--------------|---------------|
| BTN1 | PB0 | GPIO Input, Pull-Up interno |
| BTN2 | PB1 | GPIO Input, Pull-Up interno |
| BTN3 | PB10 | GPIO Input, Pull-Up interno |
| BTN4 | PB11 | GPIO Input, Pull-Up interno |

**Configuración:**
- Modo: Input
- Pull-up/Pull-down: Pull-Up
- Velocidad: Low
- Interrupción: EXTI opcional (para detección de eventos)

---

## 5. Configuración del Encoder Rotativo (Timer)

### TIM2 - Modo Encoder

| Parámetro | Valor |
|-----------|-------|
| Canal A | TIM2_CH1 (PA15 o PA5) |
| Canal B | TIM2_CH2 (PB3) |
| Modo | Encoder Mode TI1 and TI2 |
| Contador | 16-bit |
| Prescaler | 0 (sin prescaler) |
| Período (ARR) | 65535 o menor según resolución deseada |
| Polaridad | Both Edges |

**Pines sugeridos:**
- PA15: TIM2_CH1 (Encoder A)
- PB3: TIM2_CH2 (Encoder B)

---

## 6. Configuración UART1 - MIDI OUT

### USART1 - Transmisión MIDI

| Parámetro | Valor |
|-----------|-------|
| Baud Rate | 31250 |
| Word Length | 8 bits |
| Parity | None |
| Stop Bits | 1 |
| Modo | TX Only (Asynchronous) |
| Hardware Flow Control | None |
| Pin TX | PA9 (USART1_TX) |
| Pin RX | No usado (o PA10 para futuro) |

**Pin sugerido:** PA9 (USART1_TX)

---

## 7. Configuración UART2 - Debug

### USART2 - Debug Serial

| Parámetro | Valor |
|-----------|-------|
| Baud Rate | 115200 |
| Word Length | 8 bits |
| Parity | None |
| Stop Bits | 1 |
| Modo | TX + RX (Asynchronous) |
| Hardware Flow Control | None |
| Pin TX | PA2 (USART2_TX) |
| Pin RX | PA3 (USART2_RX) |

**Pines sugeridos:** PA2 (TX), PA3 (RX)

---

## 8. Configuración SPI1 - Pantalla OLED

### SPI1 - Master Mode

| Parámetro | Valor |
|-----------|-------|
| Modo | Full-Duplex Master |
| Frame Format | Motorola |
| Data Size | 8 bits |
| First Bit | MSB First |
| Baud Rate Prescaler | 8 (hasta 10.5 MHz con 84 MHz APB2) |
| Clock Polarity (CPOL) | Low (0) |
| Clock Phase (CPHA) | 1 Edge |
| NSS | Software (usar GPIO para CS) |

| Señal | Pin Sugerido | Función |
|-------|--------------|---------|
| SCK | PA5 | SPI1_SCK |
| MOSI | PA7 | SPI1_MOSI |
| MISO | PA6 | No usado (solo escritura) |
| CS | PB6 | GPIO Output (Chip Select) |
| DC | PB7 | GPIO Output (Data/Command) |
| RST | PB8 | GPIO Output (Reset) |

---

## 9. Configuración USB OTG FS - USB MIDI

### USB_OTG_FS - Device Mode

| Parámetro | Valor |
|-----------|-------|
| Modo | Device Only |
| Velocidad | Full Speed (12 Mbps) |
| VBUS Sensing | Enable |
| SOF | Enable |
| Pin D- | PA11 (USB_OTG_FS_DM) |
| Pin D+ | PA12 (USB_OTG_FS_DP) |
| ID | No usado |

**Pines fijos:**
- PA11: USB_OTG_FS_DM
- PA12: USB_OTG_FS_DP

**Nota:** Requiere oscilador externo HSE de 8 MHz para precisión de clock USB

---

## 10. Configuración de LEDs Indicadores (GPIO Output)

| LED | Pin Sugerido | Función | Modo |
|-----|--------------|---------|------|
| LED_3V3 (Verde) | PC13 | Indicador 3.3V ON | GPIO Output |
| LED_USB (Azul) | PC14 | Indicador USB Activity | GPIO Output |
| LED_PWR (Rojo) | PC15 | Indicador Power ON | GPIO Output |

**Configuración:**
- Modo: Output Push-Pull
- Pull: No Pull
- Velocidad: Low
- Nivel inicial: Low (LED apagado si es active-high)

---

## 11. Configuración de SWD (Debug)

### Serial Wire Debug

| Señal | Pin | Función |
|-------|-----|---------|
| SWDIO | PA13 | Sys_JTMS-SWDIO |
| SWCLK | PA14 | Sys_JTCK-SWCLK |

**Importante:** Estos pines son críticos. NO usar para otras funciones.

---

## 12. Configuración del Clock (RCC)

### Configuración de Relojes

| Parámetro | Valor | Notas |
|-----------|-------|-------|
| HSE (Oscilador Externo) | 8 MHz | Cristal externo obligatorio para USB |
| PLL Source | HSE | |
| PLLM | 8 | HSE / PLLM = 1 MHz |
| PLLN | 336 | 1 MHz × 336 = 336 MHz |
| PLLP | 2 | SYSCLK = 336/2 = 168 MHz |
| PLLQ | 7 | USB = 336/7 = 48 MHz (requerido) |
| SYSCLK | 168 MHz | Frecuencia máxima STM32F405 |
| AHB Prescaler | 1 | AHB = 168 MHz |
| APB1 Prescaler | 4 | APB1 = 42 MHz (máx 42 MHz) |
| APB2 Prescaler | 2 | APB2 = 84 MHz (máx 84 MHz) |
| USB Clock | 48 MHz | Debe ser exactamente 48 MHz |

**Árbol de Clock:**
```
HSE (8 MHz) → PLL → SYSCLK (168 MHz)
                  ↓
                USB (48 MHz)
                  ↓
            AHB (168 MHz)
            ↓          ↓
       APB1 (42 MHz)  APB2 (84 MHz)
       ↓                ↓
    USART1,2         SPI1, ADC1
```

---

## 13. Tabla de Asignación de Pines Completa

| Pin | Función Principal | Periférico | Señal | Notas |
|-----|-------------------|------------|-------|-------|
| PA0 | ADC Input | ADC1 | ADC1_IN0 | Entrada de MUX |
| PA2 | Debug TX | USART2 | USART2_TX | Debug serial |
| PA3 | Debug RX | USART2 | USART2_RX | Debug serial |
| PA5 | OLED Clock | SPI1 | SPI1_SCK | Display OLED |
| PA6 | (No usado) | - | - | SPI1_MISO opcional |
| PA7 | OLED Data | SPI1 | SPI1_MOSI | Display OLED |
| PA9 | MIDI TX | USART1 | USART1_TX | MIDI OUT 31250 baud |
| PA10 | (Reservado) | - | - | USART1_RX futuro |
| PA11 | USB D- | USB_OTG_FS | USB_DM | USB MIDI |
| PA12 | USB D+ | USB_OTG_FS | USB_DP | USB MIDI |
| PA13 | SWD Data | SWD | SWDIO | **NO CAMBIAR** |
| PA14 | SWD Clock | SWD | SWCLK | **NO CAMBIAR** |
| PA15 | Encoder A | TIM2 | TIM2_CH1 | Encoder rotativo |
| PB0 | Botón 1 | GPIO | Input | Pull-up interno |
| PB1 | Botón 2 | GPIO | Input | Pull-up interno |
| PB3 | Encoder B | TIM2 | TIM2_CH2 | Encoder rotativo |
| PB6 | OLED CS | GPIO | Output | Chip Select |
| PB7 | OLED DC | GPIO | Output | Data/Command |
| PB8 | OLED Reset | GPIO | Output | Reset display |
| PB10 | Botón 3 | GPIO | Input | Pull-up interno |
| PB11 | Botón 4 | GPIO | Input | Pull-up interno |
| PC0 | MUX S0 | GPIO | Output | Control multiplexor |
| PC1 | MUX S1 | GPIO | Output | Control multiplexor |
| PC2 | MUX S2 | GPIO | Output | Control multiplexor |
| PC3 | MUX S3 | GPIO | Output | Control multiplexor |
| PC4 | MUX EN | GPIO | Output | Enable MUX (opcional) |
| PC13 | LED Verde | GPIO | Output | Indicador 3.3V |
| PC14 | LED Azul | GPIO | Output | Indicador USB |
| PC15 | LED Rojo | GPIO | Output | Indicador Power |

---

## 14. Recursos Utilizados - Resumen

| Recurso | Cantidad Usada | Total Disponible | % Uso |
|---------|----------------|------------------|-------|
| GPIO Pins | ~25 | 51 (LQFP64) | ~49% |
| USART | 2 | 6 | 33% |
| SPI | 1 | 3 | 33% |
| ADC | 1 canal | 16 canales | 6% |
| Timer | 1 (TIM2) | 14 timers | 7% |
| USB OTG | 1 | 1 | 100% |
| DMA Streams | 0-2 (opcional) | 16 | 0-12% |

---

## 15. Configuraciones Adicionales en CubeMX

### NVIC (Interrupciones)

| Interrupción | Prioridad | Uso |
|--------------|-----------|-----|
| USB_OTG_FS | 0 (alta) | USB MIDI |
| USART1 | 1 | MIDI TX complete (opcional) |
| TIMx | 2 | Timer para escaneo ADC |
| EXTI (Botones) | 3 | Detección de pulsación |

### DMA (Opcional pero Recomendado)

| DMA Stream | Periférico | Dirección | Prioridad |
|------------|------------|-----------|-----------|
| DMA2_Stream0 | ADC1 | Periférico→Memoria | Alta |
| DMA2_Stream7 | USART1_TX | Memoria→Periférico | Media |

### FreeRTOS (Opcional)

Si se usa RTOS:
- Heap Size: Mínimo 8 KB
- Stack Size: 512 bytes por tarea
- Tareas sugeridas:
  - Task ADC/Faders (prioridad alta)
  - Task MIDI TX (prioridad alta)
  - Task UI/Display (prioridad media)
  - Task Botones/Encoder (prioridad media)

---

## 16. Notas de Implementación

### Escaneo de Faders
1. Configurar MUX a canal 0 (S3S2S1S0 = 0000)
2. Esperar tiempo de settling (~10 µs)
3. Iniciar conversión ADC
4. Leer valor ADC
5. Incrementar canal MUX
6. Repetir 16 veces
7. Ciclo completo: ~16-20 ms (suficiente para faders)

### MIDI Timing
- MIDI byte: 320 µs @ 31250 baud
- Mensaje Note On: ~960 µs (3 bytes)
- Usar buffer circular con DMA o interrupciones

### USB MIDI
- Device Class: Audio (0x01)
- Subclass: MIDI Streaming (0x03)
- Requiere descriptores USB-MIDI específicos
- Usar middleware USB Device de ST

### Display OLED
- Controlador típico: SSD1306
- Resolución: 128x64 (verificar tu modelo 256x64)
- Protocolo: SPI de 4 hilos
- Framebuffer: 128x64/8 = 1024 bytes (o 2048 para 256x64)

---

## 17. Checklist de Configuración CubeMX

- [ ] RCC: Activar HSE (8 MHz Crystal/Ceramic Resonator)
- [ ] RCC: Configurar PLL para 168 MHz SYSCLK y 48 MHz USB
- [ ] SYS: SWD activado (PA13, PA14)
- [ ] USART1: Asynchronous, 31250 baud, TX only
- [ ] USART2: Asynchronous, 115200 baud, TX+RX
- [ ] SPI1: Full-Duplex Master, 8-bit, Prescaler 8
- [ ] USB_OTG_FS: Device Only, activar
- [ ] ADC1: Single Conversion, 12-bit, Canal IN0
- [ ] TIM2: Encoder Mode, ambos canales
- [ ] GPIO: Configurar según tabla de pines
- [ ] NVIC: Activar interrupciones necesarias
- [ ] DMA: Configurar para ADC y USART (opcional)
- [ ] Clock Configuration: Verificar todos los clocks
- [ ] Middleware: Activar USB Device con clase Audio/MIDI
- [ ] Project Manager: Toolchain IDE, nombre proyecto, ubicación

---

## 18. Archivos a Generar

Después de configurar CubeMX:
- `faderbay.ioc` - Archivo de configuración
- `Core/Inc/main.h` - Definiciones principales
- `Core/Src/main.c` - Código de inicialización
- `USB_DEVICE/` - Middleware USB
- `Drivers/` - HAL y CMSIS

---

**Fecha de creación:** 20 de enero de 2026  
**MCU:** STM32F405RGT6 (LQFP64)  
**Proyecto:** FaderBay MIDI Controller  
**Versión:** 1.0
