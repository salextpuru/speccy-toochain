#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

int readfile(const string fname,unsigned char* buf,int fmax=0xa0000) {
	FILE *infile=fopen(fname.c_str(),"rb");
	if (!infile) return -1;		// failed to open file
	fseek(infile,0,SEEK_END);
	int iflen = ftell(infile);
	rewind(infile);
	if (iflen < fmax+1) {
		rewind(infile);
		fread((char*)buf,1,iflen,infile);
	} else {
		iflen=0;
	}
	fclose(infile);
	return iflen;
}

bool savefile(const string fname, unsigned char* buf, int len) {
	FILE *file=fopen(fname.c_str(),"wb");
	if (!file) return false;
	fwrite((char*)buf,1,len,file);
	fclose(file);
	return true;
}

unsigned char testsig(unsigned char* buf, int len) {
	if (strncmp("SINCLAIR",(char*)buf,8)==0 && *(buf+8)>=0) return 1;			// SCL
	if (len==0xA0000 && *(buf+0x8e7)==0x10) return 2;				// TRD
	return 0;									// unknown
}

void extractfile(unsigned char* buf, unsigned char* ptr,int fpos,const string fname) {
	int len=*(ptr+11)+(*(ptr+12)<<8);
	unsigned char slen=*(ptr+13);
	unsigned char rlen=*(ptr+12); if (*(ptr+11)!=0) rlen++;
	if (slen!=rlen) len=(slen<<8);
	printf("%.8s\n",ptr);
	FILE *ofile=fopen(fname.c_str(),"wb");
	if (!ofile) {cout<<"Can't write file"<<endl; return;}
	fwrite(buf+fpos,1,len,ofile);
	fclose(ofile);
}

void makedsc(unsigned char* ptr, const string fname, const string fext, int len,unsigned char slen, unsigned short start=0) {
	strncpy((char*)ptr,fname.c_str(),8);
	*(ptr+8)=fext[0];
	// start
	*(ptr+9)=(start & 0xFF);
	*(ptr+10)=(start >> 8) & 0xFF;
	// len
	*(ptr+11)=(len&0xff);
	*(ptr+12)=((len&0xff00)>>8);
	// slen
	*(ptr+13)=slen;
}

