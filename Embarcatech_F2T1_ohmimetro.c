// -- Inclusão de bibliotecas
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// -- Definição de constantes
// GPIO
#define adc 28 // ADC GPIO 28

// Display I2C
#define display_i2c_port i2c1 // Define a porta I2C
#define display_i2c_sda 14 // Define o pino SDA na GPIO 14
#define display_i2c_scl 15 // Define o pino SCL na GPIO 15
#define display_i2c_endereco 0x3C // Define o endereço do I2C
ssd1306_t ssd; // Inicializa a estrutura do display

// -- Definição de variáveis
int R_Con = 9845; // Resistor conhecido de 10k ohm (calibrado com multímetro)
float R_med = 0.0; // Resistor medido
float R_com = 0.0; // Resistor comercial
float V_ref = 3.3; // Tensão de referência
int ADC_res = 4095; // Resolução do ADC
uint16_t value_adc; // Valor analógico medido no GPIO 28

// Strings para o display
char str_R_med[5]; // Guarda o valor da resistência medida
char str_R_com[5]; // Guarda o valor da resistência comercial
const char* cor1; // Guarda o valor da cor do primeiro dígito
const char* cor2; // Guarda o valor da cor do segundo dígito
const char* cor3; // Guarda o valor da cor do multiplicador

// Função que guarda o nome das cores por dígito
const char* nome_cor(int n) {
    switch(n) {
        case 0: return "Preto";
        case 1: return "Marrom";
        case 2: return "Vermelho";
        case 3: return "Laranja";
        case 4: return "Amarelo";
        case 5: return "Verde";
        case 6: return "Azul";
        case 7: return "Violeta";
        case 8: return "Cinza";
        case 9: return "Branco";
        default: return "Erro";
    }
}

// Função para calcular as faixas de cores
void faixa_cores(float R_com){
    if(R_com == 0){
        // Mensagem no display caso o resistor medido esteja fora do alcance de medição (510 < R_med < 100k)
        cor1 = "Fora";
        cor2 = "de";
        cor3 = "alcance";
    }else{
        int ordem_grandeza = log10(R_com); // Pega a ordem de grandeza do valor do resistor
        int digitos_faixas = R_com / pow(10, ordem_grandeza - 1); // Calculo que separa os dois primeiros dígitos
        int primeiro_digito = digitos_faixas / 10; // Pega apenas o primeiro dígito
        int segundo_digito = digitos_faixas % 10; // Pega apenas o segundo dígito
        int multiplicador = ordem_grandeza - 1; // Pega o multiplicador
    
        // Define as cores com base no valor medido
        cor1 = nome_cor(primeiro_digito);
        cor2 = nome_cor(segundo_digito);
        cor3 = nome_cor(multiplicador);
    }
}

// Função para identificar o valor comercial da resistência na série E24
float converte_E24(float R_med){

    // Lista com os valores comerciais da série E24
    const int serie_E24[] = {
        510, 560, 620, 680, 750, 820, 910, 1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000, 2200, 2400, 2700, 3000, 3300, 3600, 3900, 4300, 4700, 5100, 5600, 6200, 6800, 7500, 8200, 9100, 10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000, 22000, 24000, 27000, 30000, 33000, 36000, 39000, 43000, 47000, 51000, 56000, 62000, 68000, 75000, 82000, 91000, 100000
    };

    const int tamanho_serie_E24 = (sizeof(serie_E24) /sizeof(serie_E24[0])); // Guarda a quantidade de itens da lista 

    int indice = 0; // Guarda o valor do melhor indice da lista da série E24
    float menor_erro = fabs(R_med - serie_E24[0]); // Auxilia a função de identificação guardando o menor erro entre os digitos

    // Estrutura de repetição que compara o valor calculado com os da série E24
    for (int i = 1; i < tamanho_serie_E24; i++){
        float erro = fabs(R_med - serie_E24[i]);
        if(erro < menor_erro){
            menor_erro = erro;
            indice = i;
        }
    }

    return serie_E24[indice]; // Retorna o valor comercial mais próximo da resistência medida
}

