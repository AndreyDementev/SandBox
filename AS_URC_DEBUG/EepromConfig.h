#ifndef _EEPROMCONFIG_H_
#define _EEPROMCONFIG_H_


//----------первых 100 байт оставляем под служебную информацию-----------
#define EEPROM_PD_ADDRESS     0     

//--------Сохраниение информации на случай аварийной перезагрузки--------
//-----------------100-119 адреса переменных состояния-------------------
#define _PneumoStatus       100     //bool
#define _AutoLoad           101     //bool
#define _MaxWeightStatus    102     //bool
#define _AutoLoadStatus     103     //bool
#define _WriteWaiting       104     //bool
#define _NeedToWrite        105   

//----------------120-199 разная необходимая информация--------------------
#define _MeasureStartTime   120     //long
#define _StartWeight        124     //long



#endif //_EEPROMCONFIG_H_
