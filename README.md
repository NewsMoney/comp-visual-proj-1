# Projeto 1 — Processamento de Imagens

**Disciplina:** Computação Visual — 2026.2  
**Professor:** André Kishimoto

## Integrantes

- Diogo Cornelio Martins Rosa — 10403852
- José Victor Scheurich Roling — 10418225
- Milton Almeida Leoncio — 10416764
- Vitor Neudl Gandolfi — 10408845

## Base do projeto

O projeto foi desenvolvido a partir do repositório **CompVis262**, utilizando principalmente como referência os seguintes exemplos:

- **03-image:** carregamento de imagens com `SDL_image`;
- **04-invert_image:** conversão para `RGBA32`, manipulação individual de pixels e atualização da textura;
- **05-filter_image:** separação entre a superfície original e o resultado processado;
- **06-primitives:** desenho de primitivas utilizado na janela do histograma e nos botões da interface.

A estrutura original foi refatorada em arquivos `.c` e `.h`, separando as responsabilidades de:

- carregamento e manipulação da imagem;
- cálculo de histograma e equalização;
- interface gráfica;
- fluxo principal do programa.

## Funcionalidades implementadas

O programa possui as seguintes funcionalidades:

- recebe o caminho da imagem pela linha de comando;
- identifica erros de arquivo inexistente ou imagem inválida;
- detecta se a imagem é colorida ou já está em escala de cinza;
- converte imagens coloridas para escala de cinza utilizando:

  ```text
  Y = 0.2125R + 0.7154G + 0.0721B
  ```

- cria uma janela principal inicialmente em `1024x768` e uma janela secundária para informações da imagem;
- calcula e exibe um histograma com 256 níveis de intensidade;
- calcula a média e o desvio padrão dos níveis de intensidade;
- classifica a imagem quanto à intensidade em **clara**, **média** ou **escura**;
- classifica o contraste em **alto**, **médio** ou **baixo**;
- realiza a equalização do histograma;
- permite retornar à versão original em escala de cinza sem recarregar o arquivo;
- alterna a exibição entre `1024x768` e a resolução original da imagem;
- salva a imagem atualmente exibida em `output_image.png` ao pressionar a tecla `S`;
- utiliza `SDL_ttf` para renderização dos textos da janela secundária.

## Critérios adotados para classificação

O enunciado solicita a classificação da intensidade e do contraste, mas não estabelece limiares específicos. Por isso, o grupo adotou os critérios descritos abaixo.

### Intensidade

A classificação é feita a partir da média dos níveis de intensidade, considerando o intervalo `[0, 255]`:

| Classificação | Critério |
|---|---|
| Escura | média < 85 |
| Média | 85 ≤ média < 170 |
| Clara | média ≥ 170 |

### Contraste

A classificação é feita a partir do desvio padrão:

| Classificação | Critério |
|---|---|
| Baixo | desvio padrão < 42,5 |
| Médio | 42,5 ≤ desvio padrão < 85 |
| Alto | desvio padrão ≥ 85 |

O critério considera que o desvio padrão máximo possível para uma imagem de 8 bits é aproximadamente `127,5`. Esse intervalo foi dividido em três faixas para determinar as classificações de contraste.

## Fonte utilizada

A família tipográfica utilizada pelo programa é **DejaVu Sans**.

A fonte deve estar disponível no seguinte caminho relativo ao projeto:

```text
src/07-proj1/assets/DejaVuSans.ttf
```

Dessa forma, o funcionamento do programa não depende das fontes instaladas no Windows ou em outro sistema operacional.

> **Importante:** caso o arquivo `DejaVuSans.ttf` não esteja presente no pacote recebido, ele deve ser adicionado à pasta `assets` antes da execução do programa.

## Ambiente de desenvolvimento

O projeto foi desenvolvido e testado no seguinte ambiente:

| Componente | Configuração |
|---|---|
| Sistema operacional | Windows |
| Compilador | GCC |
| Editor | Visual Studio Code |
| SDL3 | 3.4.16 |
| SDL3_image | 3.4.6 |
| SDL3_ttf |

O `makefile` segue a estrutura utilizada nos exemplos da disciplina e considera, por padrão, o seguinte caminho para a SDL:

```makefile
SDL_DIR = d:\dev\compvis\libs\SDL3
```

Caso as bibliotecas estejam instaladas em outro diretório, altere a variável `SDL_DIR` no `makefile`.

## Compilação

No terminal do Visual Studio Code, acesse o diretório:

```text
src/07-proj1
```

Em seguida, execute:

```bash
mingw32-make
```

O `makefile` gera o executável:

```text
projeto1.exe
```

Além disso, as seguintes bibliotecas são copiadas para a pasta do executável:

```text
SDL3.dll
SDL3_image.dll
SDL3_ttf.dll
```

## Execução

O caminho da imagem deve ser obrigatoriamente informado pela linha de comando.

### Sintaxe

```powershell
.\projeto1.exe caminho\para\imagem.png
```

### Exemplo

```powershell
.\projeto1.exe .\teste.png
```

Caso nenhum caminho seja informado, ou caso o arquivo fornecido seja inválido, o programa informa o erro e encerra a execução.

## Controles

| Controle | Ação |
|---|---|
| **Equalizar / Ver original** | Alterna entre a imagem equalizada e a versão original em escala de cinza |
| **Resolução original / 1024x768** | Alterna o tamanho de exibição da imagem |
| `S` | Salva a imagem atual como `output_image.png` |
| `ESC` | Encerra o programa |

## Organização dos arquivos

```text
docs/
├── projeto - Analise final.pdf
├── projeto - Analise inicial.pdf
src/07-proj1/
├── main.c
├── types.h
├── image.c
├── image.h
├── histogram.c
├── histogram.h
├── gui.c
├── gui.h
├── makefile
└── assets/
    ├── DejaVuSans.ttf
    └── README_FONTE.txt
```

### Responsabilidade dos arquivos

- `main.c`: inicialização da aplicação e controle do fluxo principal;
- `types.h`: estruturas e tipos compartilhados pelo projeto;
- `image.c` / `image.h`: carregamento, conversão e manipulação das imagens;
- `histogram.c` / `histogram.h`: cálculo do histograma, estatísticas e equalização;
- `gui.c` / `gui.h`: criação das janelas, renderização da interface e tratamento dos elementos gráficos;
- `makefile`: configuração da compilação e cópia das bibliotecas necessárias;
- `assets/`: arquivos auxiliares utilizados pela aplicação, incluindo a fonte.
