ISR(TIMER1_COMPA_vect) // 1 second intervals
{
	/***CLEAR EEPROM OFFSET SEQUENCE START***/
	if((F.ll(&F) & IMASK) == (ONE << 3)) //button 4
	counter_1++;
	else if(counter_1 < _5sec+ONE)
	counter_1=ZERO;
	if(counter_1 > _5sec){
		counter_1 = _5sec+ONE; //lock in place
		PORTC ^= (ONE << 6); // troubleshooting
		count--;
		if(!count){ //led blinks x times
			// Delete eeprom memory ZERO
			eprom.update_block(HX711_Default, (void*) ZERO, sizeblock);
			hx.get_cal(&hx)->offset_32 = HX711_Default->offset_32;
			hx.get_cal(&hx)->offset_64 = HX711_Default->offset_64;
			hx.get_cal(&hx)->offset_128 = HX711_Default->offset_128;
			hx.get_cal(&hx)->divfactor_32 = divfactor = HX711_Default->divfactor_32;
			hx.get_cal(&hx)->divfactor_64 = divfactor = HX711_Default->divfactor_64;
			hx.get_cal(&hx)->divfactor_128 = HX711_Default->divfactor_128;
			hx.get_cal(&hx)->status = HX711_Default->status;
			PORTC |= (ONE << 5); // troubleshooting
			PORTC |= (ONE << 6); // troubleshooting
			counter_1 = ZERO;
			count=blink;
		}
	}
	/***CLEAR EEPROM OFFSET SEQUENCE END***/
	/***CAL DIVFACTOR DEFINE START***/
	if((F.ll(&F) & IMASK) == (ONE << 4)) //button 5
	counter_2++;
	else if(counter_2 < _5sec+ONE)
	counter_2=ZERO; //RESET TIMER
	if(counter_2 > _5sec){
		counter_2 = ZERO; //RESET TIMER
		signal = ONE;
		PORTC &= ~(ONE << 7); // troubleshooting
	}
	/***CAL DIVFACTOR DEFINE END***/
}