public class Programa {
    public int id; // nao é utilizado em nenhum momento, pode-se usar como index se usar array
    public final int deadline; // constante para somar a deadline
    public int numInstrucoes; // numero de instrucoes hipoteticas
    public int pc; // program counter
    public final int tempo; // constante para verificar satisfacao de deadline
    public int nextDeadline; // proxima deadline em unidades de tempo
    public int tempoRestante; // tempo restante de execucao ate que deadline seja satisfeita
    public int arrivalTime; // tempo de chegada

    public Programa (int deadline, int numInstrucoes, int id, int tempo, int arrivalTime) {
        this.deadline = deadline;
        this.numInstrucoes = numInstrucoes;
        this.id = id;
        this.pc = 0;
        this.tempo = tempo;
        this.nextDeadline = deadline;
        this.tempoRestante = tempo;
        this.arrivalTime = arrivalTime;
    }

}
