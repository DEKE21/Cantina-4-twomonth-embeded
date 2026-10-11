#include <Arduino.h>

class RecoverySystem {
private:
int LEDPin;
int buzzerPin;
public:
RecoverySystem(int LEDPin, int buzzerPin);
void Begin();
void LoopStep();
};