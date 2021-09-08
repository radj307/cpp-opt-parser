#include <TermAPI.hpp>

#include "format-argv.hpp"
#include "Params.hpp"

using namespace opt;

int main(const int argc, char** argv, char** envp)
{
	try {
		std::cout << sys::term::EnableANSI;

		std::vector<std::string> pseudo_args{ "-z", "flag-capture", "--extra-dash-chars", "hello", "--opt", "world"};
		std::vector<std::string> capture_args{ "z", "extra-dash-chars"};

		const auto cont{ parseArgs(pseudo_args, capture_args) };

		const auto argcont{ ArgContainer(cont) };
		const auto argcont1{ ArgContainer{ argc, argv, { "z", "extra-dash-chars"}}};


		return 0;
	} catch (std::exception& ex) {
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cout << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}