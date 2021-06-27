while(TRUE){
	/******PREAMBLE******/
	lcd0.reboot(); //Reboot LCD
	F.boot(&F,PINF); //PORTF INPUT READING
	while(hx.query(&hx)); 
	//Catches falling Edge instance, begins bit shifting.
	..........
	..........