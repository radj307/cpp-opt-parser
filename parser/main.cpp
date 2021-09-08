#include <TermAPI.hpp>
#include <optlib.hpp>

#include "format-argv.hpp"
#include "Params.hpp"

using namespace opt;

int main(const int argc, char** argv, char** envp)
{
	try {
		std::cout << sys::term::EnableANSI;


		const auto params{ Params(argc, argv, { "opt", "f"})};
		const auto param{ Param(argc, argv, Matcher{ { 'f' }, {{"opt", true}}}) };
		sizeof(params);
		sizeof(param);


		return 0;
	} catch (std::exception& ex) {
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cout << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}