#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define Nbit 8 // A arquitetura Ahmes opera em 8 bits
#define MAX (int)(pow(2, Nbit))

void attFlagsNZ(unsigned int n);

// Flags Neander
int flagN = 0;
int flagZ = 0;

// Flags Ahmes
int flagC = 0;
int flagB = 0;
int flagV = 0;

// --- COMANDOS ORIGINAIS DO NEANDER ---
#define NOP pc++;

#define HLT pc++;

#define STA(m) {\
    mem[m] = ac & 0xFF;\
    pc += 2;\
}

#define LDA(m) {\
    ac = mem[m] & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

#define ADD(m) {\
    unsigned int temp = ac + mem[m];\
    flagC = (temp > 255) ? 1 : 0;\
    flagV = (!((ac ^ mem[m]) & 0x80) && ((ac ^ temp) & 0x80)) ? 1 : 0;\
    ac = temp & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

#define OR(m) {\
    ac = (ac | mem[m]) & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

#define AND(m) {\
    ac = (ac & mem[m]) & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

#define NOT {\
    ac = (~ac) & 0xFF;\
    attFlagsNZ(ac);\
    pc++;\
}

#define JMP(end) {\
    pc = end;\
}

#define JN(end) {\
    if(flagN) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JZ(end) {\
    if(flagZ) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

// --- ADIÇÃO DOS COMANDOS EM AHMES ---
#define SUB(m) {\
    unsigned int temp = ac - mem[m];\
    flagB = (ac < mem[m]) ? 1 : 0;\
    flagV = (((ac ^ mem[m]) & 0x80) && ((ac ^ temp) & 0x80)) ? 1 : 0;\
    ac = temp & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

#define ADC(m) {\
    unsigned int op = mem[m];\
    unsigned int temp = ac + op + flagC;\
    flagC = (temp > 255) ? 1 : 0;\
    flagV = (!((ac ^ op) & 0x80) && ((ac ^ temp) & 0x80)) ? 1 : 0;\
    ac = temp & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

#define SBC(m) {\
    unsigned int op = mem[m];\
    unsigned int subtotal = op + flagB;\
    unsigned int temp = ac - subtotal;\
    flagB = (ac < subtotal) ? 1 : 0;\
    flagV = (((ac ^ op) & 0x80) && ((ac ^ temp) & 0x80)) ? 1 : 0;\
    ac = temp & 0xFF;\
    attFlagsNZ(ac);\
    pc += 2;\
}

// --- Deslocamento e Rotação ---
#define SHR {\
    flagC = ac & 0x01;\
    ac = (ac >> 1) & 0xFF;\
    attFlagsNZ(ac);\
    pc++;\
}

#define SHL {\
    flagC = (ac & 0x80) ? 1 : 0;\
    ac = (ac << 1) & 0xFF;\
    attFlagsNZ(ac);\
    pc++;\
}

#define ROR {\
    int bit0 = ac & 0x01;\
    ac = ((ac >> 1) | (flagC << 7)) & 0xFF;\
    flagC = bit0;\
    attFlagsNZ(ac);\
    pc++;\
}

#define ROL {\
    int bit7 = (ac & 0x80) ? 1 : 0;\
    ac = ((ac << 1) | flagC) & 0xFF;\
    flagC = bit7;\
    attFlagsNZ(ac);\
    pc++;\
}

