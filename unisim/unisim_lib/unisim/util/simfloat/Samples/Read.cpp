#include "../host_floating.hh"
// #include "../Floating_gccopt.h"
#include "../host_floating.tcc"
// #include "../Floating_gccopt.template"

using namespace unisim::util::simfloat;

#include <sstream>

template class Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> >;
typedef Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> > Double;

class ExtendedDoubleTraits : public Numerics::Double::BuiltDoubleTraits<57, 11> {
  private:
   typedef Numerics::Double::BuiltDoubleTraits<57, 11> inherited;

  public:
   typedef inherited::TFloatConversion<2, 1> FloatConversion;
   // Since 57 bits holds on two int and 11 holds on one bit
   // and only the conversion towards Double is considered
};
typedef Numerics::Double::TBuiltDouble<ExtendedDoubleTraits> ExtendedDouble;

void call_print_double(void* dDouble) {
   std::cout << ((const Double*) dDouble)->implantation() << std::endl;
}

inline std::ostream&
operator<<(std::ostream& osOut, const Double& bdDouble) {
   bdDouble.write(osOut, Double::WriteParameters());
   return osOut;
}

void call_print_intern_double(void* dDouble) {
   std::cout << *((const Double*) dDouble) << std::endl;
}

void call_print_built_double(void* pDouble) {
   const Double::BuiltDouble& bdDouble = *((const Double::BuiltDouble*) pDouble);
   Double dDouble(bdDouble);
   bdDouble.write(std::cout, Double::WriteParameters().setBinary());
   std::cout << std::endl;
   dDouble.write(std::cout, Double::WriteParameters().setBinary());
   std::cout << std::endl;
   dDouble.write(std::cout, Double::WriteParameters().setDecimal());
   std::cout << std::endl;
}

void call_print_built_extended_double(void* pDouble) {
   const ExtendedDouble& bdDouble = *((const ExtendedDouble*) pDouble);
   bdDouble.write(std::cout, Double::WriteParameters().setBinary());
   std::cout << std::endl;
   bdDouble.write(std::cout, Double::WriteParameters().setDecimal());
   std::cout << std::endl;
}

int main(int argc, char** argv) {
   std::string ssLine;
   std::cin >> ssLine;
   while (ssLine.length() > 1) {
      std::istringstream issString(ssLine.c_str());
      Double::StatusAndControlFlags drpParams;
      Double::BuiltDouble bdRead;
      bdRead.read(issString, drpParams);
      std::cout << "The double read on a 52 bits mantissa is : ";
      call_print_built_double(&bdRead);
      if (drpParams.isApproximate())
         std::cout << "Approximate reading" << std::endl;
      
      std::cout << "A reading with 5 more digits for the mantissa" << std::endl;
      std::cout << "The double read on a 57 bits mantissa is : ";
      std::istringstream issString2(ssLine.c_str());
      ExtendedDouble::StatusAndControlFlags drpParams2;
      ExtendedDouble dRead2;
      dRead2.read(issString2, drpParams2);
      call_print_built_extended_double(&dRead2);
      if (drpParams2.isApproximate())
         std::cout << "Approximate reading" << std::endl;
      ExtendedDouble::StatusAndControlFlags drpParams3;
      ExtendedDouble::FloatConversion fcConversion;
      // 57 should be less than 16*sizeof(unsigned int)
      fcConversion.setSizeMantissa(57).setSizeExponent(11);
      fcConversion.mantissa()[0] = dRead2.queryMantissa()[0];
      fcConversion.mantissa()[1] = dRead2.queryMantissa()[1];
      fcConversion.exponent()[0] = dRead2.queryBasicExponent()[0];
      fcConversion.setPositive(dRead2.isPositive());
      
      // Do the conversion and computes the flags
      bdRead = Double::BuiltDouble(fcConversion, drpParams3);
      std::cout << "The conversion to a 52 bits mantissa returns : ";
      call_print_built_double(&bdRead);
      if (drpParams3.isApproximate())
         std::cout << "Approximate reading" << std::endl;
      std::cin >> ssLine;
   };

   return 0;
}


