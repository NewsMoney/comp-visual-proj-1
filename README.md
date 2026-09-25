Projeto 1 — Processamento de imagens
Disciplina: Computação Visual — 2026.2
Professor: André Kishimoto

Integrantes
Diogo Cornelio Martins Rosa — 10403852
José Victor Scheurich Roling — 10418225
Milton Almeida Leoncio — 10416764
Vitor Neudl Gandolfi — 10408845
Base do projeto
O projeto foi desenvolvido a partir do repositório CompVis262, principalmente dos exemplos:

03-image: carregamento com SDL_image;
04-invert_image: conversão para RGBA32, manipulação individual de pixels e atualização da textura;
05-filter_image: separação entre superfície original e resultado processado;
06-primitives: desenho de primitivas usado na janela do histograma e nos botões.
A estrutura foi refatorada em arquivos .c e .h para separar carregamento/manipulação da imagem, histograma/equalização, interface e fluxo principal do programa.

Funcionalidades implementadas
Recebe o caminho da imagem pela linha de comando.
Detecta erro de arquivo inexistente ou imagem inválida.
Detecta se a imagem é colorida ou já está em escala de cinza.
Converte imagens coloridas usando Y = 0.2125R + 0.7154G + 0.0721B.
Cria uma janela principal inicialmente em 1024x768 e uma janela secundária filha.
Calcula e exibe um histograma de 256 níveis.
Calcula média e desvio padrão.
Classifica intensidade em clara/média/escura e contraste em alto/médio/baixo.
Equaliza o histograma e permite retornar à versão original em escala de cinza sem recarregar o arquivo.
Alterna a exibição entre 1024x768 e a resolução original.
Salva a imagem atual em output_image.png ao pressionar S.
Usa SDL_ttf para textos na janela secundária.
Critérios adotados para classificação
O enunciado pede as classificações, mas não fixa os limiares. O grupo adotou critérios explícitos:

Intensidade pela média [0,255]:
escura: média < 85;
média: 85 <= média < 170;
clara: média >= 170.
Contraste pelo desvio padrão:
baixo: < 42,5;
médio: 42,5 <= desvio < 85;
alto: >= 85.
O segundo critério considera que o desvio padrão máximo de uma imagem de 8 bits é aproximadamente 127,5, dividindo esse intervalo em três faixas.

Fonte
A família escolhida é DejaVu Sans. O programa carrega a fonte pelo caminho relativo:

src/07-proj1/assets/DejaVuSans.ttf

O arquivo deve ser incluído no repositório do grupo. Dessa forma, o funcionamento não depende das fontes instaladas no Windows ou em outro sistema operacional.

O arquivo .ttf não está incluído neste pacote inicial. Adicione DejaVuSans.ttf à pasta assets antes da execução.

Ambiente usado pelo grupo
Conforme definido na análise inicial:

Sistema operacional: Windows;
Compilador: GCC;
Editor: Visual Studio Code;
SDL3: 3.4.16;
SDL3_image: 3.4.6;
SDL3_ttf: preencher com a versão instalada pelo grupo.
O makefile segue a estrutura dos exemplos da disciplina e parte do caminho:

SDL_DIR = d:\dev\compvis\libs\SDL3

Altere esse caminho se as bibliotecas estiverem em outro diretório.

Compilação
No terminal do VS Code, dentro de src/07-proj1:

mingw32-make

O makefile gera projeto1.exe e copia SDL3.dll, SDL3_image.dll e SDL3_ttf.dll para a pasta do executável.

Execução
A imagem é obrigatoriamente informada na linha de comando:

.\projeto1.exe caminho\para\imagem.png

Exemplo:

.\projeto1.exe .\teste.png

Controles:

botão Equalizar / Ver original: alterna o processamento;
botão Resolução original / 1024x768: alterna o tamanho de exibição;
tecla S: salva output_image.png;
tecla ESC: encerra o programa.
Organização
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