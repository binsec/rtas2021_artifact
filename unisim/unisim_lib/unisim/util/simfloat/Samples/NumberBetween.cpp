#include "../host_floating.hh"
// #include "../Floating_gccopt.h"
#include "../host_floating.tcc"
// #include "../Floating_gccopt.template"

#include <sstream>

using namespace unisim::util::simfloat;

template class Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> >;
typedef Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> > Double;

void call_print_double(void* dDouble) {
	std::cout << ((const Double*) dDouble)->implantation() << std::endl;
}

inline std::ostream&
operator<<(std::ostream& osOut, const Double& bdDouble) {
   bdDouble.write(osOut, Double::WriteParameters());
   return osOut;
}

inline std::ostream&
operator<<(std::ostream& osOut, const Double::DiffDouble& bddDiffDouble) {
   bddDiffDouble.write(osOut, Double::DiffDouble::FormatParameters());
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

int main(int argc, char** argv) {
   typedef Numerics::Double::TDoubleElement<Numerics::Double::TFloatingBase<Numerics::Double::DoubleTraits> > Double;

   double dFst, dSnd;
   std::cout << "Enter a first double : ";
   std::cin >> dFst;
   std::cout << "\nEnter a second double : ";
   std::cin >> dSnd;

   Double deFst(dFst), deSnd(dSnd);
   Double::DiffDouble ddDiff = deFst.queryNumberOfFloatsBetween(deSnd);
   call_print_built_double((void*) &((const Double::BuiltDouble&) Double::BuiltDouble(deFst)));
   std::cout << std::endl;
   call_print_built_double((void*) &((const Double::BuiltDouble&) Double::BuiltDouble(deSnd)));
   std::cout << std::endl << ddDiff << std::endl;
   ddDiff.write(std::cout, Double::DiffDouble::FormatParameters().setFullBinary(
            Double::DiffDouble().querySize()));
   std::cout << '\n' << std::endl;

   double dInter;
   std::cout << "\nEnter an intermediary double for asserting |d1,d2| = |d1,d3| = |d3,d2| : ";
   std::cin >> dInter;
   Double deInter(dInter);
   call_print_built_double((void*) &((const Double::BuiltDouble&) Double::BuiltDouble(deInter)));

   Double::DiffDouble ddDiff1 = deFst.queryNumberOfFloatsBetween(deInter);
   Double::DiffDouble ddDiff2 = deInter.queryNumberOfFloatsBetween(deSnd);
   std::cout << std::endl << ddDiff1 << '\t' << ddDiff2 << std::endl;
   if (ddDiff1 + ddDiff2 != ddDiff)
      std::cout << "The assertion was false" << std::endl;
   return 0;
}

