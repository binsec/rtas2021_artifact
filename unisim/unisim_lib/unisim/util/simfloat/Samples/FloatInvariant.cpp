#include "../host_floating.hh"
// #include "../Floating_gccopt.h"

#include "../host_floating.tcc"
// #include "../Floating_gccopt.template"

using namespace unisim::util::simfloat;

#include <sstream>

template class Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> >;
typedef Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> > Double;

template class Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::FloatTraits> >;
typedef Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::FloatTraits> > Float;

typedef Numerics::Double::TBuiltDouble<Numerics::Double::BuiltDoubleTraits<80, 15> > BuiltLongDouble;

inline std::ostream&
operator<<(std::ostream& osOut, const Double::BuiltDouble& bdDouble) {
   bdDouble.write(osOut, Double::BuiltDouble::WriteParameters());
   return osOut;
}

void call_print_double(void* dDouble) {
	std::cout << ((const Double*) dDouble)->implantation() << std::endl;
}

void call_print_intern_double(void* dDouble) {
	std::cout << *((const Double*) dDouble) << std::endl;
}

void call_print_built_double(void* pDouble) {
   const Double::BuiltDouble& bdDouble = *((const Double::BuiltDouble*) pDouble);
   Double dDouble(bdDouble);
   bdDouble.write(std::cout, Double::WriteParameters().setBinary());
   std::cout << std::endl;
   dDouble.write(std::cout, Double::WriteParameters().setDecimal());
   std::cout << std::endl;
}

void call_print_built_float(void* pFloat) {
   const Float::BuiltDouble& bdFloat = *((const Float::BuiltDouble*) pFloat);
   Float fFloat(bdFloat);
   bdFloat.write(std::cout, Float::WriteParameters().setBinary());
   std::cout << std::endl;
   bdFloat.write(std::cout, Float::WriteParameters().setDecimal());
   std::cout << std::endl;
}

void call_print_built_long_double(void* pLongDouble) {
   const BuiltLongDouble& bdFloat = *((const BuiltLongDouble*) pLongDouble);
   bdFloat.write(std::cout, Float::WriteParameters().setBinary());
   std::cout << std::endl;
   bdFloat.write(std::cout, Float::WriteParameters().setDecimal());
   std::cout << std::endl;
}

void call_print_only_built_double(void* pDouble) {
   const Double::BuiltDouble& bdDouble = *((const Double::BuiltDouble*) pDouble);
   bdDouble.write(std::cout, Double::WriteParameters().setBinary());
   std::cout << std::endl;
   bdDouble.write(std::cout, Double::WriteParameters().setDecimal());
   std::cout << std::endl;
}

