#ifndef OPCODES_H
#define OPCODES_H

#define OC_GOTO     "GOTO"
#define OC_IF       "IF"
#define OC_HALT     "HALT"
#define OC_CALL     "CALL"
#define OC_PARAM    "PARAM"
#define OC_LABEL    "LABEL" 

#define OC_ASSIGN   ":="

#define OC_ADDI     "ADDI"
#define OC_SUBI     "SUBI"
#define OC_MULI     "MULI"
#define OC_DIVI     "DIVI"
#define OC_MODI     "MODI"

#define OC_ADDF     "ADDF"
#define OC_SUBF     "SUBF"
#define OC_MULF     "MULF"
#define OC_DIVF     "DIVF"
#define OC_CHSF     "CHSF"
#define OC_CHSI     "CHSI"
#define OC_I2F      "I2F"
#define OC_F2I      "F2I"

#define REL_GT      "GT"
#define REL_LT      "LT"
#define REL_GE      "GE"
#define REL_LE      "LE"
#define REL_EQ      "EQ"
#define REL_NE      "NE"

#define REL_GTI     "GTI"
#define REL_LTI     "LTI"
#define REL_GEI     "GEI"
#define REL_LEI     "LEI"
#define REL_EQI     "EQI"
#define REL_NEI     "NEI"

#define REL_GTF     "GTF"
#define REL_LTF     "LTF"
#define REL_GEF     "GEF"
#define REL_LEF     "LEF"
#define REL_EQF     "EQF"
#define REL_NEF     "NEF"

#define REL_SUFFIX_I   "I"
#define REL_SUFFIX_F   "F"
#define REL_APPEND(str, suf)  str suf

#define PROC_PUTI   "PUTI"
#define PROC_PUTF   "PUTF"

#endif