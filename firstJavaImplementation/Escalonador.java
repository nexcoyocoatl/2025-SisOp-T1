import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

public class Escalonador {
    public static List<Programa> readyQueue = new LinkedList<>();
    public static List<Programa> waitQueue = new LinkedList<>();
    public static List<Programa> creating = new LinkedList<>();
    public static void main(String[] args) {

        auto2();
        //criarProgramas();
        // listarProgramas();

        executarProgramas();
    }

    // cria pseudoprogramas com base no exemplo dos slides de escalonamento
    // https://moodle.pucrs.br/pluginfile.php/4882481/mod_resource/content/4/escalonamento.pdf
    // pag 24
    public static void auto() {
        Programa a1 = new Programa(50,75,1,25,0);
        Programa a2 = new Programa(80,70,2,35,0);
        
        creating.add(a1); creating.add(a2);
    }

    public static void auto2() {
        Programa a1 = new Programa(5,15,1,3,2);
        Programa a2 = new Programa(3,10,2,2,0);
        
        creating.add(a1); creating.add(a2);
    }

    // Problema: processador em idle (OK)
    // Problema: arrivalTime (OK)
    // Problema: perda de deadline (OK)
    public static void executarProgramas() {

        int counter = 0; // contador de unidades de tempo

        // programa dummy para quando processador estiver em idle
        Programa dummy = new Programa(999, 999, 999, 999, 999);
        
        Programa p = dummy;
        while (!readyQueue.isEmpty() || !waitQueue.isEmpty() || !creating.isEmpty()) {

            // verifica por processos que nao foram inicializados ainda
            if (!creating.isEmpty()) {
                Iterator<Programa> it = creating.iterator();
                while (it.hasNext()) {
                    Programa i = it.next();
                    if (i.arrivalTime == counter) {
                        System.out.printf("Adicionando programa %d a readyQueue em %d\n", i.id, counter);
                        readyQueue.add(i);
                        it.remove();
                    }
                }
            }

            // processador em idle
            if (readyQueue.isEmpty()) {
                System.out.println("idle em " + counter);
                if (!waitQueue.isEmpty()) {
                    Iterator<Programa> it = waitQueue.iterator();
                    while (it.hasNext()) {
                        Programa i = it.next();
                        if (counter >= i.nextDeadline) {
                            i.nextDeadline = counter + i.deadline;
                            i.tempoRestante = i.tempo;
                            it.remove();
                            readyQueue.add(i);
                        }
                    }
                }
                counter++;
                continue;
            }

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

            // Programa terminado
            if (p.pc == p.numInstrucoes) {
                System.out.printf("Programa %d terminado em %d\n", p.id, counter);
                readyQueue.remove(p);
                p = dummy;
            }

            // Deadline perdida
            if (p.nextDeadline == counter && p.tempoRestante != 0) {
                System.out.printf("Deadline de programa %d perdida em %d\n", p.id, counter);
                p.nextDeadline = counter + p.deadline;
                p.tempoRestante = p.tempo;
            } 

            // Deadline satisfeita
            if (p.tempoRestante == 0) {
                System.out.printf("Deadline de programa %d satisfeita em %d\n", p.id, counter);
                readyQueue.remove(p);
                waitQueue.add(p);

                // troca para outro programa
                if (!readyQueue.isEmpty()){
                    p = readyQueue.get(0);
                    System.out.printf("Trocando para programa %d em %d\n", p.id, counter);
                } else {
                    p = dummy;
                }
            }

            
            // verifica por programas da waitQueue que devem voltar para readyQueue
            if (!waitQueue.isEmpty()) {
                Iterator<Programa> it = waitQueue.iterator();
                while (it.hasNext()) {
                    Programa i = it.next();
                    if (counter >= i.nextDeadline) {
                        i.nextDeadline = counter + i.deadline;
                        i.tempoRestante = i.tempo;
                        it.remove();
                        readyQueue.add(i);
                    }
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

            System.out.printf("Forneça o tempo de chegada do programa %d\n> ", i+1);
            int arrivalTime = in.nextInt();

            System.out.printf("Criando programa %d...\n", i+1);
            Programa prog = new Programa(deadline, numInstrucoes, i+1, tempo, arrivalTime);

            System.out.printf("Adicionando programa %d a creating...\n", i+1);
            creating.add(prog);

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

    // lista programas existentes na waitQueue
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

    // lista programas existentes na creating
    public static void printCreating() {
        System.out.println("Programas na creating:");
        for (Programa p : waitQueue) {
            System.out.println("[");
            System.out.println(  "id: "+p.id+
                               "\ndeadline: "+p.deadline+
                               "\nnumInstrucoes: "+p.numInstrucoes+
                               "\narrivalTime: "+p.arrivalTime);
            System.out.println("]");
        }
    }
}
