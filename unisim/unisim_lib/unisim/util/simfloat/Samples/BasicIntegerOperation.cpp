#include "../integer.hh"
// #include "../Integer_gccopt.h"

#include "../integer.tcc"
// #include "../Integer_gccopt.template"

using namespace unisim::util::simfloat;

template class Numerics::Integer::TBigInt<Numerics::Integer::Details::TIntegerTraits<52> >;
typedef Numerics::Integer::TBigInt<Numerics::Integer::Details::TIntegerTraits<52> > BigInt;

void int_write(void* pvBigInt) {
   const BigInt& biInt = *((const BigInt*) pvBigInt);
   biInt.write(std::cout, BigInt::FormatParameters().setDecimal());
   std::cout << '\n';
   biInt.write(std::cout, BigInt::FormatParameters().setFullBinary(52));
   std::cout << std::endl;
};

int main(int argc, char** argv) {
   BigInt biFst, biSnd, biThd;
   char ch;
   std::cout << "Enter a long int : ";
   biFst.read(std::cin, BigInt::FormatParameters().setDecimal());
   std::cout << "\nEnter a second long int : ";
   biSnd.read(std::cin, BigInt::FormatParameters().setDecimal());
   std::cout << "\nEnter the operation among + - * / : ";
   std::cin >> ch;

   int_write(&biFst);
   int_write(&biSnd);
   
   if (ch == '+')
      biFst += biSnd;
   else if (ch == '-')
      biFst -= biSnd;
   else if (ch == '*')
      biFst *= biSnd;
   else if (ch == '/')
      biFst /= biSnd;

   int_write(&biFst);
   return 0;
}