void pack(const string fextra, const string aname) {
	//
	bool hobeta=false;	//
	unsigned char hhob[17]; // заголовок HOBETA
	string fname,fext;
	unsigned short start=0;
	//
	size_t dotpos = fextra.rfind('.');		// get ZX filename+ext (8+1)
	// HOBETA или нет?
	if(fextra[dotpos+1]=='$'){
		hobeta = true;
	}
	//
	if (dotpos==string::npos) {fname = fextra.substr(0,8); fext = "C";}
	else {fname = fextra.substr(0,min((int)dotpos,8));
		if (dotpos>=fextra.size()-1) fext=" "; else fext=fextra.substr(dotpos+1,1);
	}
	// Имя файла
	fname.resize(8,0x20);
	unsigned char *inbuf;
	int iflen;
	if(hobeta){
		inbuf = new unsigned char[0xff11]; // +17 байт
		iflen = readfile(fextra,inbuf,0xff11);
	}
	else{
		inbuf = new unsigned char[0xff00];
		iflen = readfile(fextra,inbuf,0xff00);
	}
	if (iflen==-1) {cout<<"Can't read file"<<endl; return;}
	if (iflen==0) {cout<<"Input file is too long"<<endl; return;}
	// Для HOBETA 
	if(hobeta){
		// Переносим заголовок отдельно
		memcpy(hhob,inbuf,17);
		// В inbuf - только данные
		memmove(inbuf,inbuf+17,iflen-17);
		iflen=iflen-17;
		// Имя
		memcpy( (void*)(fname.c_str()), hhob, 8);
		// Расширение
		fext = hhob[8];
	}
	// Читаем образину
	unsigned char *obuf = new unsigned char[0xa0000];
	int olen=readfile(aname,obuf);
	if (olen<1) {cout<<"Can't read output file"<<endl; return;}
	//
	int mode=testsig(obuf,olen);
	unsigned char seclen;
	if(hobeta){
		cout << "HOBETA" << endl;
		seclen = hhob[13]; // Размер в секторах из hobeta
		start = hhob[9] + (hhob[10]<<8);
	}
	else{
		cout << "NON HOBETA" << endl;
		seclen = ((iflen & 0xff00)>>8); if ((iflen & 0xff) != 0) seclen++;	// sectors len
	}
	
	cout<<"file len: "<<(int)iflen << " " << (int)seclen << endl;
	
	unsigned char lastsec,lasttrk,files;
	unsigned int freesec,fpos,secnum;
	unsigned char* ptr = (unsigned char*)obuf;
	FILE *ofile;
	switch (mode) {
		case 1:
			files=*(obuf+8); if (files>127) {cout<<"Too many files in image"<<endl; break;}
			(*(obuf+8))++;
			ofile = fopen(aname.c_str(),"wb");
			if (!ofile) {cout<<"Can't write to file"<<endl; break;}
			freesec=9+14*files;				// old catalog len
			fwrite(obuf,1,freesec,ofile);			// save old catalog
			ptr = new unsigned char[14];
			makedsc(ptr,fname,fext,iflen,seclen,start);		// make 14bytes-len descriptor
			fwrite(ptr,1,14,ofile);				// write it
			fwrite(obuf+freesec,1,olen-freesec,ofile);	// write old data
			fwrite(inbuf,1,iflen,ofile);			// write new data
			if ((iflen&0xff)!=0) fwrite(inbuf,1,0x100-(iflen&0xff),ofile);	// write to end of sector
			fclose(ofile);
			break;
		case 2:
			files = *(obuf+0x8e4);
			if (files > 127) {
				cout << "Too many files in image" << endl;
				break;
			}
			freesec = *(obuf+0x8e5) + (*(obuf+0x8e6) << 8);
			if (freesec < seclen) {
				cout << "No room for file" << endl;
				break;
			}
	cout << "freesec = " << (int)freesec << endl;
			lastsec = *(obuf+0x8e1);
			lasttrk = *(obuf+0x8e2);
			files++;
			*(obuf+0x8e4) = files;
			freesec -= seclen;
			*(obuf+0x8e5) = freesec & 0xff;
			*(obuf+0x8e6) = ((freesec & 0xff00) >> 8);
			while (*ptr != 0) ptr += 16;
			makedsc(ptr,fname,fext,iflen,seclen,start);
			*(ptr+14) = lastsec;
			*(ptr+15) = lasttrk;
			secnum = ((lasttrk << 4) + lastsec);	// free sector abs num
			fpos = (secnum << 8); 			// (lasttrk<<12)+(lastsec<<8);
			secnum += seclen;
			lastsec = secnum & 15;
			lasttrk = ((secnum & 0xfff0) >> 4);
//			lastsec += (seclen & 15);
//			lasttrk+=((seclen&0xf0)>>4);
//			if (lastsec>15) {lastsec-=16; lasttrk++;}
			*(obuf+0x8e1) = lastsec;
			*(obuf+0x8e2) = lasttrk;
			memcpy(obuf+fpos,inbuf,iflen);
//	throw(0);
			savefile(aname,obuf,0xa0000);
			break;
		default: cout<<"Unknown image format"<<endl; break;
	}
}

void extract(const string fextra, const string aname) {
	if (fextra.size()>10) {cout<<"Filename is too long ('filename.C' is maximum)"<<endl; return;}
	if (fextra.size()<2) {cout<<"Filename is too short (must be '.C' at least)"<<endl; return;}
	if (fextra.at(fextra.size()-2)!='.') {cout<<"Filename must be in 'fileneme.e' format"<<endl; return;}
	string fname(fextra,0,fextra.size()-2); fname.resize(8,0x20);	// filename (8 chars)
	string fext(fextra,fextra.size()-1,1);				// extension (1 char)
	unsigned char *buf = new unsigned char[0xa0000];
	int len=readfile(aname,buf);
	if (len<1) {cout<<"Can't read file"<<endl; return;}
	int mode=testsig(buf,len);
	unsigned char* ptr = buf;
	unsigned char i=0;
	int fpos;
	switch (mode) {
		case 1:
			ptr+=9; fpos=9+*(buf+8)*14;		// begin of data
			for(;i<*(buf+8);i++) {
				if (strncmp(fname.c_str(),(const char*)ptr,8)==0) {
					extractfile(buf,ptr,fpos,fextra); i=128;
				}
				fpos+=(*(ptr+13)<<8); ptr+=14;
			}
			break;
		case 2:
			while(*ptr!=0 && i<128) {
				if (strncmp(fname.c_str(),(const char*)ptr,8)==0) {
					int fpos=(*(ptr+15)<<12)+(*(ptr+14)<<8);	// position of begin of file in buf
					extractfile(buf,ptr,fpos,fextra); i=128;
				}
				ptr+=16; i++;
			}
			break;
		default: cout<<"Unknown image format"<<endl; break;
	}
}

