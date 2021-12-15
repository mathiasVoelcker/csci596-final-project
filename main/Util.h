#pragma once
#include "httplib.h"
#include "json.hpp"
using json = nlohmann::json;
constexpr auto URL = "earthquake.usgs.gov";
constexpr auto CA_CERT_FILE = "./ca-bundle.crt";
namespace Util
{
	httplib::Result query(const httplib::Params& params);

	class properties
	{
	public:
		double mag, lat, longitude, depth;
	};

	void from_json(const json& j, properties& p);
};

