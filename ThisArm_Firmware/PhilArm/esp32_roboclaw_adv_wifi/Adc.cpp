
#include "ioVars.h"

#define ADC1_0  36
#define ADC1_1  39
#define ADC1_2  34
#define ADC1_3  35

int setupAdc() {
  setupioVar ("ADC_0", AN_IN, ADC1_0);
  setupioVar ("ADC_1", AN_IN, ADC1_1);
  setupioVar ("ADC_2", AN_IN, ADC1_2);
  setupioVar ("ADC_3", AN_IN, ADC1_3);
}
 
