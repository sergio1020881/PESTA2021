/*************************************************************************
	FUNCTION
Author: Sergio Santos
	<sergio.salazar.santos@gmail.com> 
License: GNU General Public License
Hardware: all
Date: 25102020
Comment:
    Always try to make general purpose bullet proof functions !!
    Very Stable
*************************************************************************/
/***Library***/
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdarg.h>
#include <inttypes.h>
#include <math.h>
/***pc use***
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
***/
#include"function.h"
/***Constant & Macro***/
#ifndef GLOBAL_INTERRUPT_ENABLE
	#define GLOBAL_INTERRUPT_ENABLE 7
#endif
#define ZERO 0
#define ONE 1
#define FUNCSTRSIZE 32
/***Global File Variable***/
char FUNCstr[FUNCSTRSIZE+ONE];
/***Header***/
unsigned int Pwr(uint8_t bs, uint8_t n);
int StringLength (const char string[]);
void Reverse(char s[]);
uint8_t  bintobcd(uint8_t bin);
/******/
unsigned int FUNCmayia(unsigned int xi, unsigned int xf, uint8_t nbits);
uint8_t FUNCpinmatch(uint8_t match, uint8_t pin, uint8_t HL);
uint8_t FUNChmerge(uint8_t X, uint8_t Y);
uint8_t FUNClmerge(uint8_t X, uint8_t Y);
uint8_t FUNChh(uint8_t xi, uint8_t xf);
uint8_t FUNCll(uint8_t xi, uint8_t xf);
uint8_t FUNClh(uint8_t xi, uint8_t xf);
uint8_t FUNChl(uint8_t xi, uint8_t xf);
uint8_t FUNCdiff(uint8_t xi, uint8_t xf);
void FUNCswap(long *px, long *py);
void FUNCcopy(char to[], char from[]);
void FUNCsqueeze(char s[], int c);
void FUNCshellsort(int v[], int n);
char* FUNCi16toa(int16_t n);
char* FUNCui16toa(uint16_t n);
char* FUNCi32toa(int32_t n);
int FUNCtrim(char s[]);
int FUNCpmax(int a1, int a2);
int FUNCgcd (int u, int v);
int FUNCstrToInt (const char string[]);
uint8_t FUNCfilter(uint8_t mask, uint8_t data);
unsigned int FUNCticks(unsigned int num);
int FUNCtwocomptoint8bit(int twoscomp);
int FUNCtwocomptoint10bit(int twoscomp);
int FUNCtwocomptointnbit(int twoscomp, uint8_t nbits);
char FUNCdec2bcd(char num);
char FUNCbcd2dec(char num);
char* FUNCresizestr(char *string, int size);
long FUNCtrimmer(long x, long in_min, long in_max, long out_min, long out_max);
unsigned char FUNCbcd2bin(unsigned char val);
unsigned char FUNCbin2bcd(unsigned val);
long FUNCgcd1(long a, long b);
uint8_t FUNCpincheck(uint8_t port, uint8_t pin);
char* FUNCprint_binary(uint8_t number);
void FUNCreverse(char* str, int len);
uint8_t FUNCintinvstr(int32_t n, char* res, uint8_t n_digit);
char* FUNCftoa(float n, char* res, uint8_t afterpoint);
/***pc use***
char* FUNCfltos(FILE* stream);
char* FUNCftos(FILE* stream);
int FUNCstrtotok(char* line,char* token[],const char* parser);
char* FUNCputstr(char* str);
int FUNCgetnum(char* x);
unsigned int FUNCgetnumv2(char* x);
int FUNCreadint(int nmin, int nmax);
***/
// uint8_t TRANupdate(struct TRAN *tr, uint8_t idata);
/***Procedure & Function***/
FUNC FUNCenable( void )
{
	uint8_t tSREG;
	tSREG=SREG;
	SREG&=~(1<<GLOBAL_INTERRUPT_ENABLE);
	// struct object
	FUNC func;
	// function pointers
	func.power=Pwr;
	func.stringlength=StringLength;
	func.reverse=Reverse;
	func.mayia=FUNCmayia;
	func.pinmatch=FUNCpinmatch;
	func.hmerge=FUNChmerge;
	func.lmerge=FUNClmerge;
	func.hh=FUNChh;
	func.ll=FUNCll;
	func.lh=FUNClh;
	func.hl=FUNChl;
	func.diff=FUNCdiff;
	func.swap=FUNCswap;
	func.copy=FUNCcopy;
	func.squeeze=FUNCsqueeze;
	func.shellsort=FUNCshellsort;
	func.i16toa=FUNCi16toa;
	func.ui16toa=FUNCui16toa;
	func.i32toa=FUNCi32toa;
	func.trim=FUNCtrim;
	func.pmax=FUNCpmax;
	func.gcd=FUNCgcd;
	func.strToInt=FUNCstrToInt;
	func.filter=FUNCfilter;
	func.ticks=FUNCticks;
	func.twocomptoint8bit=FUNCtwocomptoint8bit;
	func.twocomptoint10bit=FUNCtwocomptoint10bit;
	func.twocomptointnbit=FUNCtwocomptointnbit;
	func.dec2bcd=FUNCdec2bcd;
	func.bcd2dec=FUNCbcd2dec;
	func.resizestr=FUNCresizestr;
	func.trimmer=FUNCtrimmer;
	func.bcd2bin=FUNCbcd2bin;
	func.bin2bcd=FUNCbin2bcd;
	func.gcd1=FUNCgcd1;
	func.pincheck=FUNCpincheck;
	func.print_binary=FUNCprint_binary;
	func.ftoa=FUNCftoa;
	/***pc use***
	func.fltos=FUNCfltos;
	func.ftos=FUNCftos;
	func.strtotok=FUNCstrtotok;
	func.putstr=FUNCputstr;
	func.getnum=FUNCgetnum;
	func.getnumv2=FUNCgetnumv2;
	func.readint=FUNCreadint;
	*/
	SREG=tSREG;
	/******/
	return func;
}
// mayia
unsigned int FUNCmayia(unsigned int xi, unsigned int xf, uint8_t nbits)
{//magic formula
	unsigned int mask;
	unsigned int diff;
	unsigned int trans;
	mask=Pwr(2,nbits)-1;
	xi&=mask;
	xf&=mask;
	diff=xf^xi;
	trans=diff&xf;
	return (trans<<nbits)|diff;
}
//pinmatch
uint8_t FUNCpinmatch(uint8_t match, uint8_t pin, uint8_t HL)
{
	uint8_t result;
	result=match&pin;
	if(HL){
		if(result==match);
		else
			result=0;
	}else{
		if(result)
			result=0;
		else
			result=match;
	}
	return result;
}
// hmerge
uint8_t FUNChmerge(uint8_t X, uint8_t Y)
{
	return (X | Y);
}
// lmerge
uint8_t FUNClmerge(uint8_t X, uint8_t Y)
{
	return (X & Y);
}
// hh
uint8_t FUNChh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi&xf;
	return i;
}
// ll
uint8_t FUNCll(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi|xf;
	return ~i;
}
// lh
uint8_t FUNClh(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xi^xf;
	i&=xf;
	return i;
}
// hl
uint8_t FUNChl(uint8_t xi, uint8_t xf)
{
	uint8_t i;
	i=xf^xi;
	i&=xi;
	return i;
}
// diff
uint8_t FUNCdiff(uint8_t xi, uint8_t xf)
{
	return xf^xi;
}
// interchange *px and *py
void FUNCswap(long *px, long *py)
{
	long temp;
	temp = *px;
	*px = *py;
	*py = temp;
}
// copy: copy 'from' into 'to'; assume to is big enough
void FUNCcopy(char to[], char from[])
{
	int i;
	i = 0;
	while ((to[i] = from[i]) != '\0')
		++i;
}
// squeeze: delete all c from s
void FUNCsqueeze(char s[], int c)
{
	int i, j;
	for (i = j = 0; s[i] != '\0'; i++)
		if (s[i] != c)
			s[j++] = s[i];
		s[j] = '\0';
}
// shellsort: sort v[0]...v[n-1] into increasing order
void FUNCshellsort(int v[], int n)
{
	int gap, i, j, temp;
	for (gap = n/2; gap > 0; gap /= 2)
		for (i = gap; i < n; i++)
			for (j=i-gap; j>=0 && v[j]>v[j+gap]; j-=gap){
				temp = v[j];
				v[j] = v[j+gap];
				v[j+gap] = temp;
			}
}
// i32toa: convert n to characters in s
char* FUNCi32toa(int32_t n)
{
	uint8_t i;
	int32_t sign;
	if ((sign = n) < 0) // record sign
	n = -n; // make n positive
	i = 0;
	do { // generate digits in reverse order
		FUNCstr[i++] = n % 10 + '0'; // get next digit
	}while ((n /= 10) > 0); // delete it
	if (sign < 0)
		FUNCstr[i++] = '-';
	FUNCstr[i] = '\0';
	Reverse(FUNCstr);
	return FUNCstr;
}
// i16toa: convert n to characters in s
char* FUNCi16toa(int16_t n)
{
	uint8_t i;
	int16_t sign;
	if ((sign = n) < 0) // record sign
		n = -n; // make n positive
	i = 0;
	do { // generate digits in reverse order
		FUNCstr[i++] = n % 10 + '0'; // get next digit
	}while ((n /= 10) > 0); // delete it
	if (sign < 0)
		FUNCstr[i++] = '-';
	FUNCstr[i] = '\0';
	Reverse(FUNCstr);
	return FUNCstr;
}
// ui16toa: convert n to characters in s
char* FUNCui16toa(uint16_t n)
{
	uint8_t i;
	i = 0;
	do { // generate digits in reverse order
		FUNCstr[i++] = n % 10 + '0'; // get next digit
	}while ((n /= 10) > 0); // delete it
	FUNCstr[i] = '\0';
	Reverse(FUNCstr);
	return FUNCstr;
}
// trim: remove trailing blanks, tabs, newlines
int FUNCtrim(char s[])
{
	int n;
	for (n = StringLength(s)-1; n >= 0; n--)
		if (s[n] != ' ' && s[n] != '\t' && s[n] != '\n')
			break;
	s[n+1] = '\0';
	return n;
}
// larger number of two
int FUNCpmax(int a1, int a2)
{
	int biggest;
	if(a1 > a2){
		biggest = a1;
	}else{
		biggest = a2;
	}
	return biggest;
}
// common divisor
int FUNCgcd (int u, int v)
{
	int temp;
	while ( v != 0 ) {
		temp = u % v;
		u = v;
		v = temp;
	}
	return u;
}
// Function to convert a string to an integer
int FUNCstrToInt (const char string[])
{
	int i, intValue, result = 0;
	for ( i = 0; string[i] >= '0' && string[i] <= '9'; ++i ){
		intValue = string[i] - '0';
		result = result * 10 + intValue;
	}
	return result;
}
// filter
uint8_t FUNCfilter(uint8_t mask, uint8_t data)
{
	uint8_t Z;
	Z=mask & data;
	return Z;
}
// ticks
unsigned int FUNCticks(unsigned int num)
{
	unsigned int count;
	for(count=0;count<num;count++)
		;
	return count;
}
// Two's Complement function
int FUNCtwocomptoint8bit(int twoscomp){
  
  //Let's see if the byte is negative
  if (twoscomp & 0B10000000){
    //Invert
    twoscomp = ~twoscomp + 1;
	twoscomp = (twoscomp & 0B11111111);
    //Cast as int and multiply by negative one
    int value = (int)(twoscomp)*(-1);
    return value;
  }
  else{
    //Byte is non-negative, therefore convert to decimal and display
    //Make sure we are never over 1279
    twoscomp = (twoscomp & 0B01111111);
    //Cast as int and return
    int value = (int)(twoscomp);
    return value;
  }
}
// Two's Complement function, shifts 10 bit binary to signed integers (-512 to 512)
int FUNCtwocomptoint10bit(int twoscomp){
	
  //Let's see if the byte is negative
  if (twoscomp & 0x200){
    //Invert
    twoscomp = ~twoscomp + 1;
    twoscomp = (twoscomp & 0x3FF);
    //Cast as int and multiply by negative one
    int value = (int)(twoscomp)*(-1);
    return value;
  }
  else{
    //Serial.println("We entered the positive loop");
    //Byte is non-negative, therefore convert to decimal and display
    twoscomp = (twoscomp & 0x1FF);
    //Cast as int and return
    //Serial.println(twoscomp);
    int value = (int)(twoscomp);
    return value;
  }
}
// Two's Complement function, nbits
int FUNCtwocomptointnbit(int twoscomp, uint8_t nbits){
  unsigned int signmask;
  unsigned int mask;
  signmask = (1<<(nbits-1));
  mask=signmask-1;
  //Let's see if the number is negative
  if (twoscomp & signmask){
	twoscomp &= mask;
    twoscomp -= signmask;
  }else{
	  twoscomp &= mask;
  }
  return twoscomp;
}
// Convert Decimal to Binary Coded Decimal (BCD)
char FUNCdec2bcd(char num)
{
	return ((num/10 * 16) + (num % 10));
}
// Convert Binary Coded Decimal (BCD) to Decimal
char FUNCbcd2dec(char num)
{
	return ((num/16 * 10) + (num % 16));
}
char* FUNCresizestr(char *string, int size)
{
	int i;
	FUNCstr[size]='\0';
	for(i=0;i<size;i++){
		if(*(string+i)=='\0'){
			for(;i<size;i++){
				FUNCstr[i]=' ';
			}
			break;
		}
		FUNCstr[i]=*(string+i);
	}
	return FUNCstr;
}
long FUNCtrimmer(long x, long in_min, long in_max, long out_min, long out_max)
/***
same as arduino map function.
***/
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
// power: raise base to n-th power; n >= 0
unsigned int Pwr(uint8_t bs, uint8_t n)
{
    unsigned int i, p;
    p = 1;
    for (i = 1; i <= n; ++i)
        p = p * bs;
    return p;
}
// Function to count the number of characters in a string
int StringLength (const char string[])
{
	int count = 0;
	while ( string[count] != '\0' )
		++count;
	return count;
}
// reverse: reverse string s in place
void Reverse(char s[])
{
	int c, i, j;
	for (i = 0, j = StringLength(s)-1; i < j; i++, j--){
		c = s[i];
		s[i] = s[j];
		s[j] = c;
	}
}
unsigned char FUNCbcd2bin(unsigned char val)
{
	return (val & 0x0f) + (val >> 4) * 10;
}
unsigned char FUNCbin2bcd(unsigned val)
{
	return ((val / 10) << 4) + val % 10;
}
long FUNCgcd1(long a, long b)
{
	long r;
	if (a < b)
		FUNCswap(&a, &b);
	if (!b){
		while ((r = a % b) != 0) {
			a = b;
			b = r;
		}
	}	
	return b;
}
uint8_t FUNCpincheck(uint8_t port, uint8_t pin)
{
	uint8_t lh;
	if(port & (1<<pin))
		lh=1;
	else
		lh=0;
	return lh;
}
char* FUNCprint_binary(uint8_t number)
{
	uint8_t i,c;
    for(i=128,c=0;i;i>>=1,c++){
	(number & i) ? (FUNCstr[c]='1') : (FUNCstr[c]='0');
	}
	FUNCstr[c]='\0';
	return FUNCstr;
}
uint8_t leap_year_check(uint16_t year){
	uint8_t i;
	if ( !(year%400))
    	i=1;
  	else if ( !(year%100))
    	i=0;
  	else if ( !(year%4) )
    	i=1;
  	else
    	i=0;
	return i;
}
uint8_t  bintobcd(uint8_t bin)
{
	return (((bin) / 10) << 4) + ((bin) % 10);
}
/***intinvstr***/
uint8_t FUNCintinvstr(int32_t n, char* res, uint8_t n_digit)
{
	uint8_t k=0;
	for(res[k++] = (n % 10) + '0' ; (n/=10) > ZERO ; res[k++] = (n % 10) + '0');
	for( ; k < n_digit ; res[k++] = '0');
	res[k]='\0';
	return k;
}
/***ftoa***/
char* FUNCftoa(float n, char* res, uint8_t afterpoint)
{
	uint8_t k=ZERO;
	int32_t ipart;
	float fpart;
	int8_t sign;
	if (n < ZERO){
		n = -n;
		sign=-ONE;
	}else
		sign=ONE;
	ipart = (int32_t) n;
	fpart = n - (float)ipart;
	k=FUNCintinvstr( ipart, res, ONE );
	if (sign < ZERO)
		res[k++] = '-';
	else
		res[k++] = ' ';
	res[k]='\0';
	Reverse(res);
	if (afterpoint > ZERO) {
		res[k++] = '.';
		FUNCintinvstr( fpart * pow(10, afterpoint), res+k, afterpoint );
		Reverse(res+k);
	}	
	return res;
}
/******
int gcd( int a, int b ) {
    int result ;
    // Compute Greatest Common Divisor using Euclid's Algorithm
    __asm__ __volatile__ ( "movl %1, %%eax;"
                          "movl %2, %%ebx;"
                          "CONTD: cmpl $0, %%ebx;"
                          "je DONE;"
                          "xorl %%edx, %%edx;"
                          "idivl %%ebx;"
                          "movl %%ebx, %%eax;"
                          "movl %%edx, %%ebx;"
                          "jmp CONTD;"
                          "DONE: movl %%eax, %0;" : "=g" (result) : "g" (a), "g" (b)
    );
    return result ;
}
//
float sinx( float degree ) {
    float result, two_right_angles = 180.0f ;
    // Convert angle from degrees to radians and then calculate sin value
    __asm__ __volatile__ ( "fld %1;"
                            "fld %2;"
                            "fldpi;"
                            "fmul;"
                            "fdiv;"
                            "fsin;"
                            "fstp %0;" : "=g" (result) : 
				"g"(two_right_angles), "g" (degree)
    ) ;
    return result ;
}
//
float cosx( float degree ) {
    float result, two_right_angles = 180.0f, radians ;
    // Convert angle from degrees to radians and then calculate cos value
    __asm__ __volatile__ ( "fld %1;"
                            "fld %2;"
                            "fldpi;"
                            "fmul;"
                            "fdiv;"
                            "fstp %0;" : "=g" (radians) : 
				"g"(two_right_angles), "g" (degree)
    ) ;
    __asm__ __volatile__ ( "fld %1;"
                            "fcos;"
                            "fstp %0;" : "=g" (result) : "g" (radians)
    ) ;
    return result ;
}
//
float square_root( float val ) {
    float result ;
    __asm__ __volatile__ ( "fld %1;"
                            "fsqrt;"
                            "fstp %0;" : "=g" (result) : "g" (val)
    ) ;
    return result ;
}
*/
/***pc use***
char* FUNCfltos(FILE* stream)
{
	int i, block, NBytes;
	char caracter;
	char* value=NULL;
	for(i=0, block=4, NBytes=0; (caracter=getc(stream)) != EOF; i++){
		if(i==NBytes){
			NBytes+=block;
			value=(char*)realloc(value, NBytes*sizeof(char));
			if(value==NULL){
				perror("fltos at calloc");
				break;
			}
		}
		*(value+i)=caracter;
		if(caracter=='\n'){
			*(value+i)='\0';
			break;
		}
	}
	return value;
}
char* FUNCftos(FILE* stream)
{
	int i, block, NBytes;
	char caracter;
	char* value=NULL;
	for(i=0, block=8, NBytes=0; (caracter=getc(stream)) != EOF; i++){
		if(i==NBytes){
			NBytes+=block;
			value=(char*)realloc(value, NBytes*sizeof(char));
			if(value==NULL){
				perror("ftos at calloc");
				break;
			}
		}
		*(value+i)=caracter;
	}
	return value;
}
int FUNCstrtotok(char* line,char* token[],const char* parser)
{
	int i;
	char *str;
	str=(char*)calloc(strlen(line),sizeof(char));
	for (i = 0, strcpy(str,line); ; i++, str = NULL) {
		token[i] = strtok(str, parser);
		if (token[i] == NULL)
			break;
		printf("%d: %s\n", i, token[i]);
	}
	free(str);
	return i;
}
char* FUNCputstr(char* str)
{
	int i; char* ptr;
	ptr = (char*)calloc(strlen(str), sizeof(char));
	if(ptr == NULL){
		perror("NULL!\n");
		return NULL;
	}
	for(i=0; (ptr[i] = str[i]); i++){
		if(ptr[i] == '\0')
			break;
	}
	return (ptr);
}
int FUNCgetnum(char* x)
{
	int num;
	if(!sscanf(x, "%d", &num))
		num=0;
	return num;
}
unsigned int FUNCgetnumv2(char* x)
{
	unsigned int RETURN;
	unsigned int value;
	unsigned int n;
	errno=0;
	n=sscanf(x,"%d",&value);
	if(n==1){
		RETURN=value;
	}else if(errno != 0){
		perror("getnum at sscanf");
		RETURN=0;
	}else{
		RETURN=0;
	}
	return RETURN;
}
int FUNCreadint(int nmin, int nmax)
{
	int num;
	int flag;
	for(flag=1; flag;){
		for( num=0; !scanf("%d",&num);getchar())
			;
		//printf("num: %d nmin: %d nmax: %d\n",num, nmin, nmax);
		if((num < nmin) || (num > nmax))
			continue;
		flag=0;
	}
		return num;
}
***/
/***Interrupt***/
/***Comment
***/
/***EOF***/
