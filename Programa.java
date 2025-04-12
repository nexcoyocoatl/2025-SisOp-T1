public class Programa {
    public int id;
    public int deadline;
    public int numInstrucoes;
    public int pc;
    public int tempo;
    public int nextDeadline;
    public int timesCompleted;
    public int tempoRestante;

    public Programa (int deadline, int numInstrucoes, int id, int tempo) {
        this.deadline = deadline;
        this.numInstrucoes = numInstrucoes;
        this.id = id;
        this.pc = 0;
        this.tempo = tempo;
        this.nextDeadline = deadline;
        this.timesCompleted = 0;
        this.tempoRestante = tempo;
    }

}