int main(int argc, char** argv) {
   Double::StatusAndControlFlags drpParams;
   Double deFst, deSnd, deThd;
   char ch, ch2;
   std::cout << "Enter a first double : ";
   deFst.read(std::cin, drpParams);
   std::cout << "\nEnter a second double";
   deSnd.read(std::cin, drpParams);
   std::cout << "\nEnter the operation code '++' '--' '*+' 'dd' 'ii' 'll' ";
   std::cin >> ch >> ch2;
   if ((ch == '*') && (ch2 == '+')) {
      std::cout << "\nEnter a third double";
      deThd.read(std::cin, drpParams);
   };

   int uResult=0, uResultMax=0, uResultMin=0;
   Float::BuiltDouble bfFst, bfFstMin, bfFstMax;
   BuiltLongDouble bldFst, bldFstMin, bldFstMax;

   Double::StatusAndControlFlags rpParams;
   rpParams.setNearestRound();
   Double::BuiltDouble bdSnd(deSnd), bdFst(deFst), bdThd(deThd);
   Double::BuiltDouble bdFstMin(bdFst), bdFstMax(bdFst);
   call_print_only_built_double(&bdFst);
   call_print_built_double(&bdFst);
   std::cout << '\n';
   call_print_built_double(&bdSnd);
   std::cout << '\n' << std::endl;
   
   const int UDoubleSizeMantissa = bdFst.queryMantissa().querySize();
   const int UDoubleSizeExponent = bdFst.queryBasicExponent().querySize();
   if (ch == 'd') {
      Float::BuiltDouble::FloatConversion fcConversion;
      fcConversion.setSizeMantissa(UDoubleSizeMantissa).setSizeExponent(UDoubleSizeExponent);
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeExponent-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.exponent()[uIndex] = bdFst.queryBasicExponent()[uIndex];
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeMantissa-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.mantissa()[uIndex] = bdFst.queryMantissa()[uIndex];
      bfFst = Float::BuiltDouble(fcConversion, rpParams);
   }
   else if (ch == 'l') {
      BuiltLongDouble::FloatConversion fcConversion;
      fcConversion.setSizeMantissa(UDoubleSizeMantissa).setSizeExponent(UDoubleSizeExponent);
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeExponent-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.exponent()[uIndex] = bdFst.queryBasicExponent()[uIndex];
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeMantissa-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.mantissa()[uIndex] = bdFst.queryMantissa()[uIndex];
      bldFst = BuiltLongDouble(fcConversion, rpParams);
   }
   else if (ch == 'i') {
      Double::BuiltDouble::IntConversion icResult;
      bdFst.retrieveInteger(icResult.setSigned(), rpParams);
      uResult = icResult.queryInt();
   }
   else if (ch == '+')
      bdFst.plusAssign(bdSnd, rpParams);
   else if (ch == '-')
      bdFst.minusAssign(bdSnd, rpParams);
   else if (ch == '*') {
      if (ch2 == '+')
         bdFst.multAndAddAssign(bdSnd, bdThd, rpParams);
      else
         bdFst.multAssign(bdSnd, rpParams);
   }
   else if (ch == '/')
      bdFst.divAssign(bdSnd, rpParams);
   if (ch == 'i')
      std::cout << uResult << std::endl;
   else if (ch == 'd') {
      call_print_built_float(&bfFst);
      if (rpParams.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   }
   else if (ch == 'l') {
      call_print_built_long_double(&bldFst);
      if (rpParams.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   }
   else {
      call_print_built_double(&bdFst);
      if (rpParams.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   };

   Double::StatusAndControlFlags rpParamsMin;
   rpParamsMin.setLowestRound();
   if (ch == 'd') {
      Float::BuiltDouble::FloatConversion fcConversion;
      fcConversion.setSizeMantissa(UDoubleSizeMantissa).setSizeExponent(UDoubleSizeExponent);
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeExponent-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.exponent()[uIndex] = bdFstMin.queryBasicExponent()[uIndex];
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeMantissa-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.mantissa()[uIndex] = bdFstMin.queryMantissa()[uIndex];
      bfFstMin = Float::BuiltDouble(fcConversion, rpParamsMin);
   }
   else if (ch == 'l') {
      BuiltLongDouble::FloatConversion fcConversion;
      fcConversion.setSizeMantissa(UDoubleSizeMantissa).setSizeExponent(UDoubleSizeExponent);
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeExponent-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.exponent()[uIndex] = bdFstMin.queryBasicExponent()[uIndex];
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeMantissa-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.mantissa()[uIndex] = bdFstMin.queryMantissa()[uIndex];
      bldFstMin = BuiltLongDouble(fcConversion, rpParamsMin);
   }
   else if (ch == 'i') {
      Double::BuiltDouble::IntConversion icResult;
      bdFst.retrieveInteger(icResult.setSigned(), rpParamsMin);
      uResultMin = icResult.queryInt();
   }
   else if (ch == '+')
      bdFstMin.plusAssign(bdSnd, rpParamsMin);
   else if (ch == '-')
      bdFstMin.minusAssign(bdSnd, rpParamsMin);
   else if (ch == '*') {
      if (ch2 == '+')
         bdFstMin.multAndAddAssign(bdSnd, bdThd, rpParamsMin);
      else
         bdFstMin.multAssign(bdSnd, rpParamsMin);
   }
   else if (ch == '/')
      bdFstMin.divAssign(bdSnd, rpParamsMin);

   if (ch == 'i')
      std::cout << uResultMin << std::endl;
   else if (ch == 'd') {
      call_print_built_float(&bfFstMin);
      if (rpParamsMin.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   }
   else if (ch == 'l') {
      call_print_built_long_double(&bldFstMin);
      if (rpParamsMin.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   }
   else {
      call_print_built_double(&bdFstMin);

      if (rpParams.isUpApproximate()) {
         if ((bdFstMin.queryNthSuccessor(1U) != bdFst) || !rpParamsMin.isDownApproximate())
            std::cout << "Error: assertion failed" << std::endl;
      }
      else {
         if ((bdFstMin != bdFst) || !rpParams.hasSameApproximation(rpParamsMin))
            std::cout << "Error: assertion failed" << std::endl;
      };
   };
         
   Double::StatusAndControlFlags rpParamsMax;
   rpParamsMax.setHighestRound();
   if (ch == 'd') {
      Float::BuiltDouble::FloatConversion fcConversion;
      fcConversion.setSizeMantissa(UDoubleSizeMantissa).setSizeExponent(UDoubleSizeExponent);
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeExponent-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.exponent()[uIndex] = bdFstMax.queryBasicExponent()[uIndex];
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeMantissa-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.mantissa()[uIndex] = bdFstMax.queryMantissa()[uIndex];
      bfFstMax = Float::BuiltDouble(fcConversion, rpParamsMax);
   }
   else if (ch == 'l') {
      BuiltLongDouble::FloatConversion fcConversion;
      fcConversion.setSizeMantissa(UDoubleSizeMantissa).setSizeExponent(UDoubleSizeExponent);
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeExponent-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.exponent()[uIndex] = bdFstMax.queryBasicExponent()[uIndex];
      for (register int uIndex = 0; uIndex <= (int) ((UDoubleSizeMantissa-1)/(8*sizeof(unsigned int)));
            ++uIndex)
         fcConversion.mantissa()[uIndex] = bdFstMax.queryMantissa()[uIndex];
      bldFstMax = BuiltLongDouble(fcConversion, rpParamsMax);
   }
   else if (ch == 'i') {
      Double::BuiltDouble::IntConversion icResult;
      bdFst.retrieveInteger(icResult.setSigned(), rpParamsMax);
      uResultMax = icResult.queryInt();
   }
   else if (ch == '+')
      bdFstMax.plusAssign(bdSnd, rpParamsMax);
   else if (ch == '-')
      bdFstMax.minusAssign(bdSnd, rpParamsMax);
   else if (ch == '*') {
      if (ch2 == '+')
         bdFstMax.multAndAddAssign(bdSnd, bdThd, rpParamsMax);
      else
         bdFstMax.multAssign(bdSnd, rpParamsMax);
   }
   else if (ch == '/')
      bdFstMax.divAssign(bdSnd, rpParamsMax);

   if (ch == 'i')
      std::cout << uResultMax << std::endl;
   else if (ch == 'd') {
      call_print_built_float(&bfFstMax);
      if (rpParamsMax.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   }
   else if (ch == 'l') {
      call_print_built_long_double(&bldFstMax);
      if (rpParamsMax.isApproximate())
         std::cout << "Approximate computation\n" << std::endl;
   }
   else {
      call_print_built_double(&bdFstMax);

      if (rpParams.isDownApproximate()) {
         if ((bdFstMax.queryNthPredecessor(1U) != bdFst) || !rpParamsMax.isUpApproximate())
           std::cout << "Error: assertion failed" << std::endl;
      }
      else {
         if ((bdFstMax != bdFst) || !rpParams.hasSameApproximation(rpParamsMax))
            std::cout << "Error: assertion failed" << std::endl;
      };
   };

   if (ch == '+')
      deFst.plusAssign(deSnd, rpParams);
   else if (ch == '-')
      deFst.minusAssign(deSnd, rpParams);
   else if (ch == '*') {
      if (ch2 == '+') {
         deFst.multAssign(deSnd, rpParams);
         deFst.plusAssign(deThd, rpParams);
      }
      else
         deFst.multAssign(deSnd, rpParams);
   }
   else if (ch == '/')
      deFst.divAssign(deSnd, rpParams);
   Double::BuiltDouble bdResult(deFst);
   call_print_built_double(&bdResult);

   if (ch == 'd') {
      Float::BuiltDouble bdResult(Float((float) deFst.implantation()));
      call_print_built_float(&bdResult);
      if (bdResult != bfFst)
         std::cout << "Warning: some differences with intern computations" << std::endl;
   }
   else if (ch == 'i') {
      if (uResult != (int) deFst.implantation())
         std::cout << "Warning: some differences with intern computations" << std::endl;
   }
   else if ((ch != 'l') && (bdFst != bdResult))
      std::cout << "Warning: some differences with intern computations" << std::endl;
   
   return 0;
}