void list(const string fname) {
	unsigned char *buf = new unsigned char[0xa0000];
	int len=readfile(fname,buf);
	if (len<1) {cout<<"Can't read file"<<endl; return;}
	int mode=testsig(buf,len);
	unsigned char* ptr = buf;
	unsigned char i=0;
	switch (mode) {
		case 1:
			ptr+=9;
			printf("Name\t\tExt\tStart\tSize\tSLen\n---------------------------\n");
			for(;i<*(buf+8);i++) {
				printf("%.8s\t%.1s\t%i\t%i\t%i\n",ptr,ptr+8,*(ptr+9)+(*(ptr+10)<<8),*(ptr+11)+(*(ptr+12)<<8),*(ptr+13));
				ptr+=14;
			}
			break;
		case 2:
			printf("Name\t\tExt\tStart\tSize\tSLen\tTrk\tSec\n---------------------------\n");
			while (*ptr!=0 && i<128) {
				if (*ptr!=1) printf("%.8s\t%.1s\t%i\t%i\t%i\t%i\t%i\n",ptr,ptr+8,*(ptr+9)+(*(ptr+10)<<8),*(ptr+11)+(*(ptr+12)<<8),*(ptr+13),*(ptr+14),*(ptr+15));
				ptr+=16; i++;
			}
			break;
		default: cout<<"Unknown image format"<<endl; break;
	}
}

void nlist(const string fname) {
	unsigned char *buf = new unsigned char[0xa0000];
	int len=readfile(fname,buf);
	if (len<1) {cout<<"Can't read file"<<endl; return;}
	int mode=testsig(buf,len);
	unsigned char* ptr = buf;
	unsigned char i=0;
	switch (mode) {
		case 1:
			ptr+=9;
			for(;i<*(buf+8);i++) {
				printf("%.8s.%.1s\n",ptr,ptr+8);
				ptr+=14;
			}
			break;
		case 2:
			while (*ptr!=0 && i<128) {
				if (*ptr!=1) printf("%.8s.%.1s\n",ptr,ptr+8);
				ptr+=16; i++;
			}
			break;
		default: cout<<"Unknown image format"<<endl; break;
	}
}

void createtrd(const string fname) {
	unsigned char *buf = new unsigned char[0xa0000];
	*(buf+0x8e2)=0x01;
	*(buf+0x8e3)=0x16;
	*(buf+0x8e5)=0xf0;
	*(buf+0x8e6)=0x09;
	*(buf+0x8e7)=0x10;
	FILE *ofile = fopen(fname.c_str(),"wb");
	if (!ofile) {cout<<"Can't write to file"<<endl; return;}
	fwrite((char*)buf,1,0xa0000,ofile);
	fclose(ofile);
}

void createscl(const string fname) {
	char *buf = new char;
	strncpy(buf,"SINCLAIR\0",9);
	FILE *ofile = fopen(fname.c_str(),"wb");
	if (!ofile) {cout<<"Can't write to file"<<endl; return;}
	fwrite(buf,1,9,ofile);
	fclose(ofile);
}

void help(const string pname) {
	cout<<"Usage:"<<endl;
	cout<<pname<<" list filename\t\t\tlist of files (formated)"<<endl;
	cout<<pname<<" nlist filename\t\t\tlist of files (name.ext only)"<<endl;
	cout<<pname<<" ctrd filename\t\t\tcreate new TRD"<<endl;
	cout<<pname<<" cscl filename\t\t\tcreate new SCL"<<endl;
	cout<<pname<<" add filename archname\t\tadd file to archive"<<endl;
	cout<<pname<<" pop filename archname\t\textract file from archive"<<endl;
}

int main(int ac,char* av[]) {
	if (ac<3) {help(av[0]); return 1;}
	string mode(av[1]);
	bool yeah=false;
	if (mode=="list") {list(av[2]); yeah=true;}
	if (mode=="nlist") {nlist(av[2]); yeah=true;}
	if (mode=="ctrd") {createtrd(av[2]); yeah=true;}
	if (mode=="cscl") {createscl(av[2]); yeah=true;}
	if (mode=="pop" && ac>3) {extract(av[2],av[3]); yeah=true;}
	if (mode=="add" && ac>3) {pack(av[2],av[3]); yeah=true;}
	if (!yeah) help(av[0]);
	return 0;
}
