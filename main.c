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
    goto end;\
}

#define JN(end) {\
    if(flagN) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JZ(end) {\
    if(flagZ) {\
        pc = end;\
        goto end;\
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
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JNZ(end) {\
    if(!flagZ) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JC(end) {\
    if(flagC) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JNC(end) {\
    if(!flagC) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JB(end) {\
    if(flagB) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JNB(end) {\
    if(!flagB) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JV(end) {\
    if(flagV) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define JNV(end) {\
    if(!flagV) {\
        pc = end;\
        goto end;\
    } else {\
        pc += 2;\
    }\
}

#define PRINT(i) printf("%u \n", i)

int main(){
    printf("Testando 123");

    system("pause");
    return 0;
}

//Atualiza as flags N e Z após as operações no AC
void attFlagsNZ(unsigned int n){
    flagN = (n & 0x80) ? 1 : 0; //Bit 7 setado indica número negativo em complemento de 2
    flagZ = ((n & 0xFF) == 0) ? 1: 0;
}