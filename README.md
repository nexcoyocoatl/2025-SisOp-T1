# Sistemas Operacionais T1 - Escalonador EDF

## Documentação do Programa:

Este programa tem como objetivo a solução de escalonamento por EDF de um processador, com a abstração de programas em uma linguagem de programação similar a uma linguagem assembler.
  
Os programas em formato .txt com o Assembly hipotético deverão estar contidos na pasta “./programs” contida um nível abaixo do diretório do código fonte e executável.

Dentro da pasta “./programs” também há a opção de incluir um arquivo de texto “uservalues.txt” com os valores de arrival time, processing time e deadline de cada programa que será executado; cada linha se refere a estes 3 valores para cada programa que será lido em ordem alfabética do nome de seu arquivo. Como na figura abaixo:

![image1](https://github.com/nexcoyocoatl/2025-SisOp-T1/blob/main/readme/image1.png)

O arquivo de texto deve ser preenchido desta forma:

<img align="left" alt="image2" src="https://github.com/nexcoyocoatl/2025-SisOp-T1/blob/main/readme/image2.png">

programa 1, index 0 - arrival time: 0, processing time: 3, deadline: 9  
programa 2, index 1 - arrival time: 0, processing time: 4, deadline: 25  
programa 3, index 2 - arrival time: 0, processing time: 5, deadline: 18  
programa 4, index 3 - arrival time: 4, processing time: 2, deadline: 70  
programa 5, index 4 - arrival time: 6, processing time: 8, deadline: 25  
programa 6, index 5 - arrival time: 10, processing time: 4, deadline: 7  
programa 7, index 6 - arrival time: 0, processing time: 4. deadline: 9

 
Se não houverem linhas o suficiente, será pedido ao usuário para que preencha as demais. Se o arquivo não existir ou estiver vazio, o usuário preencherá todos os valores durante a execução do programa.

### Para executar:  
**Linux:** primeiro execute o comando “./make” no terminal no mesmo diretório dos arquivos fonte .c para realizar a compilação, e então execute “./sisop_t1” para rodar nossa solução do trabalho.

**Windows:** com o compilador MingW32 instalado, digite “mingw32-make -f Makefile.mk” no cmd no mesmo diretório dos arquivos fonte .c para realizar a compilação. Execute “sisop_t1.exe” para rodar o programa. (Este programa não foi testado extensivamente em Windows, porém)

Obs: Nossa solução apenas lê os programas em arquivo de texto com instruções separadas por linhas, incluindo labels. Não tendo suporte, portanto para casos específicos como este:

![image3](https://github.com/nexcoyocoatl/2025-SisOp-T1/blob/main/readme/image3.png)
