ISR(TIMER0_COMP_vect) // 15.4 us intervals
{
	/***Block other interrupts during this procedure***/
	uint8_t Sreg;
	Sreg = STATUS_REGISTER;
	STATUS_REGISTER &= ~(ONE << GLOBAL_INTERRUPT_ENABLE);
	hx.read_raw(&hx);
	/***enable interrupts again***/
	STATUS_REGISTER = Sreg;
}