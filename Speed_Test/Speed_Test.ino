// Тест скорости выполнения команд Arduino, версия PRO 2
// Просто помести свой код внутри test() и загрузи прошивку!

inline __attribute__((always_inline))
void test() {
  int a = analogRead(1);
}

volatile uint16_t _timer1_overflows = 0;
void setup() {
  Serial.begin(9600);
  timer1_setup();
  
  TCCR1B = (1 << CS10);
  test();
  TCCR1B = 0x00;

  uint32_t clock_cycles = (((uint32_t)_timer1_overflows << 16) | TCNT1) - 2;
  float runtime = (float)clock_cycles * (1000000.0f / F_CPU);
  float loop_frequency = (float)1000000.0f / runtime;

  Serial.print("cpu clock cycles: ");
  Serial.println(clock_cycles);
  Serial.print("runtime: ");
  Serial.print(runtime, 4);
  Serial.println(" us");
  Serial.print("loop frequency: ");
  Serial.print(loop_frequency, 4);
  Serial.println(" Hz");
}

void loop() {}

void timer1_setup(void) {
  TCCR1A = 0x00;
  TCCR1B = 0x00;
  TCNT1 = 0x00;
  TIMSK1 = (1 << TOIE1);
  _timer1_overflows = 0;
  sei();
}

ISR(TIMER1_OVF_vect) {
  _timer1_overflows++;
}