int main()
{
    // -- Inicializações
    // Monitor serial
    stdio_init_all();

    // ADC
    adc_init();
    adc_gpio_init(adc); // Inicia o ADC para o GPIO 28

    // Display I2C
    i2c_init(display_i2c_port, 400 * 1000); // Inicializa o I2C usando 400kHz
    gpio_set_function(display_i2c_sda, GPIO_FUNC_I2C); // Define o pino SDA (GPIO 14) na função I2C
    gpio_set_function(display_i2c_scl, GPIO_FUNC_I2C); // Define o pino SCL (GPIO 15) na função I2C
    gpio_pull_up(display_i2c_sda); // Ativa o resistor de pull up para o pino SDA (GPIO 14)
    gpio_pull_up(display_i2c_scl); // Ativa o resistor de pull up para o pino SCL (GPIO 15)
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, display_i2c_endereco, display_i2c_port); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display
    ssd1306_fill(&ssd, false); // Limpa o display
    ssd1306_send_data(&ssd); // Atualiza o display
    
    // Loop principal
    while (true) {
        adc_select_input(2); // Seleciona o ADC2 referente ao GPIO 28

        // Função para cálculo da média do ADC medido
        float soma = 0.0f; // Variável auxiliar para cálculo da média do ADC medido
        for(int i = 0; i < 500; i++){
            soma += adc_read();
            sleep_ms(1);
        }
        float value_adc = soma / 500.0f; // Média dos valores ADC medidos

        R_med = ((R_Con * value_adc) / (ADC_res - value_adc)); // Cálculo do valor da resistência

        R_com = converte_E24(R_med); // Encontra o valor comercial mais próximo do R_med
    
        // Função if para identificar se o R_med está dentro da escala de alcance (510 < R_med < 100k)
        if(R_com > 510 && R_com < 100000){
            faixa_cores(R_com);
        }else{
            faixa_cores(0);
        }

        // Cria as bordas do display
        ssd1306_rect(&ssd, 0, 0, 127, 63, true, false); // Borda principal
        ssd1306_line(&ssd, 1, 10, 126, 10, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 1, 20, 126, 20, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 1, 30, 126, 30, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 1, 40, 126, 40, true); // Desenha uma linha horizontal
        ssd1306_line(&ssd, 1, 50, 126, 50, true); // Desenha uma linha horizontal

        // Limpa a parte variável do display
        ssd1306_draw_string(&ssd, "        ", 58, 22); // Desenha uma string
        ssd1306_draw_string(&ssd, "        ", 58, 32); // Desenha uma string
        ssd1306_draw_string(&ssd, "        ", 58, 42); // Desenha uma string
        ssd1306_draw_string(&ssd, "        ", 50, 53); // Desenha uma string

        // Parte escrita do display
        ssd1306_draw_string(&ssd, "EMB Ohmimetro", 12, 2); // Desenha uma string
        ssd1306_draw_string(&ssd, "1a Cor", 2, 12); // Desenha uma string
        ssd1306_draw_string(&ssd, cor1, 58, 12); // Desenha uma string
        ssd1306_draw_string(&ssd, "2a Cor", 2, 22); // Desenha uma string
        ssd1306_draw_string(&ssd, cor2, 58, 22); // Desenha uma string
        ssd1306_draw_string(&ssd, "3a Cor", 2, 32); // Desenha uma string
        ssd1306_draw_string(&ssd, cor3, 58, 32); // Desenha uma string
        ssd1306_draw_string(&ssd, "Resis", 2, 42); // Desenha uma string
        sprintf(str_R_med, "%.0f", R_med); // Converte o float em string
        ssd1306_draw_string(&ssd, str_R_med, 50, 42); // Desenha uma string
        ssd1306_draw_string(&ssd, "Comer", 2, 53); // Desenha uma string
        sprintf(str_R_com, "%.0f", R_com); // Converte o float em string
        ssd1306_draw_string(&ssd, str_R_com, 50, 53); // Desenha uma string

        ssd1306_send_data(&ssd); // Atualiza o display

        sleep_ms(500);
    }
}