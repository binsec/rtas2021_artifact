#include <stdarg.h>

static void itoa(long,char*);
static void itox(unsigned long,char*);
static void lltoa(unsigned long long, char*);

void
vfprint(void (*putchar)(unsigned char), char * format,va_list ap)
{
  char buf[22];
  for(int i=0;format[i]!=0;i++) {
    if(format[i]!='%') putchar(format[i]);
    else
      {
	i++;
	if(format[i]=='i' || format[i] == 'd')
	  {
            int d = va_arg(ap,int);
            itoa(d,buf);
	    for(int j=0;buf[j]!=0;j++) putchar(buf[j]);
	  }
	else if(format[i]=='x')
	  {
            unsigned long d = va_arg(ap,unsigned long);
            itox(d,buf);
	    for(int j=0;buf[j]!=0;j++) putchar(buf[j]);
	  }
	else if(format[i]=='s')
	  {
            char *bif = va_arg(ap,char *);
	    while(*(bif)!=0) putchar(*bif++);
	  }
	else if((format[i]=='l')&&(format[i+1]=='l')&&(format[i+2]=='x'))
	  {
            unsigned long long stk = va_arg(ap,unsigned long long);
	    itox(stk >> 32,buf);
	    for(int j=0;buf[j]!=0;j++) putchar(buf[j]);
	    itox(stk & 0xFFFFFFFFULL,buf);
	    for(int j=0;buf[j]!=0;j++) putchar(buf[j]);            
            i+=2;
	  }
	else if((format[i]=='l')&&(format[i+1]=='l')&&(format[i+2]=='u'))
	  {
            unsigned long long stk = va_arg(ap,unsigned long long);
            lltoa(stk, buf);
	    for(int j=0;buf[j]!=0;j++) putchar(buf[j]);
            i+=2;
	  }
        
        else if(format[i]=='%') putchar('%');
        else {
          char *str = "<unsupported conversion: `";
          while(*str != 0) putchar(*str++);
          putchar(format[i]);
          putchar('\''); putchar('>');
        }
      }
  }
}

void __attribute__ ((format (printf, 2, 3)))
fprint(void (*putchar)(unsigned char), char * format,...){
  va_list ap;
  va_start(ap, format);  
  vfprint(putchar, format, ap);
  va_end(ap);
}


static void itoa(long number, char* aout)
{
  unsigned long i=1,pos=0,div;
  char lbuf[22];

  if(number<0) {aout[0]='-';pos++;number=-number;}
  if(number==0) {lbuf[0]='0';pos++;}
  while(number>0)
    {
      div=number/10;
      lbuf[pos++]='0'+(unsigned char)(number-div*10);
      number=div;
    }
  for(i=0;i<pos;i++) aout[i]=lbuf[pos-i-1];
  aout[pos]='\0';
}

static void lltoa(unsigned long long number, char* aout)
{
  unsigned long long div;
  int pos=0,i;
  char lbuf[22];

  if(number==0LL) {lbuf[0]='0';pos++;}
  while(number>0LL)
    {
      div=number/10LL;
      lbuf[pos++]='0'+(unsigned char)(number-div*10LL);
      number=div;
    }
  for(i=0;i<pos;i++) aout[i]=lbuf[pos-i-1];
  aout[pos]='\0';
}

static void itox(unsigned long number, char* xout)
{
  long i=0,pos=0;
  unsigned long digit;

  while((number>>i)>15) i+=4;
  while(i>=0)
    {
      digit=number>>i;
      xout[pos++]=(digit>9)?'A'+(digit-10):'0'+digit;
      number&=~(15<<i);
      i-=4;
    }
  xout[pos]='\0';
}

#if 0
/* Standalone testing. */


#include <stdio.h>

int main(void){
  char tutu[30];
  itoa(30,tutu);
  puts(tutu);
  fprint(putchar,"Hello %d %x\nWorld %g", 32,32);
  return 0;
}
#endif
