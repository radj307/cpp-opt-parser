#include <TermAPI.hpp>
#include <optlib.hpp>
#include <chrono>

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

		const auto allOpts{ params.findAll("opt")};
		const auto allOpt{ [&param]() -> Param::cont_opt {
			Param::cont_opt vec;
			for (auto opt{ param.get("opt")}; opt != param._opt.end(); opt = param.)
			return vec;
		} };

		using CLK = std::chrono::high_resolution_clock;

		auto t0{ CLK::now() };

		size_t i{ 0u };
		for (auto& it : allOpts)
			std::cout << "[" << i++ << "]\t" << it->name() << ' ' << ( it->hasv() ? it->getv().value() : std::string{ "" } ) << '\n';

		const auto dur1{ CLK::now() - t0 };
		std::cout << "\nTime:\t" << color::f::green << dur1.count() << color::reset << '\n';

		std::cout << '\n';

		t0 = CLK::now();

		i = 0u;
		for (auto& [arg, capture] : allOpt)
			std::cout << "[" << i++ << "]\t" << arg << ' ' << capture << '\n';

		const auto dur2{ CLK::now() - t0 };

		std::cout << "\nTime:\t" << color::f::green << dur2.count() << color::reset << '\n';
		std::cout << "\n\n\n";
		std::cout << "\nTime:\t" << color::f::green << dur1.count() << color::reset;
		std::cout << "\nTime:\t" << color::f::green << dur2.count() << color::reset << '\n';

		std::cout << "\nDiff:\t" << color::f::red << std::abs(dur1.count() - dur2.count()) << color::reset << '\n';

		return 0;
	} catch (std::exception& ex) {
		std::cout << sys::term::error << ex.what() << std::endl;
		return -1;
	} catch (...) {
		std::cout << sys::term::error << "An unknown exception occurred!" << std::endl;
		return -2;
	}
}