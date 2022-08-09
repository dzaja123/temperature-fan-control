#include <stdio.h>

void delay(uint32_t us) {
	uint32_t time = 100*us/7;
	while(--time);
}
void intToStr(char** buffer, int* length, int vred){
	int koeff = 10000;
	int prvaCifra = 1;
	while(koeff>=1){
		int cifra = vred/koeff;
		if (prvaCifra == 1 && cifra == 0 && koeff != 1) {
			koeff = koeff/10;
			continue;
		}
		else
			prvaCifra = 0;

		vred -= cifra*koeff;
		**buffer = '0'+cifra;
		(*buffer)++;
		(*length)++;
		koeff = koeff/10;
	}
}
uint8_t kopirajUString(char* buffer, char* str, int* pvrednost){
	int length=0;

	while ((*str)!=0){
		if ((*str) != '%') {
			*buffer++ = *str++;
			length++;
		}
		else{
			str++;
			if (*str == 'd'){
				//intToStr(&buffer, &length, *pvrednost);

				int ivred = *((int*)pvrednost);

				//intToStr(&buffer, &length, *pvrednost);

				int koeff = 10000;
				int prvaCifra = 1;
				while(koeff>=1){

					int cifra = ivred/koeff;

					if (prvaCifra == 1 && cifra == 0 && koeff != 1)	{
						koeff = koeff/10;
						continue;
					}
					else
						prvaCifra = 0;

					ivred -= cifra*koeff;
					*buffer++ = '0'+cifra;
					length++;
					koeff = koeff/10;
				}


			}
			else if (*str == 'b'){
				int binvred = *((int*)pvrednost);
				int koef = 0b1000000000000000U;
				for (int i=0;i<16; i++){
					*buffer++ = (binvred&koef)>0?'1':'0';
					koef = koef >>1;
					length++;
				}
			}/*else if (*str == 'f'){
				float fvred = *((float*)pvrednost);
				int manvred = (int)fvred;
				intToStr(&buffer, &length, fvred);
				*buffer++='.';
				length++;
				int floatvred = (int)((fvred-(float)manvred)*100.0);
				intToStr(&buffer, &length, floatvred);
			}
			*/
			str++;
		}
	}
	return length;
}

int strLength(char* niz){
	int i = 0;
	while (niz[i++]!=0);

	return i;
}


