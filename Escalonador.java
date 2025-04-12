import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

public class Escalonador {
    public static List<Programa> readyQueue = new LinkedList<>();
    public static List<Programa> waitQueue = new LinkedList<>();
    public static void main(String[] args) {

        auto();
        // criarProgramas();
        // listarProgramas();

        executarProgramas();
    }

    // cria pseudoprogramas com base no exemplo dos slides de escalonamento
    // https://moodle.pucrs.br/pluginfile.php/4882481/mod_resource/content/4/escalonamento.pdf
    // pag 24
    public static void auto() {
        Programa a1 = new Programa(50,75,1,25);
        Programa a2 = new Programa(80,70,2,35);
        readyQueue.add(a2);
        readyQueue.add(a1);
    }

    // Problema conhecido: se um programa tiver input de usuario (syscall 2), trava toda execucao
    // Solucao proposta: threads ou forks, criacao de uma ioQueue

    // Problema conhecido 2: processador em idle, quando não há programas em readyQueue, ele quebra
    public static void executarProgramas() {

        int counter = 0; // contador de unidades de tempo

        Programa p = readyQueue.get(0);
        while (!readyQueue.isEmpty() || !waitQueue.isEmpty()) {
            // seleciona programa com deadline mais proxima em readyQueue
            for (Programa temp : readyQueue) {
                if (temp.nextDeadline < p.nextDeadline) {
                    System.out.printf("Trocando para programa %d em %d\n", temp.id, counter);
                    p = temp;
                }
            }
            

            // "Le" instrucoes do programa
            p.pc++;
            p.tempoRestante--;
            counter++;

            // Deadline satisfeita
            if (p.tempoRestante == 0) {
                System.out.printf("Deadline de programa %d satisfeita em %d\n", p.id, counter);
                readyQueue.remove(p);
                waitQueue.add(p);

                // troca para outro programa
                p = readyQueue.get(0);
                System.out.printf("Trocando para programa %d em %d\n", p.id, counter);
            }

            // Programa terminado
            if (p.pc == p.numInstrucoes) {
                System.out.printf("Programa %d terminado em %d\n", p.id, counter);
                readyQueue.remove(p);
            }
            
            // verifica por programas da waitQueue que devem voltar para readyQueue
            for (Programa i : waitQueue) {
                if (counter == i.nextDeadline) {
                    i.nextDeadline += i.deadline;
                    i.tempoRestante += i.tempo;
                    waitQueue.remove(i);
                    readyQueue.add(i);
                }
            }
        }
    }

    // cria pseudoprogramas a partir de input de usuario
    public static void criarProgramas() {
        Scanner in = new Scanner(System.in);

        System.out.print("Quantos programas serao executados?\n> ");
        int numProgs = in.nextInt();

        for (int i = 0; i < numProgs; i++) {
            System.out.printf("Forneca a deadline do programa %d\n> ", i+1);
            int deadline = in.nextInt();

            System.out.printf("Forneca o tempo de execucao do programa %d\n> ", i+1);
            int tempo = in.nextInt();

            System.out.printf("Forneca o numero de instrucoes do programa %d\n> ", i+1);
            int numInstrucoes = in.nextInt();

            System.out.printf("Criando programa %d...\n", i+1);
            Programa prog = new Programa(deadline, numInstrucoes, i+1, tempo);

            System.out.printf("Adicionando programa %d a readyQueue...\n", i+1);
            readyQueue.add(prog);

            System.out.println("Ok\n");
        }

        in.close();
    }

    // lista programas existentes na readyQueue
    public static void printReady() {
        System.out.println("Programas na readyQueue:");
        for (Programa p : readyQueue) {
            System.out.println("[");
            System.out.println(  "id: "+p.id+
                               "\ndeadline: "+p.deadline+
                               "\nnumInstrucoes:"+p.numInstrucoes);
            System.out.println("]");
        }
    }

    public static void printWait() {
        System.out.println("Programas na waitQueue:");
        for (Programa p : waitQueue) {
            System.out.println("[");
            System.out.println(  "id: "+p.id+
                               "\ndeadline: "+p.deadline+
                               "\nnumInstrucoes:"+p.numInstrucoes);
            System.out.println("]");
        }
    }
}