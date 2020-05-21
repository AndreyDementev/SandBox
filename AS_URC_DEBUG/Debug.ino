


void ClearSummWeights(){
  ReadPrivetFromEEPROM(&PD);
  PD.DayWeight   = 0;
  PD.WeekWeight  = 0;
  PD.MonthWeight = 0;
  WritePrivetToEEPROM(&PD);
}

void SetCurrentTime(){
  DsRtc.setTime(21, 05, 00);     // Установка часов 12:00:00 (24часовой формат)
  DsRtc.setDate(31, 03, 2020);   // Установка даты число, месяц, год
  DsRtc.setDOW();                // MONDAY TUESDAY WEDNESDAY THURSDAY FRIDAY SATURDAY SUNDAY
}


void Debug(){
  PneumoStatus = 0;
  AutoLoad = 1;
  MaxWeightStatus = 0;
  MeasureStartTime = 101010101;
  AutoLoadStatus = 1;
  WriteWaiting = 0;
  StartWeight = 101010101;
  NeedToWrite = 0;

cout << "-------------------Print Variabls before\n";
  Serial.println(PneumoStatus);
  Serial.println(AutoLoad);
  Serial.println(MaxWeightStatus);
  Serial.println(MeasureStartTime);
  Serial.println(AutoLoadStatus);
  Serial.println(WriteWaiting);
  Serial.println(StartWeight);
  Serial.println(NeedToWrite);
//cout << "------------------WriteVarToEEPROM\n";
  WriteVarToEEPROM( PneumoStatus,    _PneumoStatus);
  WriteVarToEEPROM( AutoLoad,        _AutoLoad);
  WriteVarToEEPROM( MaxWeightStatus, _MaxWeightStatus);
  WriteVarToEEPROM( MeasureStartTime, _MeasureStartTime);
  WriteVarToEEPROM( AutoLoadStatus,  _AutoLoadStatus);
  WriteVarToEEPROM( WriteWaiting,    _WriteWaiting);
  WriteVarToEEPROM( StartWeight,     _StartWeight);
  WriteVarToEEPROM( NeedToWrite,     _NeedToWrite);
  
//cout << "------------------ReadVarFromEEPROM\n";
  ReadVarFromEEPROM( &PneumoStatus,    _PneumoStatus);
  ReadVarFromEEPROM( &AutoLoad,        _AutoLoad);
  ReadVarFromEEPROM( &MaxWeightStatus, _MaxWeightStatus);
  ReadVarFromEEPROM( &MeasureStartTime, _MeasureStartTime);
  ReadVarFromEEPROM( &AutoLoadStatus,  _AutoLoadStatus);
  ReadVarFromEEPROM( &WriteWaiting,    _WriteWaiting);
  ReadVarFromEEPROM( &StartWeight,     _StartWeight);
  ReadVarFromEEPROM( &NeedToWrite,     _NeedToWrite);

cout << "-------------------Print Variabls after\n";
  Serial.println(PneumoStatus);
  Serial.println(AutoLoad);
  Serial.println(MaxWeightStatus);
  Serial.println(MeasureStartTime);
  Serial.println(AutoLoadStatus);
  Serial.println(WriteWaiting);
  Serial.println(StartWeight);
  Serial.println(NeedToWrite);

  while(1);
}
