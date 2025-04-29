<div align="center">
    <img src="https://moodle.embarcatech.cepedi.org.br/pluginfile.php/1/theme_moove/logo/1733422525/Group%20658.png" alt="Logo Embarcatech" height="100">
</div>

<br>

# Ohmímetro por ADC

## Sumário

- [Descrição](#descrição)
- [Funcionalidades Implementadas](#funcionalidades-implementadas)
- [Ferramentas utilizadas](#ferramentas-utilizadas)
- [Objetivos](#objetivos)
- [Instruções de uso](#instruções-de-uso)
- [Vídeo de apresentação](#vídeo-de-apresentação)
- [Aluno e desenvolvedor do projeto](#aluno-e-desenvolvedor-do-projeto)
- [Licensa](#licença)

## Descrição

Este projeto implementa um ohmímetro utilizando a placa de desenvolvimento BitDogLab e o microcontrolador RP2040, com capacidade de reconhecimento automático do código de cores de resistores. O sistema realiza a leitura da resistência de um resistor conectado, utilizando o conversor analógico-digital (ADC) para calcular o valor da resistência. Após calcular o valor medido, o projeto identifica o valor comercial mais próximo da série E24 (tolerância de 5%) e determina as três primeiras faixas de cores correspondentes ao resistor (1o dígito, 2o dígito e multiplicador). As informações são exibidas em um display OLED SSD1306, mostrando tanto o valor numérico da resistência quanto as cores das faixas de maneira textual.

## Funcionalidades Implementadas

1. Leitura do ADC:

   - O microcontrolador realiza a leitura da tensão no ponto médio do divisor de tensão através do ADC (GPIO 28). Esse valor é lido a cada 1ms e uma média é feita com os últimos 500 valores, isso facilita a definição do valor, diminuindo a interferência do ADC.

2. Cálculo da resistência:

   - O sistema calcula a resistência do resistor desconhecido através da fórmula de divisor de tensão. A fórmula final considerando os níveis de ADC (resolução de 12 bits = 0 a 4095): \
$Rmed = \frac{Rcon \cdot ADCmed}{ADCmáx - ADCmed}$ \
Onde:
       - $Rcon$ é o valor da resistência do resistor conhecido,
       - $ADCmed$ é o valor do ADC medido no ponto médio,
       - $ADCmáx$ é o valor máximo (resolução) do ADC = 4095.

3. Identificação do valor comercial:

   - Após calcular o valor da resistência medida é chamada uma função que define o valor de resistência comercial da série E24 mais próximo do valor calculado.
    
4. Determinação das faixas de cores:

   - A partir do valor comercial, uma função define as faixas de cores para o 1o dígito, 2o dígito e multiplicador. Uma função auxiliar guarda o nome das cores por dígito, de acordo com o padrão de cores definido para resistores.
![image](https://github.com/user-attachments/assets/9d444a6a-be26-4658-98d4-d511a0ff15c4)
  
5. Exibição no Display OLED:

   - O display OLED exibe em tempo real:
     - Título;
     - 3 faixas de cores,
     - Valor medido da resistência desconhecida,
     - Valor aproximado da série E24.

## Ferramentas utilizadas

- **Simulador de eletrônica wokwi**: Ambiente utilizado para simular o hardware e validar o funcionamento do sistema.
- **Ferramenta educacional BitDogLab (versão 6.3)**: Placa de desenvolvimento utilizada para programar o microcontrolador.
- **Microcontrolador Raspberry Pi Pico W**: Responsável por realizar a leitura do ADC, o cálculo da resistência e a comunicação com o display OLED.
- **Conversor analógico-digital(ADC)**: Utilizado para medir a tensão no ponto médio do divisor de tensão, permitindo o cálculo da resistência desconhecida.
- **Display OLED SSD1306**: Apresenta os dados em tempo real (Faixa de cores, Resistência medida, etc.).
- **Visual Studio Code (VS Code)**: IDE utilizada para o desenvolvimento do código com integração ao Pico SDK.
- **Pico SDK**: Kit de desenvolvimento de software utilizado para programar o Raspberry Pi Pico W em linguagem C.
- **Monitor serial do VS Code**: Ferramenta utilizada para realizar testes e depuração.

## Objetivos

1. Consolidar os conceitos estudados sobre desenvolvimento e programação de sistemas embarcados.
2. Fazer a medição da resistência de um resistor desconhecido a partir do ADC e do princípio do divisor de tensão.
3. Identificar o valor comercial da série E24 mais próximo da resistência calculada.
4. Determinar as faixas de cores que representa o valor comercial do resistor.
5. Exibir essas informações no display OLED.

## Instruções de uso

1. **Clonar o Repositório**:

```bash
git clone https://github.com/bigodinhojf/Embarcatech_F2T1_ohmimetro.git
```

2. **Compilar e Carregar o Código**:
   No VS Code, configure o ambiente e compile o projeto com os comandos:

```bash	
cmake -G Ninja ..
ninja
```

3. **Interação com o Sistema**:
   - Conecte a placa ao computador.
   - Clique em run usando a extensão do raspberry pi pico.
   - Faça a montagem do circuito conforme mostrado na ficha do projeto.
   - Faça a leitura das informações no display OLED.

## Vídeo de apresentação

O vídeo apresentando o projeto pode ser assistido [clicando aqui](https://youtu.be/5bqItWLqsVY).

## Aluno e desenvolvedor do projeto

<a href="https://github.com/bigodinhojf">
        <img src="https://github.com/bigodinhojf.png" width="150px;" alt="João Felipe"/><br>
        <sub>
          <b>João Felipe</b>
        </sub>
</a>

## Licença

Este projeto está licenciado sob a licença MIT.