// --- Desvios Condicionais (Jumps) ---
#define JP(end) {\
    if(!flagN) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JNZ(end) {\
    if(!flagZ) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JC(end) {\
    if(flagC) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JNC(end) {\
    if(!flagC) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JB(end) {\
    if(flagB) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JNB(end) {\
    if(!flagB) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JV(end) {\
    if(flagV) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define JNV(end) {\
    if(!flagV) {\
        pc = end;\
    } else {\
        pc += 2;\
    }\
}

#define PRINT(i) printf("%u \n", i)


int carregarMemoria(const char *nomeArquivo, unsigned int mem[]) {
    FILE *fp = fopen(nomeArquivo, "r");
    if (fp == NULL) {
        return 0;
    }

    char linha[128];
    unsigned int idxSeq = 0;

    while (fgets(linha, sizeof(linha), fp) != NULL) {
        unsigned int col1 = 0, col2 = 0, col3 = 0;
        int lidos = sscanf(linha, "%u %u %u", &col1, &col2, &col3);

        if (lidos == 1 && idxSeq < 256) {
            mem[idxSeq++] = col1 & 0xFF;
        } else if (lidos >= 2 && col1 < 256) {
            mem[col1] = col2 & 0xFF;
            if (lidos == 3 && (col1 + 1) < 256) {
                mem[col1 + 1] = col3 & 0xFF;
            }
        }
    }

    fclose(fp);
    return 1;
}

void imprimirMemoria(const unsigned int mem[]) {
    for (unsigned int i = 0; i < 32; i++) {
        printf("%3u:%3u  %3u:%3u  %3u:%3u  %3u:%3u  %3u:%3u  %3u:%3u  %3u:%3u  %3u:%3u\n",
            i, mem[i],
            i + 32, mem[i + 32],
            i + 64, mem[i + 64],
            i + 96, mem[i + 96],
            i + 128, mem[i + 128],
            i + 160, mem[i + 160],
            i + 192, mem[i + 192],
            i + 224, mem[i + 224]);
    }
}

int main(int argc, char *argv[]) {
    unsigned int pc = 0;
    unsigned int ac = 0;
    unsigned int mem[256];

    for (int i = 0; i < 256; i++) {
        mem[i] = 0;
    }

    const char *nomeArquivo = (argc > 1) ? argv[1] : "programa.txt";

    if (!carregarMemoria(nomeArquivo, mem)) {
        printf("Erro: o arquivo '%s' nao pode ser aberto.\n", nomeArquivo);
        return 1;
    }

    printf("\nMemoria antes do processamento:\n");
    imprimirMemoria(mem);

    int rodando = 1;
    while (rodando && pc < 256) {
        unsigned int opcode = mem[pc];
        unsigned int end = (pc + 1 < 256) ? mem[pc + 1] : 0;

        switch (opcode) {
            case 0:   NOP; break;              
            case 16:  STA(end); break;         
            case 32:  LDA(end); break;         
            case 48:  ADD(end); break;         
            case 64:  OR(end); break;         
            case 80:  AND(end); break;         
            case 96:  NOT; break;              
            case 112: SUB(end); break;         
            case 116: ADC(end); break;         
            case 120: SBC(end); break;         
            case 128: JMP(end); break;         
            case 144: JN(end); break;          
            case 148: JP(end); break;          
            case 152: JV(end); break;          
            case 156: JNV(end); break;         
            case 160: JZ(end); break;          
            case 164: JNZ(end); break;         
            case 176: JC(end); break;          
            case 180: JNC(end); break;         
            case 184: JB(end); break;          
            case 188: JNB(end); break;         
            case 224: SHR; break;              
            case 225: SHL; break;              
            case 226: ROR; break;              
            case 227: ROL; break;              
            case 240: HLT; rodando = 0; break; 
            default:
                printf("\nInstrucao invalida (opcode %u) no endereco %u.\n", opcode, pc);
                rodando = 0;
                break;
        }
    }

    printf("\nMemoria apos o processamento:\n");
    imprimirMemoria(mem);

    printf("\n--- ESTADO FINAL DOS REGISTRADORES E FLAGS ---\n");
    printf("PC = %u\n", pc);
    printf("AC = %u (complemento de 2: %d)\n", ac, (signed char)ac);
    printf("Flags -> N = %d | Z = %d | C = %d | B = %d | V = %d\n",
           flagN, flagZ, flagC, flagB, flagV);

    return 0;
}

//Atualiza as flags N e Z após as operações no AC
void attFlagsNZ(unsigned int n){
    flagN = (n & 0x80) ? 1 : 0; //Bit 7 setado indica número negativo em complemento de 2
    flagZ = ((n & 0xFF) == 0) ? 1: 0;
}