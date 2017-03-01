#include <stdio.h>
#include <stdlib.h>
#include <string.h>
char* decode(int c,int col)
{
 static char str[256];
 char *po = str;
 *po = 0;
	if (col==1) {
	 switch(c)
	 {
	  case 0x80: strcpy(str,"INCLUDE"); break;
	  case 0x81: strcpy(str,"INCBIN"); break;
	  case 0x82: strcpy(str,"MACRO"); break;
	  case 0x83: strcpy(str,"LOCAL"); break;
	  case 0x84: strcpy(str,"RLCA"); break;
	  case 0x85: strcpy(str,"RRCA"); break;
	  case 0x86: strcpy(str,"HALT"); break;
	  case 0x87: strcpy(str,"CALL"); break;
	  case 0x88: strcpy(str,"PUSH"); break;
	  case 0x89: strcpy(str,"RETN"); break;
	  case 0x8A: strcpy(str,"RETI"); break;
	  case 0x8B: strcpy(str,"DJNZ"); break;
	  case 0x8C: strcpy(str,"OUTI"); break;
	  case 0x8D: strcpy(str,"OUTD"); break;
	  case 0x8E: strcpy(str,"LDIR"); break;
	  case 0x8F: strcpy(str,"CPIR"); break;
	  case 0x90: strcpy(str,"INIR"); break;
	  case 0x91: strcpy(str,"OTIR"); break;
	  case 0x92: strcpy(str,"LDDR"); break;
	  case 0x93: strcpy(str,"CPDR"); break;
	  case 0x94: strcpy(str,"INDR"); break;
	  case 0x95: strcpy(str,"OTDR"); break;
	  case 0x96: strcpy(str,"DD"); break;
	  case 0x97: strcpy(str,"DEFB"); break;
	  case 0x98: strcpy(str,"DEFW"); break;
	  case 0x99: strcpy(str,"DEFS"); break;
	  case 0x9A: strcpy(str,"DISP"); break;
	  case 0x9B: strcpy(str,"ENDM"); break;
	  case 0x9C: strcpy(str,"EDUP"); break;
	  case 0x9D: strcpy(str,"ENDL"); break;
	  case 0x9E: strcpy(str,"MAIN"); break;
	  case 0x9F: strcpy(str,"ELSE"); break;
	  case 0xA0: strcpy(str,"DISPLAY"); break;
	  case 0xA1: strcpy(str,"EXA"); break;
	  case 0xA2: strcpy(str,"DB"); break;
	  case 0xA3: strcpy(str,"DW"); break;
	  case 0xA4: strcpy(str,"DS"); break;
	  case 0xA5: strcpy(str,"NOP"); break;
	  case 0xA6: strcpy(str,"INC"); break;
	  case 0xA7: strcpy(str,"DEC"); break;
	  case 0xA8: strcpy(str,"RLA"); break;
	  case 0xA9: strcpy(str,"RRA"); break;
	  case 0xAA: strcpy(str,"DAA"); break;
	  case 0xAB: strcpy(str,"CPL"); break;
	  case 0xAC: strcpy(str,"SCF"); break;
	  case 0xAD: strcpy(str,"CCF"); break;
	  case 0xAE: strcpy(str,"ADD"); break;
	  case 0xAF: strcpy(str,"ADC"); break;
	  case 0xB0: strcpy(str,"SUB"); break;
	  case 0xB1: strcpy(str,"SBC"); break;
	  case 0xB2: strcpy(str,"AND"); break;
	  case 0xB3: strcpy(str,"XOR"); break;
	  case 0xB4: strcpy(str,"RET"); break;
	  case 0xB5: strcpy(str,"POP"); break;
	  case 0xB6: strcpy(str,"RST"); break;
	  case 0xB7: strcpy(str,"EXX"); break;
	  case 0xB8: strcpy(str,"RLC"); break;
	  case 0xB9: strcpy(str,"RRC"); break;
	  case 0xBA: strcpy(str,"SLA"); break;
	  case 0xBB: strcpy(str,"SRA"); break;
	  case 0xBC: strcpy(str,"SLI"); break;
	  case 0xBD: strcpy(str,"SRL"); break;
	  case 0xBE: strcpy(str,"BIT"); break;
	  case 0xBF: strcpy(str,"RES"); break;
	  case 0xC0: strcpy(str,"SET"); break;
	  case 0xC1: strcpy(str,"OUT"); break;
	  case 0xC2: strcpy(str,"NEG"); break;
	  case 0xC3: strcpy(str,"RRD"); break;
	  case 0xC4: strcpy(str,"RLD"); break;
	  case 0xC5: strcpy(str,"LDI"); break;
	  case 0xC6: strcpy(str,"CPI"); break;
	  case 0xC7: strcpy(str,"INI"); break;
	  case 0xC8: strcpy(str,"LDD"); break;
	  case 0xC9: strcpy(str,"CPD"); break;
	  case 0xCA: strcpy(str,"IND"); break;
	  case 0xCB: strcpy(str,"ORG"); break;
	  case 0xCC: strcpy(str,"EQU"); break;
	  case 0xCD: strcpy(str,"ENT"); break;
	  case 0xCE: strcpy(str,"INF"); break;
	  case 0xCF: strcpy(str,"DUP"); break;
	  case 0xD0: strcpy(str,"IFN"); break;
	  case 0xD1: strcpy(str,"REPEAT"); break;
	  case 0xD2: strcpy(str,"UNTIL0"); break;
	  case 0xD3: strcpy(str,"IF0"); break;
	  case 0xD4: strcpy(str,"LD"); break;
	  case 0xD5: strcpy(str,"JR"); break;
	  case 0xD6: strcpy(str,"JP"); break;
	  case 0xD7: strcpy(str,"OR"); break;
	  case 0xD8: strcpy(str,"CP"); break;
	  case 0xD9: strcpy(str,"EX"); break;
	  case 0xDA: strcpy(str,"DI"); break;
	  case 0xDB: strcpy(str,"EI"); break;
	  case 0xDC: strcpy(str,"IN"); break;
	  case 0xDD: strcpy(str,"RL"); break;
	  case 0xDE: strcpy(str,"RR"); break;
	  case 0xDF: strcpy(str,"IM"); break;
	  case 0xE0: strcpy(str,"ENDIF"); break;
	  case 0xE1: strcpy(str,"EXD"); break;
	  case 0xE2: strcpy(str,"JR	NZ,"); break;
	  case 0xE3: strcpy(str,"JR	Z,"); break;
	  case 0xE4: strcpy(str,"JR	NC,"); break;
	  case 0xE5: strcpy(str,"JR	C,"); break;
	  case 0xE6: strcpy(str,"RUN"); break;
	  case 0xFF: strcpy(str," "); break;
	  default: sprintf(str,"|0x%2.2X|",c);
			   printf("\n\nError %s\n",str);
			   return NULL;
	 }
 } else {
	 switch(c)
	 {
		case 0x9f: strcpy(str,"(BC)"); break;
		case 0xa0: strcpy(str,"(DE)"); break;
		case 0xa1: strcpy(str,"(HL)"); break;
		case 0xa2: strcpy(str,"(SP)"); break;
		case 0xa3: strcpy(str,"(IX)"); break;
		case 0xa4: strcpy(str,"(IY)"); break;
		case 0xd0: strcpy(str,"(C)"); break;
		case 0xd1: strcpy(str,"(IX"); break;
		case 0xd2: strcpy(str,"(IY"); break;
		case 0xd3: strcpy(str,"AF'"); break;
		case 0xe0: strcpy(str,"BC"); break;
		case 0xe1: strcpy(str,"DE"); break;
		case 0xe2: strcpy(str,"HL"); break;
		case 0xe3: strcpy(str,"AF"); break;
		case 0xe4: strcpy(str,"IX"); break;
		case 0xe5: strcpy(str,"IY"); break;
		case 0xe6: strcpy(str,"SP"); break;
		case 0xe7: strcpy(str,"NZ"); break;
		case 0xe8: strcpy(str,"NC"); break;
		case 0xe9: strcpy(str,"PO"); break;
		case 0xea: strcpy(str,"PE"); break;
		case 0xeb: strcpy(str,"HX"); break;
		case 0xec: strcpy(str,"LX"); break;
		case 0xed: strcpy(str,"HY"); break;
		case 0xee: strcpy(str,"LY"); break;
		case 0xef: strcpy(str,"B"); break;
		case 0xf0: strcpy(str,"C"); break;
		case 0xf1: strcpy(str,"D"); break;
		case 0xf2: strcpy(str,"E"); break;
		case 0xf3: strcpy(str,"H"); break;
		case 0xf4: strcpy(str,"L"); break;
		case 0xf5: strcpy(str,"A"); break;
		case 0xf6: strcpy(str,"P"); break;
		case 0xf7: strcpy(str,"M"); break;
		case 0xf8: strcpy(str,"Z"); break;
		case 0xf9: strcpy(str,"R"); break;
		case 0xfa: strcpy(str,"I"); break;		
		case 0xFF: strcpy(str," "); break;
		default: sprintf(str," |0x%2.2X|",c);
			   printf("\n\nError %s\n",str);
			   return NULL;
	 }
 }
 return po;
}
int main(int argc,char **argv)
{
 int i,j,k,c;
 int flen,ftyp,fstart,blk;
 char hob = 0;
 char hhead[17],name[12],ahead[64],str[256],*po=str;
 printf("\nH2ASM v1.1 (ALASM5.X files converter)\n Copyright\t(c) 2002, Alexander Shabarshin http://www.shaos.ru\n");
 printf("\t\t(r) 2013, Dmitry Mikhaltchenkov witchcraft2001@mail.ru\n\n");
 if(argc<3)
 {
    printf("\n\nH2ASM file.$h file.asm\n\n");
    return 1;
 }
 FILE *fh = fopen(argv[1],"rb");
 if(fh==NULL) return 0;
	/*Проверка сигнатуры АЛАСМа в файле*/
 fseek(fh,47,SEEK_SET);
 c = fgetc(fh);
 if (c==0xd9) 
 {
	 /*Not Hobbeta*/
		hob = 0;
		fseek(fh, 0, SEEK_END);
		flen = ftell(fh);
		fseek(fh, 0, SEEK_SET);
 } else {
	 fseek(fh,64,SEEK_SET);
	 c = fgetc(fh);
	 fseek(fh, 0, SEEK_SET);
	 if (c==0xd9) {
		 /*Hobbeta*/
		 printf("Reading HOBBETA-header...\n");
		 fread(hhead,17,1,fh);
		 for(i=0;i<8;i++) name[i]=hhead[i];
		 ftyp=hhead[8];
		 fstart=(int)hhead[9]+((int)hhead[10]<<8);
		 flen=(int)hhead[11]+((int)hhead[12]<<8);
		 blk=hhead[14];
		 int check=hhead[15]+(hhead[16]<<8);
		 for(j=i=0; i<0x0F; ++i)
		 {
		   k = hhead[i]; k &= 0xFF;
		   j += (k * 0x0101 + i);
		 }
		 j &= 0xFFFF;
		 if(j!=check)
		 {
			printf("Illegal checksum in header of hobeta!\n\n");
		 }
	 } else {
		 printf("Error! This is not ALASM file!\n");
		 fclose(fh);
		 return -1;
	 }
 }
 FILE *fa = fopen(argv[2],"wt");
 if(fa==NULL)
 {
    fclose(fh);
    return 0;
 }
 fread(ahead,64,1,fh);
 flen-=64;
 printf("Name:%s\n",ahead);
 printf("\n");
 int comment,colon,old,quat,rustxt;
 while(1)
 {
   comment = 0;
   colon = 0;
   quat = 0;
	rustxt = 0;
   k = fgetc(fh);
   flen--;
   j = 0;
   old = 0;
   for(i=1;i<k;i++)
   {
      c = fgetc(fh);
      flen--;
      if(c<128||comment||quat||rustxt)
      {
         if(c<16)
         {
           while(c-->0) str[j++]=' ';
         }
         else
		 {
			if (c==16) rustxt=1;
			else {
				//if(!comment && !quat && colon==1 && old>=128) str[j++]=' ';
				str[j++]=c;
			}
         }
         if(c==';') comment=1;
         if(c=='"')
         {
           if(quat) quat=0;
           else     quat=1;
         }
      }
      else
	  {
         po = decode(c,++colon);
         if(po==NULL) break;
         str[j] = 0;
         if(colon==1)
         {
            strcat(po,"\t");
				strcat(str,"\t"); 
				j++;
         }
         strcat(str,po);
         j += strlen(po);
      }
      old = c;
   }
    if (quat==1) {
	 str[j]='"';
	 j++;
	 str[j] = 0;
	}
   if(po==NULL) break;
   str[j] = 0;
   fprintf(fa,"%s\n",str);
   if(flen<=0) break;
 }
 fclose(fh);
 fclose(fa);
 printf("OK!\n");
 return 1;
}