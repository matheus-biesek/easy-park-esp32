# Sistema de Estacionamento - Parte Lógica (ESP32)

Este projeto utiliza o **ESP32** para a integração entre a parte física do estacionamento controlada pelo **Arduino** e a aplicação web. O ESP32 realiza requisições HTTP para a aplicação web, coleta e envia dados sobre o status das vagas, cancela e mensagens de administração, além de gerenciar a comunicação com o Arduino via **Serial2**.

## Funcionalidades

1. **Requisições para o Backend da Aplicação Web**:
   - O ESP32 realiza requisições para verificar:
     - **Status da Cancela**: Verifica se deve ser aberta ou fechada.
     - **Mensagens do Administrador**: Coleta mensagens que serão exibidas no painel LCD.
   - Além disso, o ESP32 envia o **status das vagas** para a aplicação web.

2. **Comunicação Serial com o Arduino**:
   - O ESP32 recebe o status das vagas via **Serial2**, enviado pelo Arduino no formato:
     ```
     vacancyOneStatus:true
     ```
   - O ESP32 também envia o status da cancela e as mensagens do administrador para o Arduino via **Serial2**:
     ```
     messageAdm:Bom dia!
     gateOneStatus:true
     ```

3. **Orquestração Multi-Core**:
   - O ESP32 utiliza os dois núcleos (cores) para separar as responsabilidades:
     - **Core 0**: Responsável pela comunicação com o Arduino via **Serial2**.
     - **Core 1**: Responsável por realizar as requisições HTTP para o backend da aplicação web.
   - A comunicação entre os dois núcleos é feita utilizando **variáveis booleanas**, permitindo que um núcleo sinalize para o outro quando uma ação deve ser tomada (envio de mensagens ou execução de requisições).

## Estrutura Orientada a Objetos

Para garantir uma implementação escalável e de fácil manutenção, o projeto foi desenvolvido com uma **arquitetura orientada a objetos**. O código foi estruturado em três classes principais:

1. **Classe de Comunicação entre Núcleos**:
   - Armazena as **variáveis booleanas** que servem para sincronizar as ações entre os dois núcleos.
   - Cada variável indica se uma ação precisa ser realizada, como o envio de uma mensagem via **Serial2** ou a realização de uma requisição web.

2. **Classe para Comunicação Serial**:
   - Responsável por gerenciar o envio e recebimento de dados pela **Serial2**.
   - Contém métodos para interpretar as mensagens recebidas no formato `chave:valor`, como:
     ```
     vacancyOneStatus:true
     ```
   - Realiza o envio das respostas sobre o status da cancela e mensagens administrativas para o Arduino.

3. **Classe para Requisições Web**:
   - Gerencia as requisições HTTPS para o backend da aplicação web.
   - Possui métodos para verificar o status da cancela e coletar as mensagens do administrador, além de enviar o status das vagas atualizado para o servidor.

## Fluxo de Operação

### 1. Comunicação com o Arduino via Serial2
O ESP32 recebe o status das vagas via **Serial2**, no formato `chave:valor` (por exemplo, `vacancyOneStatus:true`). Um script no ESP32 processa essa string:
- A primeira parte (`vacancyOneStatus`) identifica a variável relacionada.
- A segunda parte (`true`) é o valor associado a essa variável.

Após processar essa informação, o ESP32 pode enviar dados para o Arduino no formato:
```
messageAdm:Bom dia!
gateOneStatus:true
```

### 2. Requisições HTTPS
O ESP32 realiza requisições para o backend web em dois casos principais:
- **Coleta de mensagens do administrador**: Para atualizar o painel LCD (Arduino).
- **Verificação do status da cancela**: Para abrir ou fechar o estacionamento (Arduino).

O status das vagas é enviado periodicamente para a aplicação web para manter a interface do usuário atualizada.

### 3. Sincronização entre Núcleos
Os dois núcleos do ESP32 (Core 0 e Core 1) se comunicam utilizando variáveis booleanas:
- Quando uma variável booleana se torna verdadeira, ela indica que uma ação deve ser realizada, seja pelo **Core 0** (envio/recebimento de mensagens via Serial) ou pelo **Core 1** (realização de requisições HTTP).
- Após a execução da ação, a variável é redefinida para falso, indicando que a tarefa foi concluída.

## Componentes Utilizados

- **ESP32**: Processador principal para comunicação com a aplicação web e o Arduino.
- **Serial2**: Interface de comunicação entre o ESP32 e o Arduino.

## Exemplo de Código para Processamento de Mensagens

```cpp
// Processar mensagem recebida via Serial2
String inputString = "vacancyOneStatus:true";
String variableName = "vacancyOneStatus";

if (inputString.startsWith(variableName)) {
    String value = inputString.substring(inputString.indexOf(":") + 1);
    // Processar o valor (neste caso "true")
}
```

## Organização das Classes

### Classe de Comunicação entre Núcleos
Esta classe gerencia as variáveis booleanas utilizadas para coordenar as ações entre os dois núcleos. 

### Classe de Comunicação Serial
Gerencia o envio e recebimento de dados pela Serial2, utilizando métodos para processar as mensagens no formato `chave:valor`.

### Classe de Requisições Web
Responsável por realizar requisições HTTP para o backend da aplicação web, coletando ou enviando os dados necessários para o funcionamento do sistema.

