#pragma once
#include <OPT_PARSER_LIB.h>
#include <Params.hpp>
#include <CaptureStream.hpp> ///< @brief Requires sharedlib!
#include <vectorize-stream.hpp>
#ifdef SHARED_LIB


namespace opt {
	inline ContainerType parseStream(CaptureStream&& cap, const ParserConfig& cfg)
	{
		return parseArgs(std::move(vectorize(std::move(cap._buffer), true, " ")), cfg);
	}
}
#else
#error parseCapturedStream.hpp requires the shared library!
#endif