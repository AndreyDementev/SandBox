
template <typename T>
String ToBinary(T Number){
  String s;
  for (int i = 0; i < sizeof(T) * 8; i++){
    s = (String)((Number >> i) & 1) + s;
    if ((i+1)%8 == 0){
      s = " " + s;
    }
  }
  return s;
}


uint32_t Period;

void watchdogSetup(void) {
}

uint32_t watchdogGetPeriod (void)        { return (WDT->WDT_MR & WDT_MR_WDV_Msk)   << 2; }
uint8_t  watchdogGet_MR_WDFIEN (void)    { return (WDT->WDT_MR & WDT_MR_WDFIEN)    >> WDT_MR_WDFIEN; }
uint8_t  watchdogGet_MR_WDRSTEN (void)   { return (WDT->WDT_MR & WDT_MR_WDRSTEN)   >> WDT_MR_WDRSTEN; }
uint8_t  watchdogGet_MR_WDRPROC (void)   { return (WDT->WDT_MR & WDT_MR_WDRPROC)   >> WDT_MR_WDRPROC; }
uint8_t  watchdogGet_MR_WDDIS (void)     { return (WDT->WDT_MR & WDT_MR_WDDIS)     >> WDT_MR_WDDIS; }
uint8_t  watchdogGet_MR_WDDBGHLT (void)  { return (WDT->WDT_MR & WDT_MR_WDDBGHLT)  >> WDT_MR_WDDBGHLT; }
uint8_t  watchdogGet_MR_WDIDLEHLT (void) { return (WDT->WDT_MR & WDT_MR_WDIDLEHLT) >> WDT_MR_WDIDLEHLT; }
uint8_t  watchdogGet_SR_WDUNF (void)     { return (WDT->WDT_SR & WDT_SR_WDUNF)     >> WDT_SR_WDUNF; }
uint8_t  watchdogGet_SR_WDERR (void)     { return (WDT->WDT_SR & WDT_SR_WDERR)     >> WDT_SR_WDERR; }
uint32_t  watchdog_Get_MR (void) { return (WDT->WDT_MR); }
uint32_t  watchdog_Get_SR (void) { return (WDT->WDT_SR); }
uint32_t  watchdog_Get_CR (void) { return (WDT->WDT_CR); }

// WDT_MR_WDFIEN    (0x1u << 12) brief (WDT_MR) Watchdog Fault Interrupt Enable
// WDT_MR_WDRSTEN   (0x1u << 13) brief (WDT_MR) Watchdog Reset Enable
// WDT_MR_WDRPROC   (0x1u << 14) brief (WDT_MR) Watchdog Reset Processor
// WDT_MR_WDDIS     (0x1u << 15) brief (WDT_MR) Watchdog Disable
// WDT_MR_WDDBGHLT  (0x1u << 28) brief (WDT_MR) Watchdog Debug Halt
// WDT_MR_WDIDLEHLT (0x1u << 29) brief (WDT_MR) Watchdog Idle Halt
// WDT_SR_WDUNF     (0x1u << 0)  brief (WDT_SR) Watchdog Underflow
// WDT_SR_WDERR     (0x1u << 1)  brief (WDT_SR) Watchdog Error
 
void setup() {
  Serial.begin(9600);
  Serial.println();
  
/*  Serial.println( watchdogGetPeriod(), BIN );
  Serial.println(WDT_GetStatus( WDT ));
  Serial.println( watchdogGet_MR_WDFIEN(), BIN );
  Serial.println( watchdogGet_MR_WDRSTEN(), BIN );
  Serial.println( watchdogGet_MR_WDRPROC(), BIN );
  Serial.println( watchdogGet_MR_WDDIS(), BIN );
  Serial.println( watchdogGet_MR_WDDBGHLT(), BIN );
  Serial.println( watchdogGet_MR_WDIDLEHLT(), BIN );
  Serial.println( watchdogGet_SR_WDUNF (), BIN );
  Serial.println( watchdogGet_SR_WDERR (), BIN );*/
}


void loop() {
  
  Serial.println(ToBinary(watchdog_Get_MR ()));
  Serial.println(ToBinary(watchdog_Get_SR ()));
  Serial.println(ToBinary(watchdog_Get_CR ()));
  Serial.println();

//  WDT_Disable (WDT);
  watchdogEnable(12000);


  Serial.println(ToBinary(watchdog_Get_MR ()));
  Serial.println(ToBinary(watchdog_Get_SR ()));
  Serial.println(ToBinary(watchdog_Get_CR ()));
  Serial.println("------------");
  Serial.println();
  
  while(1);
}
